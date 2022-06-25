
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
#include "texture.h"
#include <cassert>
#include <cstring>
#include <map>

using namespace cst;

static std::map<std::string, std::weak_ptr<Texture>> textures;

texture_ptr Texture::getNamed(std::string const &name) {
  if (textures.count(name) > 0) {
    std::weak_ptr<Texture> tw = textures[name];

    if (!tw.expired())
      return tw.lock();
  }
  return nullptr;
}

void Texture::storeNamed(texture_ptr tex) { textures[tex->getName()] = tex; }

void Texture::clearCache() { textures.clear(); }

TextureStd::~TextureStd() { stbi_image_free(pix); }

uint8_t *TextureStd::getPixels() const {
  assert(pix != nullptr);
  return pix;
}

void TextureStd::load() {
  if (pix == nullptr) {
    pix = stbi_load(filename.c_str(), &width, &height, &depth, 4);
    if (pix == nullptr)
      throw std::runtime_error(std::string("failed to load image: ") +
                               filename);
    depth = 4;
  }
}
