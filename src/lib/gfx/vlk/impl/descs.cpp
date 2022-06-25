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
#include "descs.h"
#include "gfx/vlk/texture.h"

#include <cassert>
#include <iostream>

using namespace cst::vlk;

DescPool::DescPool(device_ptr dev, desclayout_ptr layout, size_t numSets,
                   size_t numUniforms, size_t numSamplers)
    : dev(dev), layout(layout) {
  std::vector<VkDescriptorPoolSize> sizes;

  if (numUniforms > 0) {
    VkDescriptorPoolSize size_uniforms{};
    size_uniforms.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    size_uniforms.descriptorCount = numUniforms * numSets;
    sizes.push_back(size_uniforms);
  }

  if (numSamplers > 0) {
    VkDescriptorPoolSize size_samplers{};
    size_samplers.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    size_samplers.descriptorCount = numSamplers * numSets;
    sizes.push_back(size_samplers);
  }

  VkDescriptorPoolCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  info.poolSizeCount = sizes.size();
  info.pPoolSizes = sizes.data();
  info.maxSets = numSets;

  if (vkCreateDescriptorPool(*dev, &info, nullptr, &pool) != VK_SUCCESS)
    throw std::runtime_error("failed to create a desciptor pool");
}

DescPool::~DescPool() { vkDestroyDescriptorPool(*dev, pool, nullptr); }

DescriptorSet::DescriptorSet(VkDescriptorSet set, descpool_ptr pool)
    : set(set), pool(pool) {}

DescriptorSet::~DescriptorSet() {
  if (vkFreeDescriptorSets(*pool->getDevice(), *pool, 1, &set) != VK_SUCCESS) {
    std::cerr << "error: failed to free descriptor sets\n";
  }
}

void DescriptorSet::bind(int binding, buffer_ptr buf) {
  VkDescriptorBufferInfo info{};
  VkWriteDescriptorSet write{};

  info.buffer = *buf;
  info.range = VK_WHOLE_SIZE;
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.dstSet = set;

  write.dstBinding = binding;
  write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  write.descriptorCount = 1;
  write.pBufferInfo = &info;

  vkUpdateDescriptorSets(*pool->getDevice(), 1, &write, 0, nullptr);
}

void DescriptorSet::bindTexture(int binding, texture_ptr tex) {
  VkWriteDescriptorSet write{};

  VkDescriptorImageInfo info{};
  info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  assert(tex->isStaged());
  std::shared_ptr<TextureVlk> vtex = std::dynamic_pointer_cast<TextureVlk>(tex);
  assert(vtex != nullptr);

  info.imageView = vtex->getImageView();
  info.sampler = *vtex->getSampler();

  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.dstSet = set;
  write.dstBinding = binding;
  write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  write.descriptorCount = 1;
  write.pImageInfo = &info;
  vkUpdateDescriptorSets(*pool->getDevice(), 1, &write, 0, nullptr);
}

std::vector<descset_ptr> cst::vlk::allocSets(size_t num, descpool_ptr pool) {
  std::vector<descset_ptr> sets(num);

  if (num > 0) {
    std::vector<VkDescriptorSetLayout> layouts(num, *pool->getLayout());

    VkDescriptorSetAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.descriptorPool = *pool;
    info.descriptorSetCount = num;
    info.pSetLayouts = layouts.data();

    std::vector<VkDescriptorSet> vksets(num);
    if (vkAllocateDescriptorSets(*pool->getDevice(), &info, vksets.data()) !=
        VK_SUCCESS)
      throw std::runtime_error("failed to allocate descriptor sets");

    for (size_t i = 0; i < num; i++)
      sets[i] = std::make_shared<DescriptorSet>(vksets[i], pool);
  }

  return sets;
}

