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
#include "geometry.h"
#include "mathutil.h"
#include <iostream>

using namespace cst;

void cst::createCube(float width, float height, float depth, float texScale,
                     std::vector<vertex> &vertices,
                     std::vector<uint32_t> &indices) {
  vertices = {//
              {{-width / 2.0f, height / 2.0f, depth / 2.0f},
               {0.0f, 0.0f, 1.0f},
               {0.0f, texScale}}, //
              {{-width / 2.0f, -height / 2.0f, depth / 2.0f},
               {0.0f, 0.0f, 1.0f},
               {0.0f, 0.0f}},
              {{width / 2.0f, -height / 2.0f, depth / 2.0f},
               {0.0f, 0.0f, 1.0f},
               {texScale, 0.f}}, //
              {{width / 2.0f, height / 2.0f, depth / 2.0f},
               {0.0f, 0.0f, 1.0f},
               {texScale, texScale}}, //

              {{width / 2.0f, height / 2.0f, depth / 2.0f},
               {1.0f, 0.0f, 0.0f},
               {0.0f, texScale}}, //
              {{width / 2.0f, -height / 2.0f, depth / 2.0f},
               {1.0f, 0.0f, 0.0f},
               {0.0f, 0.0f}},
              {{width / 2.0f, -height / 2.0f, -depth / 2.0f},
               {1.0f, 0.0f, 0.0f},
               {texScale, 0.f}}, //
              {{width / 2.0f, height / 2.0f, -depth / 2.0f},
               {1.0f, 0.0f, 0.0f},
               {texScale, texScale}}, //

              {{width / 2.0f, height / 2.0f, -depth / 2.0f},
               {0.0f, 0.0f, -1.0f},
               {0.0f, texScale}}, //
              {{width / 2.0f, -height / 2.0f, -depth / 2.0f},
               {0.0f, 0.0f, -1.0f},
               {0.0f, 0.0f}},
              {{-width / 2.0f, -height / 2.0f, -depth / 2.0f},
               {0.0f, 0.0f, -1.0f},
               {texScale, 0.f}}, //
              {{-width / 2.0f, height / 2.0f, -depth / 2.0f},
               {0.0f, 0.0f, -1.0f},
               {texScale, texScale}},

              {{-width / 2.0f, height / 2.0f, -depth / 2.0f},
               {-1.0f, 0.0f, 0.0f},
               {0.0f, texScale}}, //
              {{-width / 2.0f, -height / 2.0f, -depth / 2.0f},
               {-1.0f, 0.0f, 0.0f},
               {0.0f, 0.0f}},
              {{-width / 2.0f, -height / 2.0f, depth / 2.0f},
               {-1.0f, 0.0f, 0.0f},
               {texScale, 0.f}}, //
              {{-width / 2.0f, height / 2.0f, depth / 2.0f},
               {-1.0f, 0.0f, 0.0f},
               {texScale, texScale}},

              {{-width / 2.0f, height / 2.0f, -depth / 2.0f},
               {0.0f, 1.0f, 0.0f},
               {0.0f, texScale}}, //
              {{-width / 2.0f, height / 2.0f, depth / 2.0f},
               {0.0f, 1.0f, 0.0f},
               {0.0f, 0.0f}},
              {{width / 2.0f, height / 2.0f, depth / 2.0f},
               {0.0f, 1.0f, 0.0f},
               {texScale, 0.0f}}, //
              {{width / 2.0f, height / 2.0f, -depth / 2.0f},
               {0.0f, 1.0f, 0.0f},
               {texScale, texScale}}, //

              {{-width / 2.0f, -height / 2.0f, depth / 2.0f},
               {0.0f, -1.0f, 0.0f},
               {0.0f, texScale}}, //
              {{-width / 2.0f, -height / 2.0f, -depth / 2.0f},
               {0.0f, -1.0f, 0.0f},
               {0.0f, 0.0f}},
              {{width / 2.0f, -height / 2.0f, -depth / 2.0f},
               {0.0f, -1.0f, 0.0f},
               {texScale, 0.f}}, //
              {{width / 2.0f, -height / 2.0f, depth / 2.0f},
               {0.0f, -1.0f, 0.0f},
               {texScale, texScale}}};

  indices = {0,  1,  2,  2,  3,  0,  //
             4,  5,  6,  6,  7,  4,  //
             8,  9,  10, 10, 11, 8,  //
             12, 13, 14, 14, 15, 12, //
             16, 17, 18, 18, 19, 16, //
             20, 21, 22, 22, 23, 20};

  vertices[0].tangent = vec3(1.0f, 0.0f, 0.0f);
  vertices[1].tangent = vec3(1.0f, 0.0f, 0.0f);
  vertices[2].tangent = vec3(1.0f, 0.0f, 0.0f);
  vertices[3].tangent = vec3(1.0f, 0.0f, 0.0f);

  vertices[4].tangent = vec3(0.0f, 0.0f, 1.0f);
  vertices[5].tangent = vec3(0.0f, 0.0f, 1.0f);
  vertices[6].tangent = vec3(0.0f, 0.0f, 1.0f);
  vertices[7].tangent = vec3(0.0f, 0.0f, 1.0f);

  vertices[8].tangent = vec3(-1.0f, 0.0f, 0.0f);
  vertices[9].tangent = vec3(-1.0f, 0.0f, 0.0f);
  vertices[10].tangent = vec3(-1.0f, 0.0f, 0.0f);
  vertices[11].tangent = vec3(-1.0f, 0.0f, 0.0f);

  vertices[12].tangent = vec3(-1.0f, 0.0f, 0.0f);
  vertices[13].tangent = vec3(-1.0f, 0.0f, 0.0f);
  vertices[14].tangent = vec3(-1.0f, 0.0f, 0.0f);
  vertices[15].tangent = vec3(-1.0f, 0.0f, 0.0f);

  vertices[16].tangent = vec3(1.0f, 0.0f, 0.0f);
  vertices[17].tangent = vec3(1.0f, 0.0f, 0.0f);
  vertices[18].tangent = vec3(1.0f, 0.0f, 0.0f);
  vertices[19].tangent = vec3(1.0f, 0.0f, 0.0f);

  vertices[20].tangent = vec3(1.0f, 0.0f, 0.0f);
  vertices[21].tangent = vec3(1.0f, 0.0f, 0.0f);
  vertices[22].tangent = vec3(1.0f, 0.0f, 0.0f);
  vertices[23].tangent = vec3(1.0f, 0.0f, 0.0f);
}

void cst::createSphere(float radius, int slices, int stacks,
                       std::vector<vertex> &vertices,
                       std::vector<uint32_t> &indices) {

  vertices.clear();
  indices.clear();

  float phi = 0.0f;
  float theta = 0.0f;

  for (int i = 0; i < stacks + 1; i++) {
    phi = i * (M_PI / stacks);
    for (int j = 0; j < slices; j++) {
      theta = j * (2.0f * M_PI) / slices;

      float x = radius * sinf(phi) * cosf(theta);
      float y = radius * cosf(phi);
      float z = radius * sinf(phi) * sinf(theta);

      vertices.push_back({{radius * x, radius * y, radius * z},
                          {x, y, z},
                          {(float)j / slices, (float)i / stacks}});
      vertices[vertices.size() - 1].tangent =
          vec3(x, y, z) / sqrtf(x * x + y * y + z * z);
    }
  }

  for (int y = 0; y < stacks; y++) {
    for (int x = 0; x < slices; x++) {
      indices.push_back(y * slices + x);
      indices.push_back((y + 1) * slices + (x + 1) % slices);
      indices.push_back((y + 1) * slices + x);

      indices.push_back(y * slices + x);
      indices.push_back(y * slices + (x + 1) % slices);
      indices.push_back((y + 1) * slices + (x + 1) % slices);
    }
  }

  calcTangents(vertices, indices);
}
