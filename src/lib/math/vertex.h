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
#ifndef _CST_LIB_MATH_VERTEX_H
#define _CST_LIB_MATH_VERTEX_H

#include "vec2.h"
#include "vec3.h"

namespace cst {

/**
 * vertex
 */
struct vertex {
  alignas(4) vec3 pos;
  alignas(4) vec3 normal;
  alignas(4) vec2 texcoord;
  alignas(4) vec3 tangent;

  bool operator==(const vertex &b) const {
    return pos == b.pos && texcoord == b.texcoord && normal == b.normal &&
           tangent == b.tangent;
  }
};

std::ostream &operator<<(std::ostream &fh, vertex const &v);

} // namespace cst

template <> struct std::hash<cst::vertex> {
  size_t operator()(cst::vertex const &v) const noexcept {
    size_t h1 = hash<cst::vec3>{}(v.pos);
    size_t h2 = hash<cst::vec2>{}(v.texcoord);
    size_t h3 = hash<cst::vec3>{}(v.normal);
    size_t h4 = hash<cst::vec3>{}(v.tangent);
    return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
  }
};

#endif // _CST_LIB_MATH_VERTEX_H