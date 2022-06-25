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
#ifndef _CST_LIB_GFX_VLK_IMPL_BUFFER_H
#define _CST_LIB_GFX_VLK_IMPL_BUFFER_H

#include <vulkan/vulkan.h>

#include "commands.h"
#include "device.h"
#include "support/vk_mem_alloc.h"

namespace cst::vlk {

class Buffer {
public:
  Buffer(device_ptr dev, size_t size, VkBufferUsageFlags usage,
         VmaAllocationCreateInfo const &allocInfo);
  ~Buffer();

  operator VkBuffer() const { return buf; }

  /**
   * Copies data from src to this buffer.
   * This buffer must be memory mappable.
   */
  void copyFrom(void const *src, size_t size);

  /**
   * Copies data from src to this buffer.
   * This buffer must have allocated with VMA_ALLOCATION_CREATE_MAPPED_BIT;
   */
  void copyDirectFrom(void const *src, size_t size);

  /**
   * Copy contents of this buffer to another buffer.
   */
  void copyTo(cmdpool_ptr pool, queue_ptr queue, VkBuffer dst,
              VkDeviceSize size) const;

private:
  device_ptr dev;
  VkBuffer buf = VK_NULL_HANDLE;
  VmaAllocation mem = VK_NULL_HANDLE;
  void *dst = nullptr; // For VMA_ALLOCATION_CREATE_MAPPED_BIT
};

typedef std::shared_ptr<Buffer> buffer_ptr;

/**
 * Copy data from buffer to another.
 */
void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

/**
 * Create a staging buffer and copy the given data into it.
 */
buffer_ptr createStagingBufferFrom(device_ptr dev, void const *src,
                                   size_t size);

/*
 * Create a buffer in device memory and copy data to from src.
 * Uses an intermediatery staging buffer to handle the copying from main memory.
 * @param usage usage flags, does not need to include
 * VK_BUFFER_USAGE_TRANSFER_DST_BIT
 */
buffer_ptr createBufferFrom(device_ptr dev, cmdpool_ptr pool, queue_ptr queue,
                            VkBufferUsageFlags usage, void const *src,
                            size_t size);

/**
 * Creates an uniform buffer with the specified size in bytes.
 */
buffer_ptr createUniformBuffer(device_ptr dev, size_t size,
                               bool alwaysMapped = false);

} // namespace cst::vlk

#endif // _CST_LIB_GFX_VLK_IMPL_BUFFER_H
