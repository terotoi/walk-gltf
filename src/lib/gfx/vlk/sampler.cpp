
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
#include "sampler.h"

using namespace cst::vlk;

SamplerVlk::SamplerVlk(device_ptr dev, int mipLevels,
                       VkSamplerAddressMode addressMode)
    : dev(dev) {
  VkSamplerCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  info.magFilter = VK_FILTER_LINEAR;
  info.minFilter = VK_FILTER_LINEAR;
  info.addressModeU = addressMode;
  info.addressModeV = addressMode;
  info.addressModeW = addressMode;

  info.anisotropyEnable = VK_TRUE;
  info.maxAnisotropy = 16;
  info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  info.minLod = 0.0f;
  info.maxLod = mipLevels;

  if (vkCreateSampler(*dev, &info, nullptr, &sampler) != VK_SUCCESS)
    throw std::runtime_error("failed to create an image sampler");
}

SamplerVlk::~SamplerVlk() { vkDestroySampler(*dev, sampler, nullptr); }
