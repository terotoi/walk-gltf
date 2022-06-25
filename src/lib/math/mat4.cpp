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
#include "mat4.h"

using namespace cst;

mat4::mat4()
{
  m[0] = 1.0f;
  m[1] = 0.0f;
  m[2] = 0.0f;
  m[3] = 0.0f;

  m[4] = 0.0f;
  m[5] = 1.0f;
  m[6] = 0.0f;
  m[7] = 0.0f;

  m[8] = 0.0f;
  m[9] = 0.0f;
  m[10] = 1.0f;
  m[11] = 0.0f;

  m[12] = 0.0f;
  m[13] = 0.0f;
  m[14] = 0.0f;
  m[15] = 1.0f;
}

mat4::mat4(float m0, float m1, float m2, float m3, float m4, float m5, float m6,
           float m7, float m8, float m9, float m10, float m11, float m12,
           float m13, float m14, float m15)
{
  m[0] = m0;
  m[1] = m1;
  m[2] = m2;
  m[3] = m3;

  m[4] = m4;
  m[5] = m5;
  m[6] = m6;
  m[7] = m7;

  m[8] = m8;
  m[9] = m9;
  m[10] = m10;
  m[11] = m11;

  m[12] = m12;
  m[13] = m13;
  m[14] = m14;
  m[15] = m15;
}

mat4::mat4(std::vector<double> const &v)
{
  assert(v.size() == 16);
  for (int i = 0; i < 16; i++)
    m[i] = v[i];
}

mat4 mat4::transpose() const
{
  return mat4(m[0], m[4], m[8], m[12],  //
              m[1], m[5], m[9], m[13],  //
              m[2], m[6], m[10], m[14], //
              m[3], m[7], m[11], m[15]);
}

mat4 mat4::invert() const
{
  float a00 = m[0];
  float a01 = m[1];
  float a02 = m[2];
  float a03 = m[3];
  float a10 = m[4];
  float a11 = m[5];
  float a12 = m[6];
  float a13 = m[7];
  float a20 = m[8];
  float a21 = m[9];
  float a22 = m[10];
  float a23 = m[11];
  float a30 = m[12];
  float a31 = m[13];
  float a32 = m[14];
  float a33 = m[15];

  float b00 = a00 * a11 - a01 * a10;
  float b01 = a00 * a12 - a02 * a10;
  float b02 = a00 * a13 - a03 * a10;
  float b03 = a01 * a12 - a02 * a11;
  float b04 = a01 * a13 - a03 * a11;
  float b05 = a02 * a13 - a03 * a12;
  float b06 = a20 * a31 - a21 * a30;
  float b07 = a20 * a32 - a22 * a30;
  float b08 = a20 * a33 - a23 * a30;
  float b09 = a21 * a32 - a22 * a31;
  float b10 = a21 * a33 - a23 * a31;
  float b11 = a22 * a33 - a23 * a32;

  float det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

  if (det == 0.0f)
    throw std::runtime_error("mat4::invert: zero determinant");

  det = 1.0 / det;

  mat4 r;
  float *out = r.m;
  out[0] = (a11 * b11 - a12 * b10 + a13 * b09) * det;
  out[1] = (a02 * b10 - a01 * b11 - a03 * b09) * det;
  out[2] = (a31 * b05 - a32 * b04 + a33 * b03) * det;
  out[3] = (a22 * b04 - a21 * b05 - a23 * b03) * det;
  out[4] = (a12 * b08 - a10 * b11 - a13 * b07) * det;
  out[5] = (a00 * b11 - a02 * b08 + a03 * b07) * det;
  out[6] = (a32 * b02 - a30 * b05 - a33 * b01) * det;
  out[7] = (a20 * b05 - a22 * b02 + a23 * b01) * det;
  out[8] = (a10 * b10 - a11 * b08 + a13 * b06) * det;
  out[9] = (a01 * b08 - a00 * b10 - a03 * b06) * det;
  out[10] = (a30 * b04 - a31 * b02 + a33 * b00) * det;
  out[11] = (a21 * b02 - a20 * b04 - a23 * b00) * det;
  out[12] = (a11 * b07 - a10 * b09 - a12 * b06) * det;
  out[13] = (a00 * b09 - a01 * b07 + a02 * b06) * det;
  out[14] = (a31 * b01 - a30 * b03 - a32 * b00) * det;
  out[15] = (a20 * b03 - a21 * b01 + a22 * b00) * det;
  return r;
}

bool mat4::operator==(mat4 const &b) const
{
  return m[0] == b.m[0] && m[1] == b.m[1] && m[2] == b.m[2] && m[3] == b.m[3] &&
         m[4] == b.m[4] && m[5] == b.m[5] && m[6] == b.m[6] && m[7] == b.m[7] &&
         m[8] == b.m[8] && m[9] == b.m[9] && m[10] == b.m[10] &&
         m[11] == b.m[11] && m[12] == b.m[12] && m[13] == b.m[13] &&
         m[14] == b.m[14] && m[15] == b.m[15];
}

bool mat4::operator!=(mat4 const &b) const
{
  return m[0] != b.m[0] || m[1] != b.m[1] || m[2] != b.m[2] ||
         m[3] != b.m[3] || m[4] != b.m[4] || m[5] != b.m[5] || m[6] != b.m[6] ||
         m[7] != b.m[7] || m[8] != b.m[8] || m[9] != b.m[9] ||
         m[10] != b.m[10] || m[11] != b.m[11] || m[12] != b.m[12] ||
         m[13] != b.m[13] || m[14] != b.m[14] || m[15] != b.m[15];
}

mat4 mat4::translate(vec3 const &t)
{
  return mat4(1.0f, 0.0f, 0.0f, 0.0f, //
              0.0f, 1.0f, 0.0f, 0.0f, //
              0.0f, 0.0f, 1.0f, 0.0f, //
              t.d[0], t.d[1], t.d[2], 1.0f);
}

mat4 mat4::scale(vec3 const &s)
{
  return mat4(s.d[0], 0.0f, 0.0f, 0.0f, //
              0.0f, s.d[1], 0.0f, 0.0f, //
              0.0f, 0.0f, s.d[2], 0.0f, //
              0.0f, 0.0f, 0.0f, 1.0f);
}

mat4 mat4::rot_x(float d)
{
  mat4 r;
  float *m = r.m;
  m[5] = cos(d);
  m[6] = sin(d);
  m[9] = -sin(d);
  m[10] = cos(d);
  return r;
}

mat4 mat4::rot_y(float d)
{
  mat4 r;
  float *m = r.m;
  m[0] = cos(d);
  m[2] = -sin(d);
  m[8] = sin(d);
  m[10] = cos(d);
  return r;
}

mat4 mat4::rot_z(float d)
{
  mat4 r;
  float *m = r.m;
  m[0] = cos(d);
  m[1] = sin(d);
  m[4] = -sin(d);
  m[5] = cos(d);
  return r;
}

mat4 mat4::rot_axis(float d, vec3 const &axis)
{
  float c = cos(d);
  float oc = 1 - c;
  float s = sin(d);
  float x = axis.d[0];
  float y = axis.d[1];
  float z = axis.d[2];

  mat4 r;
  float *m = r.m;
  m[0] = oc * x * x + c;
  m[1] = oc * x * y + s * z;
  m[2] = oc * x * z - s * y;

  m[4] = oc * x * y - s * z;
  m[5] = oc * y * y + c;
  m[6] = oc * y * z + s * x;

  m[8] = oc * x * z + s * y;
  m[9] = oc * y * z - s * x;
  m[10] = oc * z * z + c;
  return r;
}

float deg2rad(float d) { return d * M_PI / 180.0f; }

mat4 mat4::project(float aspect, float fov, float near, float far)
{
  float f = 1.0f / tan(deg2rad(0.5f * fov));

  return mat4(                               //
      f / aspect, 0.0f, 0.0f, 0.0f,          //
      0.0f, -f, 0.0f, 0.0f,                  //
      0.0f, 0.0f, far / (near - far), -1.0f, //
      0.0f, 0.0f, (near * far) / (near - far), 0.0f);
}

vec4 cst::operator*(mat4 const &mat, vec4 const &vec)
{
  float const *m = mat.m;
  float const *v = vec.d;

  return vec4(m[0] * v[0] + m[4] * v[1] + m[8] * v[2] + m[12] * v[3],
              m[1] * v[0] + m[5] * v[1] + m[9] * v[2] + m[13] * v[3],
              m[2] * v[0] + m[6] * v[1] + m[10] * v[2] + m[14] * v[3],
              m[3] * v[0] + m[7] * v[1] + m[11] * v[2] + m[15] * v[3]);
}

mat4 cst::operator*(mat4 const &am, mat4 const &bm)
{
  float const *a = am.m;
  float const *b = bm.m;

  return mat4(a[0] * b[0] + a[4] * b[1] + a[8] * b[2] + a[12] * b[3],
              a[1] * b[0] + a[5] * b[1] + a[9] * b[2] + a[13] * b[3],
              a[2] * b[0] + a[6] * b[1] + a[10] * b[2] + a[14] * b[3],
              a[3] * b[0] + a[7] * b[1] + a[11] * b[2] + a[15] * b[3],

              a[0] * b[4] + a[4] * b[5] + a[8] * b[6] + a[12] * b[7],
              a[1] * b[4] + a[5] * b[5] + a[9] * b[6] + a[13] * b[7],
              a[2] * b[4] + a[6] * b[5] + a[10] * b[6] + a[14] * b[7],
              a[3] * b[4] + a[7] * b[5] + a[11] * b[6] + a[15] * b[7],

              a[0] * b[8] + a[4] * b[9] + a[8] * b[10] + a[12] * b[11],
              a[1] * b[8] + a[5] * b[9] + a[9] * b[10] + a[13] * b[11],
              a[2] * b[8] + a[6] * b[9] + a[10] * b[10] + a[14] * b[11],
              a[3] * b[8] + a[7] * b[9] + a[11] * b[10] + a[15] * b[11],

              a[0] * b[12] + a[4] * b[13] + a[8] * b[14] + a[12] * b[15],
              a[1] * b[12] + a[5] * b[13] + a[9] * b[14] + a[13] * b[15],
              a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14] * b[15],
              a[3] * b[12] + a[7] * b[13] + a[11] * b[14] + a[15] * b[15]);
}

std::ostream &operator<<(std::ostream &fh, mat4 const &m)
{
  for (int i = 0; i < 4; i++)
  {
    fh << "[ ";
    for (int j = 0; j < 4; j++)
      fh << m.m[j * 4 + i] << " ";
    fh << "]\n";
  }

  return fh;
}
