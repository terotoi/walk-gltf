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
#include "camera.h"
#include "gfx/renderer.h"

using namespace cst;

mat4 Camera::perspectiveFor(float fov, float near, float far) {
  const ivec2 viewSize = getRenderer()->getViewSize();
  return mat4::project(float(viewSize.width()) / viewSize.height(), fov, near,
                       far);
}

void Camera::updateGlobalTransform(mat4 const &p) {
  Node::updateGlobalTransform(p);
  vec3 const &pos = getGlobalPosition();
  mat4 const &rot = getGlobalRotation();
  view = rot.transpose() * mat4::translate(-pos);

  if (active)
    setView(proj, view, pos);
}
