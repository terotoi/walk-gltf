
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
#include "cubetexture.h"
#include <cstring>
#include <iostream>

using namespace cst;

static stbi_uc *load_image(std::string const &filename, int &width, int &height,
                           int &depth, int req_comp) {
  stbi_uc *pix = stbi_load(filename.c_str(), &width, &height, &depth, 4);
  if (pix == nullptr)
    throw std::runtime_error(std::string("failed to load image: ") + filename);
  return pix;
}

CubeTexture::CubeTexture(TextureType type,
                         std::string const &front, std::string const &back,
                         std::string const &top, std::string const &bottom,
                         std::string const &right, std::string const &left,
												 std::string const &name)
    : Texture(name, type), front(front), back(back), top(top), bottom(bottom),
      right(right), left(left) {}

CubeTexture::~CubeTexture() { delete[] pix; }

void CubeTexture::load() {
  stbi_uc *front_pix = load_image(front, width, height, depth, 4);
	depth = 4;

  int w, h, d;
  stbi_uc *back_pix = load_image(back, w, h, d, 4);
  if (width != w || height != h)
    throw std::runtime_error("CubeTexture: all faces must have the same size");

  stbi_uc *top_pix = load_image(top, w, h, d, 4);
  if (width != w || height != h)
    throw std::runtime_error("CubeTexture: all faces must have the same size");

  stbi_uc *bottom_pix = load_image(bottom, w, h, d, 4);
  if (width != w || height != h)
    throw std::runtime_error("CubeTexture: all faces must have the same size");

  stbi_uc *right_pix = load_image(right, w, h, d, 4);
  if (width != w || height != h)
    throw std::runtime_error("CubeTexture: all faces must have the same size");

  stbi_uc *left_pix = load_image(left, w, h, d, 4);
  if (width != w || height != h)
    throw std::runtime_error("CubeTexture: all faces must have the same size");

  size_t layer_size = width * height * 4;
	pix = new uint8_t[layer_size * 6];
  memcpy(pix, front_pix, layer_size);
  memcpy(pix + layer_size, back_pix, layer_size);
  memcpy(pix + layer_size * 2, top_pix, layer_size);
  memcpy(pix + layer_size * 3, bottom_pix, layer_size);
  memcpy(pix + layer_size * 4, right_pix, layer_size);
  memcpy(pix + layer_size * 5, left_pix, layer_size);

  stbi_image_free(left_pix);
  stbi_image_free(right_pix);
  stbi_image_free(bottom_pix);
  stbi_image_free(top_pix);
  stbi_image_free(back_pix);
  stbi_image_free(front_pix);
}
