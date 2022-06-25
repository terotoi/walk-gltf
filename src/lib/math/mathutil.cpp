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
#include "mathutil.h"
#include <iostream>
#include <unordered_map>

using namespace cst;

void cst::deduplicate(std::vector<vertex> &vertices,
                      std::vector<uint32_t> &indices) {
  std::unordered_map<vertex, uint32_t> vm;
  std::vector<vertex> out_vertices;
  std::vector<uint32_t> out_indices;

  for (auto i : indices) {
    const vertex &v = vertices[i];

    if (vm.count(v) == 0) {
      uint32_t idx = out_vertices.size();
      vm[v] = idx;
      out_vertices.push_back(v);
      out_indices.push_back(idx);
    } else {
      out_indices.push_back(vm[v]);
    }
  }

  vertices = out_vertices;
  indices = out_indices;
}

void cst::calcTangents(std::vector<vertex> &vertices,
                       std::vector<uint32_t> &indices) {

  assert(indices.size() % 3 == 0);

  for (uint32_t i = 0; i < indices.size(); i += 3) {
    uint32_t i1 = indices[i];
    uint32_t i2 = indices[i + 1];
    uint32_t i3 = indices[i + 2];

    vertex &v1 = vertices[i1];
    vertex &v2 = vertices[i2];
    vertex &v3 = vertices[i3];

    vec3 const &pos1 = v1.pos;
    vec3 const &pos2 = v2.pos;
    vec3 const &pos3 = v3.pos;

    vec2 const &uv1 = v1.texcoord;
    vec2 const &uv2 = v2.texcoord;
    vec2 const &uv3 = v3.texcoord;

    vec3 const edge1 = pos2 - pos1;
    vec3 const edge2 = pos3 - pos1;

    vec2 const duv1 = uv2 - uv1;
    vec2 const duv2 = uv3 - uv1;

    float f = 1.0f / (duv1.x() * duv2.y() - duv1.y() - duv2.x());

    v1.tangent = (edge1 * uv2.y() - edge2 * uv1.y()) * f;
    v2.tangent = v1.tangent;
    v3.tangent = v1.tangent;
  }
}

void cst::flipNormals(std::vector<vertex> &vertices) {
  for (auto &v : vertices) {
    v.normal.d[0] *= -1;
    v.normal.d[1] *= -1;
    v.normal.d[2] *= -1;
  }
}
