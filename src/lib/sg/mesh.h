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
#ifndef _CST_LIB_SG_MESH_H
#define _CST_LIB_SG_MESH_H

#include "core/lockable.h"
#include "material.h"
#include "math/aabb.h"
#include "math/vertex.h"

#include <memory>
#include <vector>

namespace cst {

class Mesh;
typedef std::shared_ptr<Mesh> mesh_ptr;

/**
 *  Mesh
 */
class Mesh : public Lockable {
public:
  Mesh(std::vector<vertex> const &vertices, material_ptr material)
      : material(material), aabb(vertices) {}

  // Mesh(material_ptr material) : material(material) {}
  Mesh(mesh_ptr mesh) : material(mesh->material), aabb(mesh->aabb) {}
  virtual ~Mesh() {}

  AABB const &getAABB() const { return aabb; }

  // Returns true if this mesh is staged (i.e. is ready to be displayed)
  virtual bool isStaged() const = 0;

  virtual std::vector<vertex> const &getVertices() const = 0;
  virtual std::vector<uint32_t> const &getIndices() const = 0;

  material_ptr getMaterial() const { return material; }
  void setMaterial(material_ptr mat) { material = mat; }

private:
  material_ptr material;
  AABB aabb;
};

/**
 *  MeshStd
 */
class MeshStd : public Mesh {
public:
  MeshStd(std::vector<vertex> const &vertices,
          std::vector<uint32_t> const &indices, material_ptr material)
      : Mesh(vertices, material), vertices(vertices), indices(indices) {}

  ~MeshStd() {}

  bool isStaged() const override { return false; }

  std::vector<vertex> const &getVertices() const override { return vertices; }
  std::vector<uint32_t> const &getIndices() const override { return indices; }

private:
  std::vector<vertex> const vertices;
  std::vector<uint32_t> const indices;
};

} // namespace cst

#endif // _CST_LIB_SG_MESH_H
