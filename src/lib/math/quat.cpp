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
#include "quat.h"

using namespace cst;

quat::operator mat4() const {
  float const w = k[0];
  float const x = k[1];
  float const y = k[2];
  float const z = k[3];

  return mat4(                    //
      1.0f - 2.0f * (y * y + z * z), //
      2.0f * (x * y + w * z),     //
      2.0f * (x * z - w * y),     //
      0.0f,                       //

      2.0f * (x * y - w * z),        //
      1.0f - 2.0f * (x * x + z * z), //
      2.0f * (y * z + w * x),        //
      0.0f,                          //

      2.0f * (x * z + w * y),        //
      2.0f * (y * z - w * x),        //
      1.0f - 2.0f * (x * x + y * y), //
      0.0f,                          //
      0.0f, 0.0f, 0.0f, 1.0f);
};
