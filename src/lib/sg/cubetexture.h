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
#ifndef _CST_LIB_SG_CUBETEXTURE_H
#define _CST_LIB_SG_CUBETEXTURE_H

#include "texture.h"

namespace cst {

/**
 * CubeTexture is a texture with six images. It is staged as normal
 * texture with the individual images appended together
 * and can be used as a skybox.
 */
class CubeTexture : public Texture {
public:
  CubeTexture(TextureType type,
              std::string const &front, std::string const &back,
              std::string const &top, std::string const &bottom,
              std::string const &right, std::string const &left,
              std::string const &name="");

  ~CubeTexture();

  int getWidth() const override { return width; };

  int getHeight() const override { return height; };

  int getDepth() const override { return depth; };

  int getLayers() const override { return 6; };

  uint8_t *getPixels() const override { return pix; };

  bool isStaged() const override { return false; }

  void load() override;

private:
  std::string const front, back, top, bottom, right, left;
  int width, height, depth;
  uint8_t *pix = nullptr;
};

} // namespace cst

#endif // _CST_LIB_SG_CUBETEXTURE_H