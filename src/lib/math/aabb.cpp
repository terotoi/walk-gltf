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
#include "aabb.h"
#include <iostream>

using namespace cst;

template <typename T> int sign(T val) { return (T(0) < val) - (val < T(0)); }

AABB::AABB(std::vector<vertex> const &vertices) {
  for (auto const &v : vertices) {
    extend(v.pos);
  }
}

void AABB::extend(vec3 const &v) {
  if (p1.x() > v.x())
    p1.d[0] = v.d[0];

  if (p1.y() > v.y())
    p1.d[1] = v.d[1];

  if (p1.z() > v.z())
    p1.d[2] = v.d[2];

  if (p2.x() < v.x())
    p2.d[0] = v.d[0];

  if (p2.y() < v.y())
    p2.d[1] = v.d[1];

  if (p2.z() < v.z())
    p2.d[2] = v.d[2];
}

bool is_inside(vec4 const &v) {
  return v.x() >= -v.w() && v.x() <= v.w() && //
         v.y() >= -v.w() && v.y() <= v.w() && //
         v.z() >= 0.0f && v.z() <= v.w();
}

bool AABB::isVisible(mat4 const &tr) const {
  vec4 const k1 = tr * p1;
  vec4 const k2 = tr * p2;
  vec4 const k3 = tr * vec3(p2.x(), p1.y(), p1.z());
  vec4 const k4 = tr * vec3(p2.x(), p1.y(), p2.z());
  vec4 const k5 = tr * vec3(p1.x(), p1.y(), p2.z());
  vec4 const k6 = tr * vec3(p1.x(), p2.y(), p2.z());
  vec4 const k7 = tr * vec3(p1.x(), p2.y(), p1.z());
  vec4 const k8 = tr * vec3(p2.x(), p2.y(), p1.z());

  if ((k1.x() < -k1.w()) && (k2.x() < -k2.w()) && (k3.x() < -k3.w()) &&
      (k4.x() < -k4.w()) && (k5.x() < -k5.w()) && (k6.x() < -k6.w()) &&
      (k7.x() < -k7.w()) && (k8.x() < -k8.w()))
    return false;

  if ((k1.x() > k1.w()) && (k2.x() > k2.w()) && (k3.x() > k3.w()) &&
      (k4.x() > k4.w()) && (k5.x() > k5.w()) && (k6.x() > k6.w()) &&
      (k7.x() > k7.w()) && (k8.x() > k8.w()))
    return false;

  if ((k1.y() < -k1.w()) && (k2.y() < -k2.w()) && (k3.y() < -k3.w()) &&
      (k4.y() < -k4.w()) && (k5.y() < -k5.w()) && (k6.y() < -k6.w()) &&
      (k7.y() < -k7.w()) && (k8.y() < -k8.w()))
    return false;

  if ((k1.y() > k1.w()) && (k2.y() > k2.w()) && (k3.y() > k3.w()) &&
      (k4.y() > k4.w()) && (k5.y() > k5.w()) && (k6.y() > k6.w()) &&
      (k7.y() > k7.w()) && (k8.y() > k8.w()))
    return false;

  if ((k1.z() < 0.0f) && (k2.z() < 0.0f) && (k3.z() < 0.0f) &&
      (k4.z() < 0.0f) && (k5.z() < 0.0f) && (k6.z() < 0.0f) &&
      (k7.z() < 0.0f) && (k8.z() < 0.0f))
    return false;

  if ((k1.z() > k1.w()) && (k2.z() > k2.w()) && (k3.z() > k3.w()) &&
      (k4.z() > k4.w()) && (k5.z() > k5.w()) && (k6.z() > k6.w()) &&
      (k7.z() > k7.w()) && (k8.z() > k8.w()))
    return false;

  return true;
}

AABB cst::operator*(mat4 const &tr, AABB const &aabb) { return aabb * tr; }

std::ostream &cst::operator<<(std::ostream &fh, AABB const &aabb) {
  fh << "aabb{ p1: " << aabb.p1 << " p2: " << aabb.p2 << "}";
  return fh;
}
