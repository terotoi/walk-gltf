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
#ifndef _CST_LIB_MATH_AABB_H
#define _CST_LIB_MATH_AABB_H

#include "mat4.h"
#include "vertex.h"

namespace cst {
/**
 * AABB - axis-aligned bounding box
 */
struct AABB {
  AABB() {}

  AABB(vec3 const &p1, vec3 const &p2) : p1(p1), p2(p2) {}

  // Construct an AABB from list of vertices.
  AABB(std::vector<vertex> const &vertices);

  // vec3 size() const { return p2 - p1; }

  // Extends the AABB by a vertex.
  void extend(vec3 const &v);

  // Extends this AABB by another AABB.
  void extend(AABB const &aabb) {
    extend(aabb.p1);
    extend(aabb.p2);
  }

  // Returns the area of the AABB (x * z)
  float area() const {
    vec3 const s = p2 - p1;
    return s.x() * s.z();
  }

  // Returns the volume of the AABB (x * y * z)
  float volume() const {
    vec3 const s = p2 - p1;
    return s.x() * s.y() * s.z();
  }

  bool operator==(AABB const &b) const { return p1 == b.p1 && p2 == b.p2; }

  bool operator!=(AABB const &b) const { return p1 != b.p1 || p2 == b.p2; }

  bool isVisible(mat4 const &tr) const;

  // Returns the AABB transformed by the given matrix.
  AABB operator*(mat4 const &tr) const { return AABB(tr * p1, tr * p2); }

  vec3 p1;
  vec3 p2;
};

AABB operator*(mat4 const &tr, AABB const &aabb);
std::ostream &operator<<(std::ostream &os, AABB const &aabb);

} // namespace cst

#endif // _CST_LIB_MATH_AABB_H