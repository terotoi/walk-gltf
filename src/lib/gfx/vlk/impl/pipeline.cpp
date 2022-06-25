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
#include "pipeline.h"
#include "math/vertex.h"

#include <iostream>
#include <map>
#include <memory>

using namespace cst::vlk;

static std::string shaderPath = "./shaders/bin";

static std::map<std::string, shader_ptr> shaders;

void cst::vlk::setShaderPath(std::string const &path) { shaderPath = path; }

static shader_ptr shaderFromCache(device_ptr dev, std::string const &name) {
  if (shaders.count(name)) {
    return shaders[name];
  } else {
    auto sp = std::make_shared<Shader>(dev, shaderPath + "/" + name);
    shaders[name] = sp;
    return sp;
  }
}

void cst::vlk::clearShaderCache() { shaders.clear(); }

Pipeline::Pipeline(device_ptr dev, VkExtent2D const &size,
                   VkRenderPass renderPass, VkPipelineLayout layout,
                   std::string const &vertShaderName,
                   std::string const &fragShaderName, bool doubleSided,
                   VkCompareOp depthCompareOp)
    : dev(dev) {
  vertShader = shaderFromCache(dev, vertShaderName + ".vert.spv").get();
  fragShader = shaderFromCache(dev, fragShaderName + ".frag.spv").get();
  auto stages = setupShaderStages(*vertShader, *fragShader);

  VkGraphicsPipelineCreateInfo pl{};
  pl.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pl.stageCount = stages.size();
  pl.pStages = stages.data();

  /** Vertex attribute **/
  VkVertexInputBindingDescription bdesc{};
  bdesc.binding = 0;
  bdesc.stride = sizeof(vertex);
  bdesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  std::vector<VkVertexInputAttributeDescription> adescs;
  adescs.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex, pos)});
  adescs.push_back(
      {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex, normal)});
  adescs.push_back({2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(vertex, texcoord)});
  adescs.push_back(
      {3, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex, tangent)});

  VkPipelineVertexInputStateCreateInfo input{};
  input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  input.vertexBindingDescriptionCount = 1;
  input.pVertexBindingDescriptions = &bdesc;
  input.vertexAttributeDescriptionCount = adescs.size();
  input.pVertexAttributeDescriptions = adescs.data();

  VkPipelineInputAssemblyStateCreateInfo assem{};
  VkViewport viewport{};
  VkRect2D scissor{};
  VkPipelineViewportStateCreateInfo vps{};

  VkPipelineRasterizationStateCreateInfo rast{};
  rast.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rast.polygonMode = VK_POLYGON_MODE_FILL;
  rast.lineWidth = 1.0f;
  rast.cullMode = doubleSided ? VK_CULL_MODE_NONE : VK_CULL_MODE_BACK_BIT;
  rast.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

  VkPipelineMultisampleStateCreateInfo ms{};
  VkPipelineColorBlendAttachmentState ba{};
  VkPipelineColorBlendStateCreateInfo blend{};
  setupFixed(size, assem, viewport, scissor, vps, ms, ba, blend);

  pl.pVertexInputState = &input;
  pl.pInputAssemblyState = &assem;
  pl.pViewportState = &vps;
  pl.pRasterizationState = &rast;
  pl.pMultisampleState = &ms;
  pl.pColorBlendState = &blend;

  /** Depth and stencil **/
  VkPipelineDepthStencilStateCreateInfo ds{};
  ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  ds.depthTestEnable = VK_TRUE;
  ds.depthWriteEnable = VK_TRUE;
  // ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
  // ds.depthCompareOp = VK_COMPARE_OP_LESS;
  ds.depthCompareOp = depthCompareOp;

  pl.pDepthStencilState = &ds;

  // createLayout(descLayouts);
  pl.layout = layout;

  pl.renderPass = renderPass;

  if (vkCreateGraphicsPipelines(*dev, VK_NULL_HANDLE, 1, &pl, nullptr,
                                &pipeline) != VK_SUCCESS)
    throw std::runtime_error("failed to create a pipeline");
}

Pipeline::~Pipeline() { vkDestroyPipeline(*dev, pipeline, nullptr); }

std::vector<VkPipelineShaderStageCreateInfo>
Pipeline::setupShaderStages(VkShaderModule vertModule,
                            VkShaderModule fragModule) {
  VkPipelineShaderStageCreateInfo vertStage{};
  vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertStage.module = vertModule;
  vertStage.pName = "main";

  VkPipelineShaderStageCreateInfo fragStage{};
  fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragStage.module = fragModule;
  fragStage.pName = "main";

  std::vector<VkPipelineShaderStageCreateInfo> stages{vertStage, fragStage};
  return stages;
}

void Pipeline::setupFixed(VkExtent2D const &size,
                          VkPipelineInputAssemblyStateCreateInfo &assem,
                          VkViewport &viewport, VkRect2D &scissor,
                          VkPipelineViewportStateCreateInfo &vps,
                          VkPipelineMultisampleStateCreateInfo &ms,
                          VkPipelineColorBlendAttachmentState &ba,
                          VkPipelineColorBlendStateCreateInfo &blend) {
  assem.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  assem.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  // assem.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;

  viewport.width = size.width;
  viewport.height = size.height;
  viewport.maxDepth = 1.0f;

  scissor.extent = VkExtent2D{size.width, size.height};

  vps.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  vps.viewportCount = 1;
  vps.pViewports = &viewport;
  vps.scissorCount = 1;
  vps.pScissors = &scissor;

  ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  ms.rasterizationSamples = VK_SAMPLE_COUNT_8_BIT;
  ms.sampleShadingEnable = VK_TRUE;
  ms.minSampleShading = 0.2f; // 1.0f;

  ba.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  blend.attachmentCount = 1;
  blend.pAttachments = &ba;
}
