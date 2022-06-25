
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
#ifndef _CST_LIB_GFX_VLK_TEXTURE_H
#define _CST_LIB_GFX_VLK_TEXTURE_H

#include "impl/image.h"
#include "sampler.h"
#include "sg/texture.h"
#include "math/vec4.h"

namespace cst::vlk {

/**
 * TextureVlk implements a texture in vlk.
 */
class TextureVlk : public cst::Texture {
public:
  TextureVlk(device_ptr dev, std::string const &name, image_ptr image,
             sampler_ptr sampler, int layers, int mipLevels, TextureType type);
  ~TextureVlk();

  int getWidth() const override;
  int getHeight() const override;
  int getDepth() const override;
  int getLayers() const override { return layers; }

  uint8_t *getPixels() const override;

  bool isStaged() const override { return true; }

  void load() override;

  VkImageView getImageView() const { return view; }
  sampler_ptr getSampler() const { return sampler; }

private:
  device_ptr dev;
  image_ptr image;
  sampler_ptr sampler;
  int layers;
  VkImageView view = VK_NULL_HANDLE;
};

} // namespace cst::vlk

#endif // _CST_LIB_GFX_TEXTURE_H