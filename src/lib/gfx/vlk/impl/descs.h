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
#ifndef _CST_LIB_GFX_VLK_IMPL_DESCS_H
#define _CST_LIB_GFX_VLK_IMPL_DESCS_H

#include "buffer.h"
#include "device.h"

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

namespace cst {
  class Texture;
  typedef std::shared_ptr<Texture> texture_ptr;
} // namespace cst

namespace cst::vlk {

class DescPool;
typedef std::shared_ptr<DescPool> descpool_ptr;

typedef std::shared_ptr<VkDescriptorSetLayout> desclayout_ptr;

/**
 * DescriptorSet
 */
class DescriptorSet {
public:
  DescriptorSet(VkDescriptorSet set, descpool_ptr pool);
  ~DescriptorSet();

  operator VkDescriptorSet() const { return set; }
  descpool_ptr getPool() const { return pool; }

  // Bind a buffer to this descriptor.
  void bind(int binding, buffer_ptr buf);

  // Bind a texture image to this descriptor);
  void bindTexture(int binding, texture_ptr tex);

private:
  VkDescriptorSet set;
  descpool_ptr pool;
  //buffer_ptr buffer;
  //texture_ptr texture;
};

typedef std::shared_ptr<DescriptorSet> descset_ptr;

/**
 * DescPool is a pool from which descriptor sets are allocated.
 */
class DescPool {
public:
  DescPool(device_ptr dev, desclayout_ptr layout, size_t numSets,
           size_t numUniforms, size_t numSamplers);
  ~DescPool();

  operator VkDescriptorPool() const { return pool; }

  device_ptr getDevice() const { return dev; }
  desclayout_ptr getLayout() const { return layout; }

private:
  device_ptr dev;
  desclayout_ptr layout;
  VkDescriptorPool pool = VK_NULL_HANDLE;
};

// Allocate descriptor sets from the pool.
std::vector<descset_ptr> allocSets(size_t num, descpool_ptr pool);

} // namespace cst::vlk

#endif // _CST_LIB_GFX_VLK_IMPL_DESCS_H
