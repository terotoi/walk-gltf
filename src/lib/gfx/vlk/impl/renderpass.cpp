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
#include "renderpass.h"
#include "device.h"

#include <stdexcept>

using namespace cst::vlk;

RenderPass::RenderPass(device_ptr dev, VkFormat format) : dev(dev) {
  VkAttachmentDescription color{};
  color.format = format;
  color.samples = VK_SAMPLE_COUNT_8_BIT;
  color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference color_ref{};
  color_ref.attachment = 0;
  color_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depth{};
  depth.format = VK_FORMAT_D32_SFLOAT;
  depth.samples = VK_SAMPLE_COUNT_8_BIT;
  depth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depth.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depth_ref{};
  depth_ref.attachment = 1;
  depth_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription resolve{};
  resolve.format = format;
  resolve.samples = VK_SAMPLE_COUNT_1_BIT;
  resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference resolve_ref{};
  resolve_ref.attachment = 2;
  resolve_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription sub{};
  sub.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  sub.colorAttachmentCount = 1;
  sub.pColorAttachments = &color_ref;
  sub.pDepthStencilAttachment = &depth_ref;
  sub.pResolveAttachments = &resolve_ref;

  // Subpass dependencies
  VkSubpassDependency dep{};
  dep.srcSubpass = VK_SUBPASS_EXTERNAL;
  dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                     VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                     VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                      VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  std::vector<VkAttachmentDescription> atts = {color, depth, resolve};

  VkRenderPassCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  info.attachmentCount = atts.size();
  info.pAttachments = atts.data();
  info.subpassCount = 1;
  info.pSubpasses = &sub;
  info.dependencyCount = 1;
  info.pDependencies = &dep;

  if (vkCreateRenderPass(*dev, &info, nullptr, &pass) != VK_SUCCESS)
    throw std::runtime_error("failed to create a render pass");
}

RenderPass::~RenderPass() { vkDestroyRenderPass(*dev, pass, nullptr); }
