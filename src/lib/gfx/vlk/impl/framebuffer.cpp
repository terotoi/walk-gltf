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
#include "framebuffer.h"
#include "image.h"

using namespace cst::vlk;

Framebuffer::Framebuffer(device_ptr dev, VkExtent2D const &size, VkFormat colorFormat, VkSampleCountFlagBits samples,
                         VkRenderPass renderPass, VkImageView swapChainView)
    : dev(dev) {
  createColorImage(size, colorFormat, samples);
  createDepthImage(size, samples);

  std::vector<VkImageView> atts = {colorView, depthView, swapChainView};

  VkFramebufferCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  info.renderPass = renderPass;
  info.attachmentCount = atts.size();
  info.pAttachments = atts.data();
  info.width = size.width;
  info.height = size.height;
  info.layers = 1;

  if (vkCreateFramebuffer(*dev, &info, nullptr, &fb) != VK_SUCCESS)
    throw std::runtime_error("failed to create a framebuffer.");
}

Framebuffer::~Framebuffer() {
  vkDestroyFramebuffer(*dev, fb, nullptr);
  vkDestroyImageView(*dev, depthView, nullptr);
  vkDestroyImageView(*dev, colorView, nullptr);

  delete depthImage;
  delete colorImage;
}

void Framebuffer::createColorImage(VkExtent2D const &size, VkFormat format, VkSampleCountFlagBits samples) {
  colorImage = new ImageVlk(dev, size, format, 
   (VkImageUsageFlagBits)(VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT), samples);

  VkImageViewCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  info.image = *colorImage;
  info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  info.format = format;

  info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  info.subresourceRange.levelCount = 1;
  info.subresourceRange.layerCount = 1;
  if (vkCreateImageView(*dev, &info, nullptr, &colorView) != VK_SUCCESS)
    throw std::runtime_error("failed to create an image view for a color image");
}

void Framebuffer::createDepthImage(VkExtent2D const &size, VkSampleCountFlagBits samples) {
  depthImage = new ImageVlk(dev, size, VK_FORMAT_D32_SFLOAT,
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, samples);

  VkImageViewCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  info.image = *depthImage;
  info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  info.format = VK_FORMAT_D32_SFLOAT;

  info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  info.subresourceRange.levelCount = 1;
  info.subresourceRange.layerCount = 1;
  if (vkCreateImageView(*dev, &info, nullptr, &depthView) != VK_SUCCESS)
    throw std::runtime_error("failed to create an image view a depth image");
}