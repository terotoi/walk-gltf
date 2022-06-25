/*
 Copyright (c) 2022 Tero Oinas

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */
#include "renderer.h"
#include "canvas.h"
#include "impl/image.h"
#include "node.h"
#include "texture.h"

#include <SDL_vulkan.h>
#include <iostream>

#ifndef BUILD_TYPE
#define BUILD_TYPE "Debug"
#endif

using namespace cst;
using namespace cst::vlk;

namespace cst::vlk {

static const int NUM_MATERIAL_SETS = 2000;
static const int NUM_NODE_SETS = 20000;

thread_local cmdpool_ptr cmdPool;

// Handler for Window shared_ptr
struct Window {
  Window(SDL_Window *window) : window(window) {}
  ~Window() { SDL_DestroyWindow(window); }

  SDL_Window *window;
};

// Handler for PipelineLayout shared_ptr
struct PipelineLayout {
  PipelineLayout(device_ptr device, VkPipelineLayout layout)
      : device(device), layout(layout) {}
  ~PipelineLayout() { vkDestroyPipelineLayout(*device, layout, nullptr); }

  operator VkPipelineLayout() const { return layout; }

  device_ptr device;
  VkPipelineLayout layout;
};
} // namespace cst::vlk

RendererVlk::RendererVlk(int reqWidth, int reqHeight, bool fullScreen,
                         bool borderless, bool grabMouse)
    : dispatcher(getQueueDispatcher()) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
    throw std::runtime_error("failed to initialize SDL.");
  }
  atexit(SDL_Quit);

  if (SDL_Vulkan_LoadLibrary(nullptr) == -1)
    throw std::runtime_error("failed to load vulkan library");

  createWindow(reqWidth, reqHeight, fullScreen, borderless, grabMouse);
  setRenderer(this);

  bool validation = std::string(BUILD_TYPE) != "Release";

  device = std::make_shared<Device>(window->window, validation);
  cmdPool = std::make_shared<CommandPool>(device,
                                          device->getGfxQueue(0)->getFamily());
  gfxQueueDraw = device->getGfxQueue(0);
  gfxQueueUtil = device->getGfxQueue((device->numGfxQueues() > 1) ? 1 : 0);
  presentQueue = device->getPresentQueue();

  globalLayout = createGlobalLayout();
  matLayout = createMaterialDescLayout();
  nodeLayout = createNodeLayout();
  createPipelineLayout();

  materialPool =
      std::make_shared<DescPool>(device, matLayout, NUM_MATERIAL_SETS, 1, 0);
  nodePool =
      std::make_shared<DescPool>(device, nodeLayout, NUM_NODE_SETS, 1, 1);

  windowResized();
}

void RendererVlk::createWindow(int reqWidth, int reqHeight, bool fullScreen,
                               bool borderless, bool grabMouse) {
  SDL_DisplayMode mode;
  if (SDL_GetDesktopDisplayMode(0, &mode) != 0) {
    throw std::runtime_error("failed to get desktop display mode");
  }

  std::cout << "Desktop size: " << mode.w << "x" << mode.h << std::endl;

  Uint32 flags = SDL_WINDOW_SHOWN;
  if (fullScreen)
    flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

  if (borderless)
    flags |= SDL_WINDOW_BORDERLESS;

  if (!(fullScreen || borderless || grabMouse))
    flags |= SDL_WINDOW_RESIZABLE;

  flags |= SDL_WINDOW_VULKAN;

  if (reqWidth == 0)
    reqWidth = mode.w;
  if (reqHeight == 0)
    reqHeight = mode.h;

  SDL_Window *wnd =
      SDL_CreateWindow("walk", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       reqWidth, reqHeight, flags);
  if (wnd == nullptr)
    throw std::runtime_error("failed to create a window");

  window = std::make_unique<Window>(wnd);

  if (grabMouse)
    if (SDL_SetRelativeMouseMode(SDL_TRUE))
      throw std::runtime_error("failed to grab mouse");

  SDL_SetWindowInputFocus(wnd);
}

RendererVlk::~RendererVlk() {
  if (canvas != nullptr)
    canvas->waitFences();

  samplers.clear();
  clearPipelineCache();
  clearShaderCache();
  cmds.clear();
  materials.clear();
  frameBuffers.clear();
}

desclayout_ptr RendererVlk::createGlobalLayout() {
  VkDescriptorSetLayoutBinding ubo{};
  ubo.binding = BIND_GLOBAL_UBO;
  ubo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  ubo.descriptorCount = 1;
  ubo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT |
                   VK_SHADER_STAGE_FRAGMENT_BIT; // VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayout layout;

  VkDescriptorSetLayoutCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  info.bindingCount = 1;
  info.pBindings = &ubo;
  if (vkCreateDescriptorSetLayout(*device, &info, nullptr, &layout) !=
      VK_SUCCESS)
    throw std::runtime_error("failed to create global descriptor set layout.");

  return std::shared_ptr<VkDescriptorSetLayout>(
      new VkDescriptorSetLayout(layout), [this](VkDescriptorSetLayout *layout) {
        vkDestroyDescriptorSetLayout(*device, *layout, nullptr);
      });
}

desclayout_ptr RendererVlk::createMaterialDescLayout() {
  VkDescriptorSetLayoutBinding ubo{};
  ubo.binding = BIND_MAT_UBO;
  ubo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  ubo.descriptorCount = 1;
  ubo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding albedo{};
  albedo.binding = BIND_MAT_ALBEDO_TEXTURE;
  albedo.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  albedo.descriptorCount = 1;
  albedo.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding rough{};
  rough.binding = BIND_MAT_ROUGHNESS_TEXTURE;
  rough.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  rough.descriptorCount = 1;
  rough.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding norm{};
  norm.binding = BIND_MAT_NORMAL_TEXTURE;
  norm.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  norm.descriptorCount = 1;
  norm.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding cube{};
  cube.binding = BIND_MAT_CUBEMAP;
  cube.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  cube.descriptorCount = 1;
  cube.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::vector<VkDescriptorSetLayoutBinding> bindings{ubo, albedo, rough, norm,
                                                     cube};

  VkDescriptorSetLayout matLayout;

  VkDescriptorSetLayoutCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  info.bindingCount = bindings.size();
  info.pBindings = bindings.data();
  if (vkCreateDescriptorSetLayout(*device, &info, nullptr, &matLayout) !=
      VK_SUCCESS)
    throw std::runtime_error(
        "failed to create material descriptor set layout.");

  return std::shared_ptr<VkDescriptorSetLayout>(
      new VkDescriptorSetLayout(matLayout),
      [this](VkDescriptorSetLayout *layout) {
        vkDestroyDescriptorSetLayout(*device, *layout, nullptr);
      });
}

desclayout_ptr RendererVlk::createNodeLayout() {
  VkDescriptorSetLayoutBinding ubo{};
  ubo.binding = 0;
  ubo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  ubo.descriptorCount = 1;
  ubo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

  std::vector<VkDescriptorSetLayoutBinding> bindings{ubo};

  VkDescriptorSetLayoutCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  info.bindingCount = bindings.size();
  info.pBindings = bindings.data();

  VkDescriptorSetLayout nodeLayout;
  if (vkCreateDescriptorSetLayout(*device, &info, nullptr, &nodeLayout) !=
      VK_SUCCESS)
    throw std::runtime_error("failed to create node descriptor set layout.");

  return std::shared_ptr<VkDescriptorSetLayout>(
      new VkDescriptorSetLayout(nodeLayout),
      [this](VkDescriptorSetLayout *layout) {
        vkDestroyDescriptorSetLayout(*device, *layout, nullptr);
      });
}

void RendererVlk::createPipelineLayout() {
  std::vector<VkDescriptorSetLayout> dls{*globalLayout, *matLayout,
                                         *nodeLayout};

  VkPipelineLayoutCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  info.setLayoutCount = dls.size();
  info.pSetLayouts = dls.data();

  VkPipelineLayout layout;
  if (vkCreatePipelineLayout(*device, &info, nullptr, &layout) != VK_SUCCESS)
    throw std::runtime_error("failed to create layout");

  pipeLayout = std::make_shared<PipelineLayout>(device, layout);
}

void RendererVlk::allocateGlobalSets(int runningFrames) {
  globalPool =
      std::make_shared<DescPool>(device, globalLayout, runningFrames, 1, 0);
  globalSets = allocSets(runningFrames, globalPool);

  // Delete old buffers
  for (size_t i = 0; i < globalBufs.size(); i++)
    globalBufs[i] = nullptr;

  // Create the new buffers
  globalBufs.resize(runningFrames, nullptr);
  for (size_t i = 0; i < globalBufs.size(); i++) {
    globalBufs[i] = createUniformBuffer(device, sizeof(GlobalData), true);
    globalSets[i]->bind(BIND_GLOBAL_UBO, globalBufs[i]);
  }
}

void RendererVlk::setLight(uint32_t n, vec3 const &pos, vec3 const &color) {
  if (n >= MAX_LIGHTS) {
    std::cerr << "WARNING: too many lights, maximum is defined as "
              << MAX_LIGHTS << "\n";
  } else {
    std::scoped_lock lock(globalDataMux);
    globalData.lightPos[n] = pos;
    globalData.lightColor[n] = color;
    if (globalData.numLights < n + 1)
      globalData.numLights = n + 1;
  }
}

/**
 * Use queueWorker to call these.
 */
texture_ptr RendererVlk::stage(texture_ptr tex) {
  std::scoped_lock lock(tex->mutex());

  if (!tex->isStaged()) {
    texture_ptr texv = Texture::getNamed(tex->getName());

    if (texv != nullptr && texv->isStaged()) {
      std::cout << "Returning from texture " << tex->getName() << "\n";
      return texv;
    } else {

      uint mipLevels;

      if (tex->getLayers() == 1)
        mipLevels =
            std::max(1u, static_cast<uint32_t>(std::floor(std::log2(
                             std::max(tex->getWidth(), tex->getHeight())))));
      else
        mipLevels = 1;

      sampler_ptr sampler;
      std::string samplerName = (tex->getLayers() == 6 ? "edge_" : "repeat_") +
                                std::to_string(mipLevels);

      if (samplers.count(samplerName) > 0)
        sampler = samplers[samplerName];
      else {
        sampler = std::make_shared<SamplerVlk>(
            device, mipLevels,
            (tex->getLayers() == 6) ? VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
                                    : VK_SAMPLER_ADDRESS_MODE_REPEAT);
        samplers[samplerName] = sampler;
      }

      std::mutex m;
      std::condition_variable cv;

      queue_ptr gfxQueue = device->getGfxQueue(1);

      dispatcher->add(
          [this, tex, mipLevels, sampler, gfxQueue, &cv, &texv]() {
            if (cmdPool == nullptr)
              cmdPool =
                  std::make_shared<CommandPool>(device, gfxQueue->getFamily());

            image_ptr image = std::make_shared<ImageVlk>(
                device, cmdPool, gfxQueue, tex->getPixels(), tex->getWidth(),
                tex->getHeight(), tex->getDepth(), tex->getLayers(),
                (tex->getType() == TEXTURE_TYPE_ALBEDO)
                    ? VK_FORMAT_R8G8B8A8_SRGB
                    : VK_FORMAT_R8G8B8A8_UNORM,
                mipLevels);

            texv = std::make_shared<TextureVlk>(device, tex->getName(), image,
                                                sampler, tex->getLayers(),
                                                mipLevels, tex->getType());
            Texture::storeNamed(texv);
            cv.notify_all();
          },
          gfxQueue, "stage texture");

      std::unique_lock lock(m);
      cv.wait(lock);
      return texv;
    }
  }
  return tex;
}

material_ptr RendererVlk::stage(material_ptr mat) {
  // std::cout << "Staging material " << mat << "\n";
  std::scoped_lock lock(mat->mutex());
  if (!mat->isStaged()) {
    if (mat->getAlbedoTex() != nullptr)
      mat->setAlbedoTex(stage(mat->getAlbedoTex()));

    if (mat->getRoughnessTex() != nullptr)
      mat->setRoughnessTex(stage(mat->getRoughnessTex()),
                           mat->isRoughnessArm());

    if (mat->getNormalTex() != nullptr)
      mat->setNormalTex(stage(mat->getNormalTex()));

    std::shared_ptr<MaterialVlk> vmat = std::make_shared<MaterialVlk>(
        mat, device, materialPool, canvas->getSize(), *renderPass, *pipeLayout);
    materials.insert(vmat);
    return vmat;
  }
  return mat;
}

mesh_ptr RendererVlk::stage(mesh_ptr mesh) {
  // std::cout << "Staging mesh " << mesh << "\n";

  bool staged;
  {
    std::scoped_lock lock(mesh->mutex());
    staged = mesh->isStaged();
  }

  if (!staged) {
    std::mutex m;
    std::unique_lock lock(m);
    std::condition_variable cv;

    queue_ptr gfxQueue = device->getGfxQueue(1);

    dispatcher->add(
        [this, &cv, &mesh, gfxQueue]() {
          if (cmdPool == nullptr) {
            cmdPool =
                std::make_shared<CommandPool>(device, gfxQueue->getFamily());
          }

          mesh = std::make_shared<MeshVlk>(mesh, device, cmdPool, materialPool,
                                           gfxQueue);
          cv.notify_all();
        },
        gfxQueue, "stage mesh");
    cv.wait(lock);

    mesh->setMaterial(stage(mesh->getMaterial()));
  }

  return mesh;
}

node_ptr RendererVlk::stage(node_ptr node) {
  // std::scoped_lock lock(node->mutex());

  if (!node->isStaged() && node->isVisual()) {
    if (node->numMeshes() > 0) {
      node->mapMeshes([this](mesh_ptr mesh) { return stage(mesh); });

      return std::make_shared<NodeVlk>(node, nodePool, runningFrames);
    }
  }

  return node;
}

void RendererVlk::windowResized() {
  int wndWidth, wndHeight;
  SDL_GetWindowSize(window->window, &wndWidth, &wndHeight);
  std::cout << "Window size: " << wndWidth << "x" << wndHeight << "\n";

  if (canvas != nullptr)
    canvas->waitFences();
  frameBuffers.clear();

  if (canvas != nullptr) {
    std::scoped_lock lock(canvas->mutex());
    clearPipelineCache();
    cmds.clear();

    canvas->resized(device->getSurface(), {wndWidth, wndHeight});
    if (canvas->getNumImages() != numImages)
      throw std::runtime_error("number of images changed");
    if (canvas->getRunningFrames() != runningFrames)
      throw std::runtime_error("number of running frames changed");
    runningFrames = canvas->getRunningFrames();

    cmds.resize(runningFrames, nullptr);
    for (size_t i = 0; i < runningFrames; i++)
      cmds[i] = std::make_shared<CommandBuffer>(cmdPool, false);

  } else {
    canvas =
        std::make_unique<Canvas>(device, ivec2{wndWidth, wndHeight}, false);
    numImages = canvas->getNumImages();
    runningFrames = canvas->getRunningFrames();

    allocateGlobalSets(runningFrames);

    cmds.resize(runningFrames, nullptr);
    for (size_t i = 0; i < runningFrames; i++)
      cmds[i] = std::make_shared<CommandBuffer>(cmdPool, false);
  }

  for (size_t i = drawMutex.size(); i < runningFrames; i++) {
    drawMutex.push_back(std::make_unique<std::mutex>());
    drawCV.push_back(std::make_unique<std::condition_variable>());
    drawOn.push_back(false);
  }

  renderPass =
      std::make_shared<RenderPass>(device, canvas->getSwapchain()->getFormat());

  frameBuffers = canvas->getSwapchain()->createFramebuffers(renderPass);

  VkExtent2D const &extent = canvas->getSize();
  viewSize = {(int)extent.width, (int)extent.height};

  for (std::shared_ptr<MaterialVlk> mat : materials) {
    std::scoped_lock lock(mat->mutex());
    mat->bind(device, extent, *renderPass, *pipeLayout);
  }
}

void RendererVlk::buildCommands(int frame, int imageIdx, vec4 const &clearColor,
                                std::vector<node_ptr> const &nodes,
                                mat4 const &viewProj) {
  CommandBuffer *cmd = cmds[frame].get();

  cmd->create();
  cmd->begin(true);

  cmd->beginRenderPass(*renderPass, *frameBuffers[imageIdx], canvas->getSize(),
                       clearColor);

  cmd->bindDescriptorSet(DESC_SET_GLOBAL, globalSets[frame], *pipeLayout);

  for (node_ptr const &node : nodes) {
    std::scoped_lock lock(node->mutex());

    std::shared_ptr<NodeVlk> nv = std::dynamic_pointer_cast<NodeVlk>(node);
    if (nv == nullptr)
      throw std::runtime_error("node " + node->getName() + " not staged");

    if (nv->isVisible()) {
      nv->copyTransformToBuffer(frame);

      Material *currentMat = nullptr;
      VkPipeline currentPipeline = nullptr;

      nv->buildCommands(cmd, frame, *pipeLayout.get(), viewProj, &currentMat,
                        &currentPipeline);
    }
  }

  cmd->endRenderPass();
  cmd->end();
}

void RendererVlk::render(std::vector<node_ptr> const &nodes) {
  int frame = totalFrames % runningFrames;
  totalFrames++;

  int imageIdx;
  {
    std::mutex m;
    std::unique_lock lock(m);
    std::condition_variable cv;

    dispatcher->add(
        [this, frame, &imageIdx, &cv, &m]() {
          imageIdx = canvas->acquireImage(frame);

          if (imageIdx == -1) {
            std::cout << "no image acquired" << std::endl;
            throw std::runtime_error("no image acquired");
            return;
          }

          std::scoped_lock plock(m);
          cv.notify_all();
        },
        presentQueue, "acquire image");
    cv.wait(lock);
  }

  drawOn[frame] = true;
  dispatcher->add(
      [this, frame, imageIdx, nodes]() {
        {
          mat4 viewProj;
          {
            std::scoped_lock lock(globalDataMux);
            globalBufs[frame]->copyDirectFrom(&globalData, sizeof(GlobalData));
            viewProj = globalData.project * globalData.view;
          }

          buildCommands(frame, imageIdx, clearColor, nodes, viewProj);
        }

        canvas->draw(frame, cmds[frame], gfxQueueDraw);

        std::unique_lock lock(*drawMutex[frame]);
        drawCV[frame]->notify_all();
        drawOn[frame] = false;
      },
      gfxQueueDraw, "draw");

  dispatcher->add(
      [this, imageIdx, frame]() {
        {
          std::unique_lock lock(*drawMutex[frame]);
          if (drawOn[frame]) {
            drawCV[frame]->wait(lock);
            drawOn[frame] = false;
          }
        }

        // canvas->draw(frame, cmds[imageIdx], gfxQueueDraw);
        canvas->present(frame, imageIdx, *presentQueue);
      },
      presentQueue, "present");
}

void RendererVlk::flush() {
  std::scoped_lock lock(canvas->mutex());
  canvas->waitFences();
  vkDeviceWaitIdle(*device);
}
