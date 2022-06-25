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
#include "buffer.h"

#include <cassert>
#include <cstring>
#include <iostream>

#include "commands.h"

using namespace cst::vlk;

Buffer::Buffer(device_ptr dev, size_t size, VkBufferUsageFlags usage,
               VmaAllocationCreateInfo const &allocInfo)
    : dev(dev) {
  VkBufferCreateInfo bufInfo{};
  bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufInfo.size = size;
  bufInfo.usage = usage;
  bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationInfo ainfo{};

  if (vmaCreateBuffer(dev->getAllocator(), &bufInfo, &allocInfo, &buf, &mem,
                      &ainfo) != VK_SUCCESS)
    throw std::runtime_error("failed to allocate a buffer");

  if (allocInfo.flags & VMA_ALLOCATION_CREATE_MAPPED_BIT)
    dst = ainfo.pMappedData;
}

Buffer::~Buffer() { vmaDestroyBuffer(dev->getAllocator(), buf, mem); }

void Buffer::copyFrom(void const *src, size_t size) {
  void *dst;
  vmaMapMemory(dev->getAllocator(), mem, &dst);
  memcpy(dst, src, size);
  vmaUnmapMemory(dev->getAllocator(), mem);
}

void Buffer::copyDirectFrom(void const *src, size_t size) {
  assert(dst);
  memcpy(dst, src, size);
}

void Buffer::copyTo(cmdpool_ptr pool, queue_ptr queue, VkBuffer dst,
                    VkDeviceSize size) const {
  CommandBuffer cmd(pool, true);
  cmd.begin(true);
  cmd.copyBuffer(buf, dst, size);
  cmd.end();
  cmd.submit(queue, true);
}

buffer_ptr cst::vlk::createStagingBufferFrom(device_ptr dev, void const *src,
                                             size_t size) {
  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

  buffer_ptr staging = std::make_shared<Buffer>(
      dev, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, allocInfo);
  staging->copyFrom(src, size);
  return staging;
}

buffer_ptr cst::vlk::createBufferFrom(device_ptr dev, cmdpool_ptr pool,
                                      queue_ptr queue, VkBufferUsageFlags usage,
                                      void const *src, size_t size) {
  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

  Buffer staging =
      Buffer(dev, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, allocInfo);
  staging.copyFrom(src, size);

  allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  buffer_ptr buf = std::make_shared<Buffer>(
      dev, size, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, allocInfo);
  staging.copyTo(pool, queue, *buf, size);

  return buf;
}

buffer_ptr cst::vlk::createUniformBuffer(device_ptr dev, size_t size,
                                         bool alwaysMapped) {
  VkBufferUsageFlags usage =
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; // |
                                          // VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  VmaAllocationCreateInfo allocInfo{};
  allocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
  allocInfo.preferredFlags = 0;

  allocInfo.flags = alwaysMapped ? VMA_ALLOCATION_CREATE_MAPPED_BIT : 0;
  allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

  return std::make_shared<Buffer>(dev, size, usage, allocInfo);
}
