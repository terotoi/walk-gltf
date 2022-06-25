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
#ifndef _CST_LIB_MATH_GEOMETRY_H
#define _CST_LIB_MATH_GEOMETRY_H

#include "math/vertex.h"

#include <memory>
#include <vector>

namespace cst {

/**
 * Fill vertices and indices with geometry data for a cube.
 * @param width width of the cube
 * @param height height of the cube
 * @param depth depth of the cube
 * @param texScale texture scale
 */
void createCube(float width, float height, float depth, float texScale,
                std::vector<vertex> &vertices, std::vector<uint32_t> &indices);

/**
 * Fill vertices and indices with geometry data for a sphere.
 * @param radius radius of the sphere
 * @param slices number of slices (longitude)
 * @param stacks number of stacks (latitude)
 * @param vertices output vector of vertices
 * @param indices output vector of indices
 */
void createSphere(float radius, int slices, int stacks,
                  std::vector<vertex> &vertices,
                  std::vector<uint32_t> &indices);

} // namespace cst

#endif // _CST_LIB_MATH_GEOMETRY_H
