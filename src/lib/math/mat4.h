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
#ifndef _CST_LIB_MATH_MAT4_H
#define _CST_LIB_MATH_MAT4_H

#include <cmath>

#include "vec4.h"

namespace cst {

/*************************************************************
 * mat4                                                      *
 * Uses the OpenGL storage format (translation is elements   *
 * 12, 13, 14)                                               *
 ************************************************************/
struct mat4 {
  float m[16];

  // Creates an identity matrix.
  mat4();

  // Creates a matrix from values. Values are in column-major storage format.
  mat4(float m0, float m1, float m2, float m3, float m4, float m5, float m6,
       float m7, float m8, float m9, float m10, float m11, float m12, float m13,
       float m14, float m15);

  mat4(std::vector<double> const &v);

  mat4 transpose() const;
  mat4 invert() const;

  bool operator==(mat4 const &b) const;
  bool operator!=(mat4 const &b) const;

  vec3 getForward() const { return vec3(m[0], m[1], m[2]); }

  vec3 getTranslation() const { return vec3(m[12], m[13], m[14]); }

  mat4 getRotation() const {
    return mat4(m[0], m[1], m[2], 0.0f,  //
                m[4], m[5], m[6], 0.0f,  //
                m[8], m[9], m[10], 0.0f, //
                0.0f, 0.0f, 0.0f, 1.0f);
  }

  // Returns the Z-axis of the rotation part of this matrix.
  // This is often the axis that you want to use for a camera.
  vec3 getRotZ() const {
    return vec3(m[8], m[9], m[10]);
  }

  static mat4 translate(vec3 const &t);
  static mat4 translate(float x, float y, float z) {
    return translate(vec3(x, y, z));
  }

  static mat4 scale(vec3 const &s);

  // Create a uniform scale matrix.
  static mat4 scale(float s) { return scale(vec3(s, s, s)); }

  // Returns a matrix which rotates d radians around the +X axis.
  static mat4 rot_x(float d);

  // Returns a matrix which rotates d radians around the +Y axis.
  static mat4 rot_y(float d);

  // Returns a matrix which rotates d radians around the +Z axis.
  static mat4 rot_z(float d);

  static mat4 rot_axis(float d, vec3 const &axis);

  static mat4 project(float aspect, float fov, float near, float far);
};

// m . v
vec4 operator*(mat4 const &m, vec4 const &v);

// Alias for m . vec4(v, 1.0f)
inline vec4 operator*(mat4 const &m, vec3 const &v) {
  return m * vec4(v, 1.0f);
}

mat4 operator*(mat4 const &a, mat4 const &b);
} // namespace cst

std::ostream &operator<<(std::ostream &fh, cst::mat4 const &m);

#endif // _CST_LIB_MATH_MAT4_H