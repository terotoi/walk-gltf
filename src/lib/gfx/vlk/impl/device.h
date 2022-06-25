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
#ifndef _CST_LIB_GFX_VLK_IMPL_DEVICE_H
#define _CST_LIB_GFX_VLK_IMPL_DEVICE_H

#include "queue.h"
#include "support/vk_mem_alloc.h"

#include <SDL_video.h>
#include <cassert>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

namespace cst::vlk {

struct QueueCreateInfo;

/**
 * Device abstracts the physical and logical vulkan device.
 */
class Device {
public:
  Device(SDL_Window *window, bool validationLayers = true);
  ~Device();

  operator VkDevice() const { return device; }
  VkInstance getInstance() const { return instance; }
  VkPhysicalDevice getPhysicalDevice() const { return physDev; }
  VmaAllocator getAllocator() const { return allocator; }

  // Canvas needs this to create a swap chain.
  std::vector<uint32_t> getQueueFamilyIndices() const;

  size_t numGfxQueues() const { return gfxQueues.size(); }

  queue_ptr getGfxQueue(size_t index) const {
    return gfxQueues[index % gfxQueues.size()];
  }

  queue_ptr getPresentQueue() const { return presentQueue; }

  // Canvas needs this to create a swap chain.
  VkSurfaceKHR getSurface() const { return surface; }

private:
  void waitIdle() const { vkDeviceWaitIdle(device); }
  void createInstance(bool validation);
  void setupPhysicalDevice();
  void selectQueueFamilies(uint32_t &gfxQueue2Index);
  void createLogicalDevice();
  void setupDebugCallback();
  void createAllocator();

  std::vector<QueueCreateInfo>
  setupCreateQueues(std::vector<VkDeviceQueueCreateInfo> &qinfos,
                    std::vector<float> &pris);

  // Finds a memory suitable for given memory type bit and requirements.
  // Returns memory heap index.
  uint32_t findSuitableMemory(uint32_t type, VkMemoryPropertyFlags props);

  SDL_Window *wnd = nullptr;
  VkInstance instance = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT debugMsg = VK_NULL_HANDLE;
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  VkPhysicalDevice physDev = VK_NULL_HANDLE;

  VkDevice device = VK_NULL_HANDLE;
  VkPhysicalDeviceMemoryProperties memProps;
  VmaAllocator allocator = VK_NULL_HANDLE;

  std::vector<queue_ptr> gfxQueues;
  queue_ptr presentQueue;
};

typedef std::shared_ptr<Device> device_ptr;

} // namespace cst::vlk

#endif // _CST_LIB_GFX_VLK_IMPL_DEVICE_H
