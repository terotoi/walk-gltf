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
#ifndef _CST_LIB_SG_LIGHT_H
#define _CST_LIB_SG_LIGHT_H

#include "node.h"

namespace cst {

class Renderer;
typedef std::shared_ptr<Renderer> renderer_ptr;

/**
 * Light is a light source in the scene.
 */
class Light : public Node {
public:
  Light(int num, vec3 const &color = vec3(1.0f), std::string const &name = "");

  void setColor(vec4 const &color) { this->color = color; }
  vec3 const &getColor() const { return color; }

  bool isStaged() const override { return true; }
  bool isVisual() const override { return false; }

  // Sends the light info to the renderer.
  void update(Renderer *renderer) override;

private:
  int num;
  vec3 color;
};

typedef std::shared_ptr<Light> light_ptr;

} // namespace cst

#endif // _CST_LIB_SG_LIGHT_H
