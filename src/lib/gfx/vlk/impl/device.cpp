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
#define SINGLETON_DEFINE_DEV
#define SINGLETON_DEFINE_ALLOCATOR
#define SINGLETON_DEFINE_QUEUES

#include "device.h"

#include <SDL_vulkan.h>

#include <cassert>
#include <iostream>
#include <vector>

using namespace cst::vlk;
using namespace std::literals::chrono_literals;

Device::Device(SDL_Window *window, bool validationLayers) : wnd(window) {
  createInstance(validationLayers);
  setupDebugCallback();

  if (!SDL_Vulkan_CreateSurface(wnd, instance, &surface))
    throw std::runtime_error("failed to create a surface");

  setupPhysicalDevice();
  createLogicalDevice();

  createAllocator();
}

Device::~Device() {
  gfxQueues.clear();
  vmaDestroyAllocator(allocator);

  vkDestroyDevice(device, nullptr);
  vkDestroySurfaceKHR(instance, surface, nullptr);

  auto destroyDebugUtilsMessenger =
      (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          instance, "vkDestroyDebugUtilsMessengerEXT");

  if (destroyDebugUtilsMessenger != nullptr)
    destroyDebugUtilsMessenger(instance, debugMsg, nullptr);
  vkDestroyInstance(instance, nullptr);
}

void Device::createInstance(bool validation) {
  uint32_t num;
  if (SDL_Vulkan_GetInstanceExtensions(wnd, &num, nullptr) == SDL_FALSE)
    throw std::runtime_error("failed to query number of instance extenstions");

  std::vector<const char *> exts(num);
  if (!SDL_Vulkan_GetInstanceExtensions(wnd, &num, exts.data()))
    throw std::runtime_error("failed to query instance extensions");

  exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  VkApplicationInfo app{};
  app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app.pApplicationName = "vlk_walk";
  app.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
  app.pEngineName = "Custom Engine";
  app.engineVersion = VK_MAKE_VERSION(0, 0, 1);
  app.apiVersion = VK_API_VERSION_1_2;

  VkInstanceCreateInfo create{};
  create.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create.pApplicationInfo = &app;
  create.enabledExtensionCount = exts.size();
  create.ppEnabledExtensionNames = exts.data();

  const char *valid_layers[] = {"VK_LAYER_KHRONOS_validation"};
  if (validation) {
    std::cout << "Enabling VK_LAYER_KHRONOS_validation\n";
    create.enabledLayerCount = 1;
    create.ppEnabledLayerNames = valid_layers;
  }
  if (vkCreateInstance(&create, nullptr, &instance) != VK_SUCCESS)
    throw std::runtime_error("failed to create vulkan instance");
}

void Device::setupPhysicalDevice() {
  uint32_t num;
  vkEnumeratePhysicalDevices(instance, &num, nullptr);

  if (num == 0)
    throw std::runtime_error("failed to find a suitable GPU supporting Vulkan");

  std::vector<VkPhysicalDevice> devices(num);
  vkEnumeratePhysicalDevices(instance, &num, devices.data());

  for (auto &dev : devices) {
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(dev, &props);
    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      physDev = dev;
      std::cout << "Selecting GPU " << props.deviceName << "\n";
      break;
    }
  }

  if (physDev == VK_NULL_HANDLE) {
    std::cout << "Falling back to non-discrete GPU.\n";
    physDev = devices[0];
  }

  vkGetPhysicalDeviceMemoryProperties(physDev, &memProps);

  VkPhysicalDeviceFeatures feats;
  vkGetPhysicalDeviceFeatures(physDev, &feats);
}

enum QueueType { QueueGfx = 0, QueuePresent, QueueTransfer };

struct cst::vlk::QueueCreateInfo {
  QueueType type;
  uint32_t family;
  uint32_t index;
};

void Device::createLogicalDevice() {
  std::vector<VkDeviceQueueCreateInfo> qinfos;
  std::vector<float> pris;
  auto qcreates = setupCreateQueues(qinfos, pris);

  VkPhysicalDeviceFeatures feats{};
  feats.samplerAnisotropy = VK_TRUE;
  feats.sampleRateShading = VK_TRUE;

  VkDeviceCreateInfo create{};
  create.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create.pQueueCreateInfos = qinfos.data();
  create.queueCreateInfoCount = qinfos.size();
  create.pEnabledFeatures = &feats;

  std::vector<const char *> exts = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  create.enabledExtensionCount = exts.size();
  create.ppEnabledExtensionNames = exts.data();

  if (vkCreateDevice(physDev, &create, nullptr, &device) != VK_SUCCESS)
    throw std::runtime_error("failed to create logical device");

  for (auto &q : qcreates) {
    VkQueue vkqueue;
    vkGetDeviceQueue(device, q.family, q.index, &vkqueue);

    queue_ptr queue = std::make_shared<Queue>(q.family, vkqueue);
    if (q.type == QueuePresent)
      presentQueue = queue;
    else if (q.type == QueueGfx)
      gfxQueues.push_back(queue);
  }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
              void *pUserData) {
  std::cerr << "debug: " << pCallbackData->pMessage << std::endl;
  throw std::runtime_error("exiting on debug callback");
  return VK_FALSE;
}

void Device::setupDebugCallback() {
  VkDebugUtilsMessengerCreateInfoEXT info{};
  info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  info.messageSeverity = 0 | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  info.pfnUserCallback = debugCallback;

  auto createDebugUtilsMessenger =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          instance, "vkCreateDebugUtilsMessengerEXT");
  if (createDebugUtilsMessenger == nullptr)
    throw std::runtime_error("failed to get vkCreateDebugUtilsMessengerEXT");

  if (createDebugUtilsMessenger(instance, &info, nullptr, &debugMsg) !=
      VK_SUCCESS)
    throw std::runtime_error("failed to setup a debug callback");
}

uint32_t Device::findSuitableMemory(uint32_t type,
                                    VkMemoryPropertyFlags props) {
  for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
    if ((type & (1 << i)) &&
        (memProps.memoryTypes[i].propertyFlags & props) == props)
      return i;
  }

  throw std::runtime_error("failed to find suitable memory");
  return -1;
}

void Device::createAllocator() {
  VmaAllocatorCreateInfo allocatorInfo{};
  allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
  allocatorInfo.physicalDevice = physDev;
  allocatorInfo.device = device;
  allocatorInfo.instance = instance;

  if (vmaCreateAllocator(&allocatorInfo, &allocator) != VK_SUCCESS)
    throw std::runtime_error("failed to create memory allocator");
}

std::vector<QueueCreateInfo>
Device::setupCreateQueues(std::vector<VkDeviceQueueCreateInfo> &qinfos,
                          std::vector<float> &pris) {
  uint32_t numFamilies;
  vkGetPhysicalDeviceQueueFamilyProperties(physDev, &numFamilies, nullptr);

  std::vector<VkQueueFamilyProperties> qfs(numFamilies);
  vkGetPhysicalDeviceQueueFamilyProperties(physDev, &numFamilies, qfs.data());

  // Collect number of queues and wether presentation is available in the
  // queue family.
  std::vector<int> gfx_queues_left;
  std::vector<bool> has_present;

  std::vector<unsigned int> num_gfx_allocated;
  num_gfx_allocated.resize(numFamilies);

  std::vector<bool> present_allocated;
  num_gfx_allocated.resize(numFamilies);

  for (uint32_t i = 0; i < numFamilies; i++) {
    VkBool32 present = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physDev, i, surface, &present);
    gfx_queues_left.push_back(qfs[i].queueCount);
    has_present.push_back(present);
  }

  bool presentFound = false;
  int numGfxFound = 0;
  std::vector<QueueCreateInfo> qcreates;

  // Find a dedicated presentation queue.
  uint32_t presentFamily = -1;
  for (uint32_t i = 0; i < numFamilies; i++) {
    if (has_present[i] && !(qfs[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
      presentFamily = i;
      presentFound = true;
      qcreates.push_back({QueuePresent, i, 0});
      break;
    }
  }

  for (uint32_t i = 0; i < numFamilies; i++) {
    if (gfx_queues_left[i] == 0)
      continue;

    VkQueueFamilyProperties &qf = qfs[i];

    if (qf.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      int num = (2 - numGfxFound) < gfx_queues_left[i] ? (2 - numGfxFound)
                                                       : gfx_queues_left[i];

      for (int j = 0; j < num; j++)
        qcreates.push_back(
            QueueCreateInfo{QueueGfx, i, num_gfx_allocated[i] + j});
      numGfxFound += num;

      gfx_queues_left[i] -= num;
      num_gfx_allocated[i] += num;
    }

    if (!presentFound && has_present[i]) {
      presentFound = true;
      presentFamily = i;
      qcreates.push_back({QueuePresent, i, num_gfx_allocated[i]});
    }
  }

  if (numGfxFound == 0)
    throw std::runtime_error(
        "no suitable graphics queue family found for painting");

  if (!presentFound)
    throw std::runtime_error("no suitable presentation queue family found");

  pris.reserve(numGfxFound + 1);

  // Fill up queue creation info
  for (uint32_t i = 0; i < numFamilies; i++) {
    size_t queue_count = num_gfx_allocated[i];
    if (presentFamily == i)
      queue_count++;

    if (queue_count > 0) {
      VkDeviceQueueCreateInfo qc{};
      qc.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      qc.queueFamilyIndex = i;
      qc.queueCount = queue_count;

      size_t s = pris.size();
      for (uint32_t j = 0; j < queue_count; j++)
        pris.push_back(1.0f);

      qc.pQueuePriorities = &pris[s];
      qinfos.push_back(qc);
    }
  }

  return qcreates;
}

// Canvas needs this to create a swap chain.
std::vector<uint32_t> Device::getQueueFamilyIndices() const {
  std::vector<uint32_t> indices;
  for (auto &q : gfxQueues) {
    if (std::find(indices.begin(), indices.end(), q->getFamily()) ==
        indices.end())
      indices.push_back(q->getFamily());
  }

  indices.push_back(presentQueue->getFamily());
  return indices;
}
