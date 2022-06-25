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
#ifndef _CST_LIB_GFX_VLK_IMPL_PIPELINE_H
#define _CST_LIB_GFX_VLK_IMPL_PIPELINE_H

#include "device.h"
#include "math/ivec2.h"
#include "shader.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace cst::vlk {

/*************************************************************
 * Pipeline                                                  *
 ************************************************************/
class Pipeline {
public:
  Pipeline(device_ptr dev, VkExtent2D const &size, VkRenderPass renderPass,
           VkPipelineLayout layout, std::string const &vertShaderName,
           std::string const &fragShaderName, bool doubleSided,
           VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS);
  ~Pipeline();

  operator VkPipeline() const { return pipeline; }

private:
  std::vector<VkPipelineShaderStageCreateInfo>
  setupShaderStages(VkShaderModule vertModule, VkShaderModule fragModule);

  void setupFixed(VkExtent2D const &size,
                  VkPipelineInputAssemblyStateCreateInfo &assem,
                  VkViewport &viewport, VkRect2D &scissor,
                  VkPipelineViewportStateCreateInfo &vps,
                  VkPipelineMultisampleStateCreateInfo &ms,
                  VkPipelineColorBlendAttachmentState &ba,
                  VkPipelineColorBlendStateCreateInfo &blend);

  device_ptr dev;
  Shader *vertShader = nullptr; // An unowned reference
  Shader *fragShader = nullptr; // An unowned reference
  VkPipeline pipeline = VK_NULL_HANDLE;
};

typedef std::shared_ptr<Pipeline> pipeline_ptr;

// Sets the root path to shader files.
void setShaderPath(std::string const &path);

// Clears the shader cache.
void clearShaderCache();

} // namespace cst::vlk

#endif // _CST_LIB_GFX_VLK_IMPL_PIPELINE_H
