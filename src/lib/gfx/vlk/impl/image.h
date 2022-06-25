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
#ifndef _CST_LIB_GFX_VLK_IMPL_IMAGE_H
#define _CST_LIB_GFX_VLK_IMPL_IMAGE_H

#include <vulkan/vulkan.h>

#include <memory>
#include <mutex>

#include "commands.h"
#include "device.h"
#include "support/vk_mem_alloc.h"

namespace cst::vlk {

class ImageVlk;
typedef std::shared_ptr<ImageVlk> image_ptr;

class ImageVlk {
public:
  // Create an image suitable for a color or depth image.
  ImageVlk(device_ptr dev, VkExtent2D const &size, VkFormat format,
           VkImageUsageFlagBits usage, VkSampleCountFlagBits samples);

  ImageVlk(device_ptr dev, cmdpool_ptr pool, queue_ptr queue, uint8_t *pix,
           int width, int height, int depth, int layers, VkFormat format,
           int mipLevels);
  ~ImageVlk();

  operator VkImage() const { return img; }

private:
  void generateMipmaps(cmdpool_ptr pool, queue_ptr queue, int width, int height,
                       int layers, int mipLevels);

  device_ptr dev;
  VkImage img = VK_NULL_HANDLE;
  VmaAllocation mem = VK_NULL_HANDLE;
};

} // namespace cst::vlk

#endif // _CST_LIB_GFX_VLK_IMPL_IMAGE_H
