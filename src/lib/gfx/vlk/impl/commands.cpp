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
#include "commands.h"
#include "descs.h"
#include "pipeline.h"

#include <iostream>

using namespace cst::vlk;

CommandPool::CommandPool(device_ptr dev, uint32_t queueFamily) : dev(dev) {
  VkCommandPoolCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.queueFamilyIndex = queueFamily;

  if (vkCreateCommandPool(*dev, &info, nullptr, &pool) != VK_SUCCESS)
    throw std::runtime_error("failed to create a command pool");
}

CommandPool::~CommandPool() { vkDestroyCommandPool(*dev, pool, nullptr); }

CommandBuffer::CommandBuffer(cmdpool_ptr pool, bool fenced)
    : pool(pool), cmd(VK_NULL_HANDLE) {
  create();

  if (fenced) {
    VkFenceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if (vkCreateFence(*pool->dev, &info, nullptr, &fence) != VK_SUCCESS)
      throw std::runtime_error("failed to create a fence");
  }
}

CommandBuffer::~CommandBuffer() {
  if (!signaled && fence != VK_NULL_HANDLE) {
    vkWaitForFences(*pool->dev, 1, &fence, VK_TRUE, UINT64_MAX);
  }

  vkFreeCommandBuffers(*pool->dev, *pool, 1, &cmd);
  vkDestroyFence(*pool->dev, fence, nullptr);
}

void CommandBuffer::create() {
  if (cmd != VK_NULL_HANDLE)
    vkFreeCommandBuffers(*pool->dev, *pool, 1, &cmd);

  descs.clear();
  buffers.clear();
  pipelines.clear();

  VkCommandBufferAllocateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info.commandPool = *pool;
  info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  info.commandBufferCount = 1;
  if (vkAllocateCommandBuffers(*pool->dev, &info, &cmd) != VK_SUCCESS)
    throw std::runtime_error("failed to allocate command buffers");
}

void CommandBuffer::begin(bool onetime) {
  VkCommandBufferBeginInfo begin{};
  begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin.flags = onetime ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : 0;
  vkBeginCommandBuffer(cmd, &begin);
}

void CommandBuffer::copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size) {
  VkBufferCopy region{};
  region.size = size;
  vkCmdCopyBuffer(cmd, src, dst, 1, &region);
}

void CommandBuffer::copyBuffer(VkBuffer src, VkImage dst, uint32_t width,
                               uint32_t height, uint32_t layers) {
  VkBufferImageCopy region{};
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.layerCount = layers;
  region.imageExtent = {width, height, 1};
  vkCmdCopyBufferToImage(cmd, src, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                         &region);
}

void CommandBuffer::transitionImageLayout(VkImage image,
                                          VkImageLayout oldLayout,
                                          VkImageLayout newLayout,
                                          int layers,
                                          int mipLevels) {
  VkImageMemoryBarrier b{};
  b.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  b.image = image;
  b.oldLayout = oldLayout;
  b.newLayout = newLayout;
  b.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  b.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

  b.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  b.subresourceRange.levelCount = mipLevels;
  b.subresourceRange.layerCount = layers;

  VkPipelineStageFlags srcStage, dstStage;
  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
      newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    b.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    b.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    b.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else {
    throw std::runtime_error("invalid image layout transition");
  }

  vkCmdPipelineBarrier(cmd, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1,
                       &b);
}

void CommandBuffer::beginRenderPass(VkRenderPass pass, VkFramebuffer fb,
                                    VkExtent2D const &size,
                                    vec4 const &clearColor) {
  VkRenderPassBeginInfo beginPass{};
  beginPass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  beginPass.renderPass = pass;
  beginPass.framebuffer = fb;
  beginPass.renderArea.extent = size;

  VkClearValue color{};
  color.color = {{clearColor.r(), clearColor.g(), clearColor.b(),
                 clearColor.a()}};
  VkClearValue depth = {};
  depth.depthStencil = {1.0f, 0};
  std::vector<VkClearValue> clears = {color, depth};

  beginPass.clearValueCount = clears.size();
  beginPass.pClearValues = clears.data();
  vkCmdBeginRenderPass(cmd, &beginPass, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::endRenderPass() { vkCmdEndRenderPass(cmd); }

void CommandBuffer::bindPipeline(pipeline_ptr pipeline) {
  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);
  pipelines.push_back(pipeline);
}

void CommandBuffer::drawIndexed(buffer_ptr vertexBuf, buffer_ptr indexBuf,
                                unsigned int numIndices) {

  buffers.push_back(vertexBuf);
  buffers.push_back(indexBuf);

  VkDeviceSize offsets{0};
  VkBuffer vbuf = *vertexBuf;
  vkCmdBindVertexBuffers(cmd, 0, 1, &vbuf, &offsets);
  vkCmdBindIndexBuffer(cmd, *indexBuf, 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(cmd, numIndices, 1, 0, 0, 0);
}

void CommandBuffer::bindDescriptorSet(uint32_t index, descset_ptr set,
                                      VkPipelineLayout layout) {
  descs.push_back(set);
  VkDescriptorSet s[] = {*set};
  vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, index,
                          1, s, 0, nullptr);
}

void CommandBuffer::end() { vkEndCommandBuffer(cmd); }

void CommandBuffer::submit(queue_ptr queue, bool wait) {
  VkSubmitInfo submit{};
  submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit.commandBufferCount = 1;
  submit.pCommandBuffers = &cmd;

  vkQueueSubmit(*queue, 1, &submit, fence);
  signaled = false;

  if (wait) {
    assert(fence != VK_NULL_HANDLE);
    if (vkWaitForFences(*pool->dev, 1, &fence, VK_TRUE, UINT64_MAX) !=
        VK_SUCCESS)
      throw std::runtime_error("failed to wait for fence");
    vkResetFences(*pool->dev, 1, &fence);
    signaled = true;

    descs.clear();
    buffers.clear();
    pipelines.clear();
  }
}

void CommandBuffer::submit(queue_ptr queue, VkSemaphore wait, VkSemaphore signal,
                           VkFence frameFence) {

  // Submit commands
  VkSubmitInfo submit{};
  submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submit.waitSemaphoreCount = 1;
  submit.pWaitSemaphores = &wait;
  submit.pWaitDstStageMask = waitStages;
  submit.commandBufferCount = 1;

  submit.pCommandBuffers = &cmd;
  submit.signalSemaphoreCount = 1;
  submit.pSignalSemaphores = &signal;

  if (vkQueueSubmit(*queue, 1, &submit, frameFence) != VK_SUCCESS)
    throw std::runtime_error("failed to submit a command buffer");
  signaled = false;
}