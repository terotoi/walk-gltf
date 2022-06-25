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
#ifndef _CST_LIB_SG_CAMERA_H
#define _CST_LIB_SG_CAMERA_H

#include "node.h"

namespace cst {

/**
 * Camera sets the projection and view matrices based on its global transform
 * and projection parameters.
 */
class Camera : public Node {
public:
  Camera(std::string const &name = "") : Node(name) {}

  bool isStaged() const override { return true; }
  bool isVisual() const override { return false; }

  mat4 const &getProjection() const { return proj; }
  mat4 const &getView() const { return view; }

  // Returns a perspective projection matrix for the given parameters.
  // This uses the current size for the aspect ratio.
  mat4 perspectiveFor(float fov, float near, float far);

  void setPerspective(float fov, float near, float far) {
    proj = perspectiveFor(fov, near, far);
  }

  void updateGlobalTransform(mat4 const &p) override;
private:
  mat4 proj;
  mat4 view;
  bool active = true;
};

typedef std::shared_ptr<Camera> camera_ptr;

} // namespace cst

#endif // _CST_LIB_SG_CAMERA_H
