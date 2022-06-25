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
#include "nodeutil.h"
#include "cubetexture.h"
#include "math/geometry.h"
#include "math/mathutil.h"

using namespace cst;

node_ptr cst::createMeshNode(std::string const &name,
                             std::vector<vertex> const &vertices,
                             std::vector<uint32_t> const &indices,
                             material_ptr mat) {
  mesh_ptr mesh = std::make_shared<MeshStd>(vertices, indices, mat);
  return std::make_shared<Node>(mesh, name);
}

node_ptr cst::createMeshNode(
    std::string const &name, std::vector<vertex> const &vertices,
    std::vector<uint32_t> const &indices, std::string const &textureRoot,
    std::string const &albedo, std::string const &normal,
    std::string const &roughness, std::string const &ao) {

  material_ptr mat = std::make_shared<MaterialStd>(
      SHADE_MODE_SMOOTH, vec4(1.0f), 0.1f, 0.9f, 1.0f, false);

  {
    texture_ptr tex = Texture::getNamed(textureRoot + "/" + albedo);
    if (tex == nullptr) {
      tex = std::make_shared<TextureStd>(textureRoot + "/" + albedo,
                                         TEXTURE_TYPE_ALBEDO);
      Texture::storeNamed(tex);
    }
    tex->load();
    mat->setAlbedoTex(tex);
  }

  if (normal != "") {
    texture_ptr tex = Texture::getNamed(textureRoot + "/" + normal);

    if (tex == nullptr) {
      tex = std::make_shared<TextureStd>(textureRoot + "/" + normal,
                                         TEXTURE_TYPE_NORMAL);
      Texture::storeNamed(tex);
    }
    tex->load();
    mat->setNormalTex(tex);
  }

  return createMeshNode(name, vertices, indices, mat);
}

node_ptr cst::createSkyBox(float size, material_ptr mat,
                           std::string const &name) {
  std::vector<vertex> vertices;
  std::vector<uint32_t> indices;
  createCube(size, size, size, 1.0f, vertices, indices);
  flipNormals(vertices);

  mesh_ptr mesh = std::make_shared<MeshStd>(vertices, indices, mat);
  return std::make_shared<Node>(mesh, name);
}

node_ptr cst::createSkyBox(float size, std::string const &textureDir,
                           std::string const &name) {
  texture_ptr tex = std::make_shared<CubeTexture>(
      TEXTURE_TYPE_ALBEDO, //
      textureDir + "/px.jpg", textureDir + "/nx.jpg", textureDir + "/py.jpg",
      textureDir + "/ny.jpg", textureDir + "/pz.jpg", textureDir + "/nz.jpg",
      "skybox");
  tex->load();

  material_ptr mat = std::make_shared<MaterialStd>(
      SHADE_MODE_UNSHADED, vec4(1.0f), 0.1f, 0.9f, 1.0f, true);
  mat->setAlbedoTex(tex);
  return createSkyBox(size, mat, name);
}
