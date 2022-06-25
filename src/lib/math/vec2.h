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
#ifndef _CST_LIB_MATH_VEC2_H
#define _CST_LIB_MATH_VEC2_H

#include <ostream>

namespace cst {

/*************************************************************
 * vec2                                                      *
 ************************************************************/
struct vec2 {
  float d[2];

  // Creates a vector with x, y = 0.
  vec2() {
    d[0] = 0.0f;
    d[1] = 0.0f;
  }

  vec2(float x, float y) {
    d[0] = x;
    d[1] = y;
  }

  float x() const { return d[0]; }
  float y() const { return d[1]; }

  float g() const { return d[0]; }
  float a() const { return d[1]; }

  bool operator==(vec2 const &b) const {
    return d[0] == b.d[0] && d[1] == b.d[1];
  }

  vec2 operator-(vec2 const &b) const {
    return vec2(d[0] - b.d[0], d[1] - b.d[1]);
  }
};

std::ostream &operator<<(std::ostream &fh, vec2 const &v);
} // namespace cst

template <> struct std::hash<cst::vec2> {
  size_t operator()(cst::vec2 const &v) const noexcept {
    size_t h1 = std::hash<float>{}(v.d[0]);
    size_t h2 = std::hash<float>{}(v.d[1]);
    return h1 ^ (h2 << 1);
  }
};

#endif // _CST_LIB_MATH_VEC2_H