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
#include "material.h"

#include <iostream>
#include <map>

#include "impl/commands.h"

using namespace cst::vlk;

static std::map<std::string, std::weak_ptr<Pipeline>> pipelines;

void cst::vlk::clearPipelineCache() { pipelines.clear(); }

static pipeline_ptr getNamed(std::string const &name) {
  if (pipelines.count(name) > 0) {
    std::weak_ptr<Pipeline> pw = pipelines[name];
    if (!pw.expired())
      return pw.lock();
  }
  return nullptr;
}

MaterialVlk::MaterialVlk(material_ptr src, device_ptr dev,
                         descpool_ptr materialPool, VkExtent2D const &viewSize,
                         VkRenderPass renderPass, VkPipelineLayout pipeLayout)
    : shadeMode(src->getShadeMode()), doubleSided(src->isDoubleSided()),
      albedoTex(src->getAlbedoTex()), roughnessTex(src->getRoughnessTex()),
      normalTex(src->getNormalTex()), roughnessArm(src->isRoughnessArm()) {
  data.albedo = src->getAlbedo();
  data.metallic = src->getMetallic();
  data.roughness = src->getRoughness();
  data.ao = src->getAO();
  data.emissiveColor = src->getEmissiveColor();

  buf = createUniformBuffer(dev, sizeof(MaterialData));
  set = allocSets(1, materialPool)[0];

  set->bind(BIND_MAT_UBO, buf);
  updateUBO();

  bind(dev, viewSize, renderPass, pipeLayout);

  if (albedoTex != nullptr && albedoTex->isStaged()) {
    set->bindTexture((albedoTex->getLayers() == 6) ? BIND_MAT_CUBEMAP
                                                   : BIND_MAT_ALBEDO_TEXTURE,
                     albedoTex);
  }

  if (roughnessTex != nullptr && roughnessTex->isStaged())
    set->bindTexture(BIND_MAT_ROUGHNESS_TEXTURE, roughnessTex);

  if (normalTex != nullptr && normalTex->isStaged())
    set->bindTexture(BIND_MAT_NORMAL_TEXTURE, normalTex);
}

MaterialVlk::~MaterialVlk() {}

void MaterialVlk::bind(device_ptr dev, VkExtent2D const &viewSize,
                       VkRenderPass renderPass, VkPipelineLayout pipeLayout) {
  if (buf == nullptr)
    throw std::runtime_error("material not staged");

  bool cubeMap = albedoTex != nullptr && (albedoTex->getLayers() == 6);

  auto const vertName =
      cubeMap ? "cube"
              : (std::string("default") +
                 ((shadeMode == SHADE_MODE_FLAT) ? "_flat" : "_smooth"));
  std::string fragName;

  if (shadeMode == SHADE_MODE_UNSHADED)
    fragName += "unshaded";
  else if (shadeMode == SHADE_MODE_SMOOTH)
    fragName += "smooth";
  else if (shadeMode == SHADE_MODE_FLAT)
    fragName += "flat";

  if (albedoTex != nullptr && albedoTex->isStaged()) {
    fragName += "_albedo";

    if (cubeMap)
      fragName += "_cube";

    if (roughnessTex != nullptr && roughnessTex->isStaged())
      fragName += roughnessArm ? "_arm" : "_rough";

    if (normalTex != nullptr && normalTex->isStaged())
      fragName += "_norm";
  }

  VkCompareOp depthCompareOp =
      cubeMap ? VK_COMPARE_OP_LESS_OR_EQUAL : VK_COMPARE_OP_LESS;

  std::string const pipeName = vertName + "_" + fragName +
                               (doubleSided ? "_double" : "_single") +
                               (cubeMap ? "_lequal" : "_less");

  pipeline = getNamed(pipeName);
  if (pipeline == nullptr) {
    auto sp = std::make_shared<Pipeline>(dev, viewSize, renderPass, pipeLayout,
                                         vertName, fragName, doubleSided,
                                         depthCompareOp);
    pipelines[pipeName] = sp;
    pipeline = sp;
  }

  assert(pipeline != nullptr);
}

void MaterialVlk::updateUBO() { buf->copyFrom(&data, sizeof(MaterialData)); }

int numPipelineSwaps = 0;

void MaterialVlk::buildCommands(CommandBuffer *cmd, VkPipelineLayout pipeLayout,
                                VkPipeline *currentPipeline) const {
  if (*pipeline != *currentPipeline) {
    assert(*pipeline != nullptr);

    numPipelineSwaps++;
    *currentPipeline = *pipeline;
    cmd->bindPipeline(pipeline);
  }

  cmd->bindDescriptorSet(DESC_SET_MATERIAL, set, pipeLayout);
}
