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
#ifndef _CST_LIB_GFX_VLK_IMPL_SWAPCHAIN_H
#define _CST_LIB_GFX_VLK_IMPL_SWAPCHAIN_H

#include <vulkan/vulkan.h>

#include <vector>

#include "framebuffer.h"
#include "renderpass.h"

namespace cst::vlk {

/**
 * Swapchain
 */
class Swapchain {
 public:
  /**
   * Create a new swap chain.
   * @param queueFamilyIndices list of queue families which share access to this
   * swap chain.
   */
  Swapchain(device_ptr dev,
            VkPhysicalDevice physDev,
            VkSurfaceKHR surface,
            std::vector<uint32_t> queueFamilyIndices,
            VkExtent2D const& size,
            bool forceImmediate,
            VkSwapchainKHR old);
  ~Swapchain();

  operator VkSwapchainKHR() const { return sc; }

  size_t getNumImages() const { return imageViews.size(); }
  VkExtent2D getSize() const { return size; }
  VkFormat getFormat() const { return format; }
  std::vector<VkImageView> getImageViews() const { return imageViews; }

  // Create framebuffers for this swap chain.
  std::vector<framebuffer_ptr> createFramebuffers(renderpass_ptr renderPass);

 private:
  void createImageViews();

  device_ptr dev;
  VkSwapchainKHR sc = VK_NULL_HANDLE;
  VkExtent2D size;
  VkFormat format;
  std::vector<VkImage> images;
  std::vector<VkImageView> imageViews;
};

typedef std::shared_ptr<Swapchain> swapchain_ptr;

}  // namespace cst::vlk

#endif  // _CST_LIB_GFX_VLK_IMPL_SWAPCHAIN_H
