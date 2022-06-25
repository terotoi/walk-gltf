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
#ifndef _CST_LIB_MATH_VEC4_H
#define _CST_LIB_MATH_VEC4_H

#include "vec3.h"

namespace cst {

/*************************************************************
 * vec4                                                      *
 ************************************************************/
struct vec4 {
  float d[4];

  // Creates a vector with x, y, z of 0, w of 1.
  vec4() {
    d[0] = 0.0f;
    d[1] = 0.0f;
    d[2] = 0.0f;
    d[3] = 1.0f;
  }

  // Creates a vector with x, y, z of v, w of 1.
  vec4(float v) {
    d[0] = v;
    d[1] = v;
    d[2] = v;
    d[3] = 1.0f;
  }

  vec4(float x, float y, float z, float w) {
    d[0] = x;
    d[1] = y;
    d[2] = z;
    d[3] = w;
  }

  vec4(vec3 const &v, float w) {
    d[0] = v.d[0];
    d[1] = v.d[1];
    d[2] = v.d[2];
    d[3] = w;
  }

  vec4(std::vector<double> const &v) {
    assert(v.size() == 4);
    d[0] = v[0];
    d[1] = v[1];
    d[2] = v[2];
    d[3] = v[3];
  }

  operator vec3() const { return vec3(d[0], d[1], d[2]); }

  float x() const { return d[0]; }
  float y() const { return d[1]; }
  float z() const { return d[2]; }
  float w() const { return d[3]; }

  float r() const { return d[0]; }
  float g() const { return d[1]; }
  float b() const { return d[2]; }
  float a() const { return d[3]; }

  vec4 operator-() const { return vec4(-d[0], -d[1], -d[2], -d[3]); }
  vec4 operator*(float c) const {
    return vec4(d[0] * c, d[1] * c, d[2] * c, d[3] * c);
  }
  vec4 operator/(float c) const {
    return vec4(d[0] / c, d[1] / c, d[2] / c, d[3] / c);
  }

  vec4 operator+(vec4 const &b) const {
    return vec4(d[0] + b.d[0], d[1] + b.d[1], d[2] + b.d[2], d[3] + b.d[3]);
  }

  vec4 operator-(vec4 const &b) const {
    return vec4(d[0] - b.d[0], d[1] - b.d[1], d[2] - b.d[2], d[3] - b.d[3]);
  }

  vec4 operator*(vec4 const &b) const {
    return vec4(d[0] * b.d[0], d[1] * b.d[1], d[2] * b.d[2], d[3] * b.d[3]);
  }

  bool operator==(vec4 const &b) const {
    return d[0] == b.d[0] && d[1] == b.d[1] && d[2] == b.d[2] && d[3] == b.d[3];
  }

  bool operator!=(vec4 const &b) const {
    return d[0] != b.d[0] || d[1] == b.d[1] || d[2] == b.d[2] || d[3] != b.d[3];
  }

  vec4 &operator=(vec4 const &b) {
    d[0] = b.d[0];
    d[1] = b.d[1];
    d[2] = b.d[2];
    d[3] = b.d[3];
    return *this;
  }

  vec4 &operator=(vec3 const &b) {
    d[0] = b.d[0];
    d[1] = b.d[1];
    d[2] = b.d[2];
    d[3] = 1.0f;
    return *this;
  }
};

std::ostream &operator<<(std::ostream &fh, cst::vec4 const &v);

} // namespace cst

#endif // _CST_LIB_MATH_VEC4_H