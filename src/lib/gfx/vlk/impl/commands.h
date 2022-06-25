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
#ifndef _CST_LIB_GFX_VLK_IMPL_COMMANDS_H
#define _CST_LIB_GFX_VLK_IMPL_COMMANDS_H

#include "device.h"

#include "math/vec4.h"

#include <memory>
#include <vulkan/vulkan.h>

namespace cst::vlk {

class Buffer;
typedef std::shared_ptr<Buffer> buffer_ptr;

class DescriptorSet;
typedef std::shared_ptr<DescriptorSet> descset_ptr;

class Pipeline;
typedef std::shared_ptr<Pipeline> pipeline_ptr;

/**
 * CommandPool
 */
class CommandPool {
public:
  CommandPool(device_ptr dev, uint32_t queueFamily);
  ~CommandPool();

  operator VkCommandPool() const { return pool; }

  device_ptr dev;

private:
  VkCommandPool pool;
};

typedef std::shared_ptr<CommandPool> cmdpool_ptr;

/**
 * CommandBuffer
 */
class CommandBuffer {
public:
  CommandBuffer(cmdpool_ptr pool, bool fenced);
  ~CommandBuffer();

  operator VkCommandBuffer() const { return cmd; }

  /** Creates a new VkCommandBuffer */
  void create();

  /** Begins command buffer recording. */
  void begin(bool onetime = false);

  /** Copies a buffer into another buffer. */
  void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

  /**
   * Copies a buffer into an image.
   * The receiving image must be in correct layout.
   */
  void copyBuffer(VkBuffer src, VkImage dst, uint32_t width, uint32_t height,
                  uint32_t layers);

  void transitionImageLayout(VkImage image, VkImageLayout oldLayout,
                             VkImageLayout newLayout, int layers,
                             int mipLevels);

  /** Begins a render pass. */
  void beginRenderPass(VkRenderPass pass, VkFramebuffer db,
                       VkExtent2D const &size, cst::vec4 const &clearColor);

  /** Ends a renderpass. */
  void endRenderPass();

  /** Bind a descriptor set using the given pipeline layout. */
  void bindDescriptorSet(uint32_t index, descset_ptr set,
                         VkPipelineLayout layout);

  /** Ends command buffer recording. */
  void end();

  void bindPipeline(pipeline_ptr pipeline);

  void drawIndexed(buffer_ptr vertexBuf, buffer_ptr indexBuf,
                   unsigned int numIndices);

  /** Submits command buffer.
   * If queue is VK_NULL_HANDLE, graphics queue is used. */
  void submit(queue_ptr queue, bool wait);

  void submit(queue_ptr queue, VkSemaphore wait, VkSemaphore signal,
              VkFence frameFence);

private:
  cmdpool_ptr pool;
  VkFence fence = VK_NULL_HANDLE;
  bool signaled = true;
  VkCommandBuffer cmd = VK_NULL_HANDLE;
  std::vector<descset_ptr> descs;
  std::vector<buffer_ptr> buffers;
  std::vector<pipeline_ptr> pipelines;
};

typedef std::shared_ptr<CommandBuffer> cmdbuf_ptr;

} // namespace cst::vlk

#endif // _CST_LIB_GFX_VLK_IMPL_COMMANDS_H
