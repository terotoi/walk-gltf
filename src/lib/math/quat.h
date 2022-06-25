/*
 Copyright (c) 2022 Tero Oinas

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef _CST_LIB_MATH_QUAT_H
#define _CST_LIB_MATH_QUAT_H

#include "mat4.h"

namespace cst {

/*************************************************************
 * quat                                                      *
 ************************************************************/
struct quat {
  float k[4];

  // Creates an identity quaternion (w=1, x=0, y=0, z=0).
  quat() {
    k[0] = 1.0f;
    k[1] = 0.0f;
    k[2] = 0.0f;
    k[3] = 0.0f;
  }

  // Creates a quaternion with the given values.
  quat(float w, float x, float y, float z) {
    k[0] = w;
    k[1] = x;
    k[2] = y;
    k[3] = z;
  }

  // Loads quaternion from an array of floats.
  // The order of the values is assumed to be in GLTF format, [x y z w].
  quat(std::vector<double> const &v) {
    assert(v.size() == 4);
    k[0] = v[3];
    k[1] = v[0];
    k[2] = v[1];
    k[3] = v[2];
  }

  // Returns a rotation matrix from this quaternion.
  operator mat4() const;
};

}

#endif // _CST_LIB_MATH_QUAT_H
