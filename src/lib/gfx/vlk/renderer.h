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
#ifndef _CST_LIB_GFX_VLK_RENDERER_H
#define _CST_LIB_GFX_VLK_RENDERER_H

#include "gfx/renderer.h"
#include "impl/commands.h"
#include "impl/descs.h"
#include "impl/framebuffer.h"
#include "impl/renderpass.h"
#include "material.h"
#include "queue_dispatcher.h"
#include "sampler.h"

#include <SDL_video.h>
#include <map>
#include <set>

namespace cst::vlk {

class Canvas;
struct PipelineLayout;
struct Window;

/**
 * RendererVlk is a vulkan renderer.
 */
class RendererVlk : public Renderer {
public:
  RendererVlk(int reqWidth, int reqHeight, bool fullScreen, bool borderless,
              bool grabMouse);
  ~RendererVlk();

  device_ptr getDevice() const { return device; }

  virtual ivec2 const &getViewSize() const override { return viewSize; }

  void setClearColor(vec4 const &color) override { clearColor = color; }

  void setAmbientLight(vec4 const &color) override {
    globalData.ambientColor = color;
  }

  void setLight(uint32_t n, vec3 const &pos, vec3 const &color);

  void setView(mat4 const &projectMat, mat4 const &viewMat,
               vec3 const &viewPos) override {
    std::scoped_lock lock(globalDataMux);
    globalData.project = projectMat;
    globalData.view = viewMat;
    globalData.viewPos = viewPos;
    projView = projectMat * viewMat;
  }

  mat4 const &getProjectionView() const override { return projView; }

  virtual void setElapsed(float time) override { globalData.time = time; }

  node_ptr stage(node_ptr node) override;

  void windowResized() override;

  void render(std::vector<node_ptr> const &nodes) override;
  void flush() override;

private:
  desclayout_ptr createGlobalLayout();
  desclayout_ptr createMaterialDescLayout();
  desclayout_ptr createNodeLayout();

  void createWindow(int reqWidth, int reqHeight, bool fullScreen,
                    bool borderless, bool grabMouse);
  void createPipelineLayout();
  void allocateGlobalSets(int numImages);

  void buildCommands(int frame, int imageIdx, vec4 const &clearColor,
                     std::vector<node_ptr> const &nodes, mat4 const &viewProj);

  // Stages a texture for display. This can entail loading the
  // texture into CPU and then GPU memory.
  texture_ptr stage(texture_ptr tex);

  // Stages a material for display. Returns the staged material
  // which might be different than the original.
  material_ptr stage(material_ptr mat);

  // Stages a mesh for display. Returns the staged mesh
  // which might be different than the original.
  mesh_ptr stage(mesh_ptr mesh);

  QueueDispatcher *dispatcher;
  queue_ptr presentQueue;
  ivec2 viewSize;
  size_t numImages;
  size_t runningFrames;
  std::unique_ptr<Window> window;
  std::shared_ptr<Device> device;
  std::unique_ptr<Canvas> canvas;
  desclayout_ptr globalLayout;
  desclayout_ptr matLayout;
  desclayout_ptr nodeLayout;
  std::shared_ptr<PipelineLayout> pipeLayout;
  descpool_ptr globalPool;
  descpool_ptr materialPool;
  descpool_ptr nodePool;

  std::set<std::shared_ptr<MaterialVlk>> materials;
  std::map<std::string, sampler_ptr> samplers;

  vec4 clearColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
  GlobalData globalData{};
  std::mutex globalDataMux;
  mat4 projView;

  std::vector<descset_ptr> globalSets;
  std::vector<buffer_ptr> globalBufs;

  renderpass_ptr renderPass;
  std::vector<framebuffer_ptr> frameBuffers;
  std::vector<cmdbuf_ptr> cmds;
  queue_ptr gfxQueueDraw, gfxQueueUtil;
  int totalFrames = 0;

  std::vector<std::unique_ptr<std::mutex>> drawMutex;
  std::vector<std::unique_ptr<std::condition_variable>> drawCV;
  std::vector<bool> drawOn = {false};
};
} // namespace cst::vlk

#endif // _CST_LIB_GFX_VLK_RENDERER_H
