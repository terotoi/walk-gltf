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
#ifndef _CST_LIB_LOADER_GLTF_H_
#define _CST_LIB_LOADER_GLTF_H_

#include <tiny_gltf.h>
#include "sg/node.h"

namespace cst {

/**
 * GLTF model loader
 */
class GLTFLoader {
public:
  GLTFLoader(bool flatShading, bool deduplicateVertices, bool doLoadTextures, bool doLoadLights);
  ~GLTFLoader();

  /**
   * Loads a gltf model from a file
   * @param filename
   * @return scene graph
   */
  node_ptr load(std::string const &filename);

private:
  void loadIndices(tinygltf::Primitive const &prim,
                   std::vector<uint32_t> &indices);
  void loadVertices(tinygltf::Primitive const &prim,
                    std::vector<vertex> &vertices, bool &tangentsLoaded);
  material_ptr loadMaterial(tinygltf::Material const &tm);
  std::vector<mesh_ptr> loadMesh(tinygltf::Mesh const &mesh);
  void loadNode(tinygltf::Node const &m_node, int depth, node_ptr root);
  void loadScene(node_ptr root);

  bool flatShading;
  bool deduplicateVertices;
  bool doLoadTextures;
  bool doLoadLights;
  tinygltf::Model model;
  std::string dirPath;
  std::map<int, material_ptr> modelMaterials;
  std::map<int, std::vector<mesh_ptr>> modelMeshes;
  std::map<std::string, texture_ptr> textures;
};

} // namespace cst

#endif // _CST_LIB_LOADER_GLTF_H_
