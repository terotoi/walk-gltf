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
#ifndef _CST_LIB_GFX_VLK_IMPL_FRAMEBUFFER_H
#define _CST_LIB_GFX_VLK_IMPL_FRAMEBUFFER_H

#include <vulkan/vulkan.h>

#include "device.h"

namespace cst::vlk {

class ImageVlk;

class Framebuffer {
public:
  Framebuffer(device_ptr dev, VkExtent2D const &size, VkFormat colorFormat, VkSampleCountFlagBits samples,
      VkRenderPass renderPass, VkImageView swapChainView);
  ~Framebuffer();

  operator VkFramebuffer() const { return fb; }

private:
  void createColorImage(VkExtent2D const &size, VkFormat format, VkSampleCountFlagBits samples);
  void createDepthImage(VkExtent2D const &size, VkSampleCountFlagBits samples);

  device_ptr dev;
  VkFramebuffer fb;
  ImageVlk *colorImage = nullptr;
  ImageVlk *depthImage = nullptr;
  VkImageView colorView = VK_NULL_HANDLE;
  VkImageView depthView = VK_NULL_HANDLE;
};

typedef std::shared_ptr<Framebuffer> framebuffer_ptr;

} // namespace cst::vlk

#endif // _CST_LIB_GFX_VLK_IMPL_FRAMEBUFFER_H
