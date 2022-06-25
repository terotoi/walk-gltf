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
#ifndef _CST_LIB_MATH_VEC3_H
#define _CST_LIB_MATH_VEC3_H

#include <cassert>
#include <cmath>
#include <ostream>
#include <vector>

namespace cst {

/*************************************************************
 * vec3                                                      *
 ************************************************************/
struct vec3 {
  float d[3];

  // Creates a vector with x, y, z = 0.
  vec3() {
    d[0] = 0.0f;
    d[1] = 0.0f;
    d[2] = 0.0f;
  }

  vec3(float x, float y, float z) {
    d[0] = x;
    d[1] = y;
    d[2] = z;
  }

  vec3(std::vector<double> const &v) {
    assert(v.size() == 3);
    d[0] = v[0];
    d[1] = v[1];
    d[2] = v[2];
  }

  vec3(float s) {
    d[0] = s;
    d[1] = s;
    d[2] = s;
  }

  float x() const { return d[0]; }
  float y() const { return d[1]; }
  float z() const { return d[2]; }

  float r() const { return d[0]; }
  float g() const { return d[1]; }
  float b() const { return d[2]; }

  vec3 operator-() const { return vec3(-d[0], -d[1], -d[2]); }
  vec3 operator*(float c) const { return vec3(d[0] * c, d[1] * c, d[2] * c); }
  vec3 operator/(float c) const { return vec3(d[0] / c, d[1] / c, d[2] / c); }

  vec3 operator+(vec3 const &b) const {
    return vec3(d[0] + b.d[0], d[1] + b.d[1], d[2] + b.d[2]);
  }

  vec3 operator-(vec3 const &b) const {
    return vec3(d[0] - b.d[0], d[1] - b.d[1], d[2] - b.d[2]);
  }

  vec3 operator*(vec3 const &b) const {
    return vec3(d[0] * b.d[0], d[1] * b.d[1], d[2] * b.d[2]);
  }

  bool operator==(vec3 const &b) const {
    return d[0] == b.d[0] && d[1] == b.d[1] && d[2] == b.d[2];
  }

  bool operator!=(vec3 const &b) const {
    return d[0] != b.d[0] || d[1] == b.d[1] || d[2] == b.d[2];
  }

  float len() const { return sqrtf(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]); }

  vec3 normalize() const { return (*this) / len(); }

  float dot(vec3 const &b) const {
    return d[0] * b.d[0] + d[1] * b.d[1] + d[2] * b.d[2];
  }

  // Clip all values of this vector to be between min and max inclusive.
  vec3 clip(float min = 0.0f, float max = 1.0f) {
    return vec3(fmax(fmin(d[0], max), min), fmax(fmin(d[1], max), min),
                fmax(fmin(d[2], max), min));
  }
};

std::ostream &operator<<(std::ostream &fh, cst::vec3 const &v);

} // namespace cst

template <> struct std::hash<cst::vec3> {
  size_t operator()(cst::vec3 const &v) const noexcept {
    size_t h1 = std::hash<float>{}(v.d[0]);
    size_t h2 = std::hash<float>{}(v.d[1]);
    size_t h3 = std::hash<float>{}(v.d[2]);

    return ((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1);
  }
};

#endif // _CST_LIB_MATH_VEC4_H