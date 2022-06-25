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
#ifndef _CST_LIB_MATH_IVEC2_H
#define _CST_LIB_MATH_IVEC2_H

#include <ostream>
#include <stdint.h>

namespace cst
{
  /**
   * ivec2
   */
  struct ivec2
  {
    int32_t v[2];

    // Creates a vector with x, y = 0.
    ivec2()
    {
      v[0] = 0;
      v[1] = 0;
    }

    ivec2(int32_t x, int32_t y)
    {
      v[0] = x;
      v[1] = y;
    }

    int32_t operator[](size_t i) const { return v[i]; }
    int32_t &operator[](size_t i) { return v[i]; }

    int32_t x() const { return v[0]; }
    int32_t y() const { return v[1]; }

    int32_t width() const { return v[0]; }
    int32_t height() const { return v[1]; }

    bool operator==(ivec2 const &b) const
    {
      return v[0] == b.v[0] && v[1] == b.v[1];
    }

    bool operator!=(ivec2 const &b) const
    {
      return v[0] != b.v[0] || v[1] != b.v[1];
    }
  };

  std::ostream &operator<<(std::ostream &os, ivec2 const &v);

} // namespace cst

#endif // _CST_LIB_MATH_IVEC2_H