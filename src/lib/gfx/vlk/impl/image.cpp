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
#include "image.h"

#include <iostream>
#include <map>
#include <string>

#include "buffer.h"
#include "commands.h"
#include "support/stb_image.h"

using namespace cst::vlk;

ImageVlk::ImageVlk(device_ptr dev, VkExtent2D const &size, VkFormat format,
                   VkImageUsageFlagBits usage, VkSampleCountFlagBits samples)
    : dev(dev) {
  VkImageCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  info.imageType = VK_IMAGE_TYPE_2D;
  info.extent.width = size.width;
  info.extent.height = size.height;
  info.extent.depth = 1;
  info.mipLevels = 1;
  info.arrayLayers = 1;
  info.format = format;
  info.usage = usage;
  info.tiling = VK_IMAGE_TILING_OPTIMAL;
  info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  info.samples = samples;

  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  if (vmaCreateImage(dev->getAllocator(), &info, &allocInfo, &img, &mem,
                     nullptr) != VK_SUCCESS) {
    throw std::runtime_error("failed to create a depth image");
  }
}

ImageVlk::ImageVlk(device_ptr dev, cmdpool_ptr pool, queue_ptr queue,
                   uint8_t *pix, int width, int height, int depth, int layers,
                   VkFormat format, int mipLevels)
    : dev(dev) {

  assert(depth == 4);
  assert(layers == 1 || layers == 6);
  VkDeviceSize size = width * height * depth * layers;
  buffer_ptr buf = createStagingBufferFrom(dev, pix, size);

  VkImageCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  info.imageType = VK_IMAGE_TYPE_2D;
  info.extent.width = width;
  info.extent.height = height;
  info.extent.depth = 1;
  info.mipLevels = mipLevels;
  info.arrayLayers = layers;
  info.format = format;
  info.tiling = VK_IMAGE_TILING_OPTIMAL;
  info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
               VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  info.samples = VK_SAMPLE_COUNT_1_BIT;
  info.flags = (layers == 6) ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;

  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  if (vmaCreateImage(dev->getAllocator(), &info, &allocInfo, &img, &mem,
                     nullptr) != VK_SUCCESS) {
    throw std::runtime_error("failed to create an image");
  }

  // Transition image to receive the data, copy the data then transition for
  // read by shader.
  CommandBuffer cmd(pool, true);
  cmd.begin(true);
  cmd.transitionImageLayout(img, VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layers, mipLevels);
  cmd.copyBuffer(*buf, img, width, height, layers);

  cmd.end();
  cmd.submit(queue, false);

  generateMipmaps(pool, queue, width, height, layers, mipLevels);
}

void ImageVlk::generateMipmaps(cmdpool_ptr pool, queue_ptr queue, int width,
                               int height, int layers, int mipLevels) {
  CommandBuffer cmd(pool, true);
  cmd.begin(true);

  VkImageMemoryBarrier br{};
  br.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  br.image = img;
  br.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  br.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  br.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  br.subresourceRange.baseArrayLayer = 0;
  br.subresourceRange.layerCount = layers;
  br.subresourceRange.levelCount = 1;

  int32_t mipWidth = width;
  int32_t mipHeight = height;

  for (int i = 1; i < mipLevels; i++) {
    br.subresourceRange.baseMipLevel = i - 1;
    br.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    br.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    br.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    br.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                         nullptr, 1, &br);

    VkImageBlit blit{};
    blit.srcOffsets[0] = {0, 0, 0};
    blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.srcSubresource.mipLevel = i - 1;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = layers;
    blit.dstOffsets[0] = {0, 0, 0};
    blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1,
                          mipHeight > 1 ? mipHeight / 2 : 1, 1};
    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.dstSubresource.mipLevel = i;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = layers;

    vkCmdBlitImage(cmd, img, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, img,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit,
                   VK_FILTER_LINEAR);

    br.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    br.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    br.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    br.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
                         0, nullptr, 1, &br);

    if (mipWidth > 1)
      mipWidth /= 2;
    if (mipHeight > 1)
      mipHeight /= 2;
  }

  br.subresourceRange.baseMipLevel = mipLevels - 1;
  br.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  br.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  br.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  br.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

  vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0,
                       nullptr, 1, &br);

  cmd.end();
  cmd.submit(queue, true);
}

ImageVlk::~ImageVlk() { vmaDestroyImage(dev->getAllocator(), img, mem); }
