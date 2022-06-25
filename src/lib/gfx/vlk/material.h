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
#ifndef _CST_LIB_GFX_VLK_MATERIAL_H
#define _CST_LIB_GFX_VLK_MATERIAL_H

#include "gfx/shader_data.h"
#include "impl/descs.h"
#include "impl/pipeline.h"
#include "math/vec4.h"
#include "sg/material.h"

#include <memory>
#include <string>
#include <vulkan/vulkan.h>

namespace cst::vlk {

/**
 * Material
 */
class MaterialVlk : public Material {
public:
  MaterialVlk(material_ptr src, device_ptr dev, descpool_ptr materialPool,
              VkExtent2D const &viewSize, VkRenderPass renderPass,
              VkPipelineLayout pipeLayout);
  ~MaterialVlk();

  bool isStaged() const override { return true; }

  /*
   * Material interface
   */
  ShadeMode getShadeMode() const override { return shadeMode; }
  vec4 const &getAlbedo() const override { return data.albedo; }
  void setAlbedo(vec4 const &color) override { data.albedo = color; }

  float getMetallic() const override { return data.metallic; }
  float getRoughness() const override { return data.roughness; }
  float getAO() const override { return data.ao; }

  vec4 const &getEmissiveColor() const override { return data.emissiveColor; }
  virtual void setEmissiveColor(vec4 const &color) override {
    data.emissiveColor = color;
  }

  bool isDoubleSided() const override { return doubleSided; }

  void setAlbedoTex(texture_ptr tex) override {}
  texture_ptr getAlbedoTex() const override { return nullptr; }

  void setRoughnessTex(texture_ptr tex, bool isArm) override {}
  texture_ptr getRoughnessTex() const override { return nullptr; }

  bool isRoughnessArm() const override { return roughnessArm; }

  void setNormalTex(texture_ptr tex) override {}
  texture_ptr getNormalTex() const override { return nullptr; }

  /*
   * Renderer access
   */

  // Must be called after sating and a changed view size or renderPass.
  void bind(device_ptr dev, VkExtent2D const &viewSize, VkRenderPass renderPass,
            VkPipelineLayout pipeLayout);

  // Update the contents of the UBO to match data (MaterialData)
  void updateUBO();

  void buildCommands(CommandBuffer *cmd, VkPipelineLayout pipeLayout,
                     VkPipeline *currentPipeline) const;

private:
  ShadeMode shadeMode;
  bool doubleSided;
  MaterialData data{};

  pipeline_ptr pipeline;
  texture_ptr albedoTex;
  texture_ptr roughnessTex;
  texture_ptr normalTex;
  bool roughnessArm;

  buffer_ptr buf;
  descset_ptr set;
  descset_ptr oldSet;
};

// Clears the pipeline cache and destroys all the pipelines in it.
// Must be called when canvas size, render pass or descriptor set layouts
// have changed.
void clearPipelineCache();

} // namespace cst::vlk

#endif // _CST_LIB_GFX_VLK_MATERIAL_H
