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
#include "gltf.h"
#include "core/fileutil.h"
#include "gfx/shader_data.h"
#include "math/mathutil.h"
#include "math/quat.h"
#include "sg/empty.h"
#include "sg/light.h"

using namespace cst;

GLTFLoader::GLTFLoader(bool flatShading, bool deduplicateVertices,
                       bool doLoadTextures, bool doLoadLights)
    : flatShading(flatShading), deduplicateVertices(deduplicateVertices),
      doLoadTextures(doLoadTextures),
      doLoadLights(doLoadLights) {}

GLTFLoader::~GLTFLoader() {}

void GLTFLoader::loadIndices(tinygltf::Primitive const &prim,
                             std::vector<uint32_t> &indices) {
  // Load the indices
  assert(prim.indices >= 0);
  auto &acc = model.accessors[prim.indices];
  assert(acc.bufferView >= 0);

  tinygltf::BufferView const &view = model.bufferViews[acc.bufferView];
  assert(acc.type == TINYGLTF_TYPE_SCALAR);

  assert(view.buffer >= 0);
  tinygltf::Buffer const &buf = model.buffers[view.buffer];

  assert(acc.count % 3 == 0); // Triangles
  indices.resize(acc.count);
  if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
    assert(acc.ByteStride(view) == 2);
    uint16_t *d =
        (uint16_t *)(buf.data.data() + view.byteOffset + acc.byteOffset);
    for (size_t i = 0; i < acc.count; i++)
      indices[i] = d[i];
  } else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
    assert(acc.ByteStride(view) == 1);
    uint8_t *d =
        (uint8_t *)(buf.data.data() + view.byteOffset + acc.byteOffset);
    for (size_t i = 0; i < acc.count; i++)
      indices[i] = d[i];

  } else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
    assert(acc.ByteStride(view) == 4);
    uint32_t *d =
        (uint32_t *)(buf.data.data() + view.byteOffset + acc.byteOffset);
    for (size_t i = 0; i < acc.count; i++) {
      indices[i] = d[i];
    }
  } else {
    std::cerr << "component: " << acc.componentType << "\n";
    throw std::runtime_error("unsupported index componet type");
  }
}

void GLTFLoader::loadVertices(tinygltf::Primitive const &prim,
                              std::vector<vertex> &vertices,
                              bool &tangentsLoaded) {
  bool normalsLoaded = false;

  for (auto const &attr : prim.attributes) {
    if (attr.first == "POSITION" || attr.first == "NORMAL" ||
        attr.first == "TEXCOORD_0" || attr.first == "TANGENT") {
      tinygltf::Accessor const &acc = model.accessors[attr.second];
      tinygltf::BufferView const &view = model.bufferViews[acc.bufferView];

      assert(acc.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

      if (!(acc.type == TINYGLTF_TYPE_SCALAR ||
            acc.type == TINYGLTF_TYPE_VEC3 || acc.type == TINYGLTF_TYPE_VEC2 ||
            acc.type == TINYGLTF_TYPE_VEC4))
        throw std::runtime_error("unsupported accessor type");

      tinygltf::Buffer const &buf = model.buffers[view.buffer];

      if (attr.first == "POSITION") {
        assert(acc.ByteStride(view) == 12);
        assert(acc.type == TINYGLTF_TYPE_VEC3);
        assert(vertices.size() == 0 || vertices.size() == acc.count);

        if (acc.count > vertices.size())
          vertices.resize(acc.count);
        float *d =
            (float *)(buf.data.data() + view.byteOffset + acc.byteOffset);
        for (size_t i = 0; i < acc.count; i++) {
          vertices[i].pos = vec3(d[i * 3], d[i * 3 + 1], d[i * 3 + 2]);
        }
      } else if (attr.first == "NORMAL") {
        assert(acc.ByteStride(view) == 12);
        assert(acc.type == TINYGLTF_TYPE_VEC3);
        assert(vertices.size() == 0 || vertices.size() == acc.count);

        if (acc.count > vertices.size())
          vertices.resize(acc.count);
        float *d =
            (float *)(buf.data.data() + view.byteOffset + acc.byteOffset);
        for (size_t i = 0; i < acc.count; i++) {
          vertices[i].normal = vec3(d[i * 3], d[i * 3 + 1], d[i * 3 + 2]);
        }
        normalsLoaded = true;
      } else if (attr.first == "TEXCOORD_0") {
        assert(acc.ByteStride(view) == 8);
        assert(acc.type == TINYGLTF_TYPE_VEC2);
        assert(vertices.size() == 0 || vertices.size() == acc.count);

        if (acc.count > vertices.size())
          vertices.resize(acc.count);
        float *d =
            (float *)(buf.data.data() + view.byteOffset + acc.byteOffset);
        for (size_t i = 0; i < acc.count; i++) {
          vertices[i].texcoord = vec2(d[i * 2], d[i * 2 + 1]);
        }
      } else if (attr.first == "TANGENT") {
        assert(acc.ByteStride(view) == 16);
        assert(acc.type == TINYGLTF_TYPE_VEC4);
        assert(vertices.size() == 0 || vertices.size() == acc.count);

        if (acc.count > vertices.size())
          vertices.resize(acc.count);
        float *d =
            (float *)(buf.data.data() + view.byteOffset + acc.byteOffset);
        for (size_t i = 0; i < acc.count; i++) {
          vertices[i].tangent = vec3(d[i * 4], d[i * 4 + 1], d[i * 4 + 2]);
        }
        tangentsLoaded = true;
      }
    }
  }

  if (!normalsLoaded)
    throw std::runtime_error("no normals given in the model");
}

material_ptr GLTFLoader::loadMaterial(tinygltf::Material const &tm) {
  vec4 albedo{0.8f, 0.8f, 0.8f, 1.0f};
  float metallic = 0.0f;
  float roughness = 0.5f;
  float ao = 1.0f;

  std::string name = tm.name;

  if (tm.pbrMetallicRoughness.baseColorFactor.size() > 0) {
    albedo = vec4(tm.pbrMetallicRoughness.baseColorFactor);
    metallic = tm.pbrMetallicRoughness.metallicFactor;
    roughness = tm.pbrMetallicRoughness.roughnessFactor;
  }

  // Refactor these
  std::string albedoTexName;
  if (tm.pbrMetallicRoughness.baseColorTexture.index >= 0) {
    assert((int)model.textures.size() >
           tm.pbrMetallicRoughness.baseColorTexture.index);

    tinygltf::Texture const &mtex =
        model.textures[tm.pbrMetallicRoughness.baseColorTexture.index];

    assert(mtex.source >= 0 && (int)model.images.size() > mtex.source);
    tinygltf::Image const &mimage = model.images[mtex.source];

    albedoTexName = dirPath + "/" + mimage.uri;
  }

  std::string roughnessTexName;
  if (tm.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0) {
    assert((int)model.textures.size() >
           tm.pbrMetallicRoughness.metallicRoughnessTexture.index);

    tinygltf::Texture const &mtex =
        model.textures[tm.pbrMetallicRoughness.metallicRoughnessTexture.index];

    assert(mtex.source >= 0 && (int)model.images.size() > mtex.source);
    tinygltf::Image const &mimage = model.images[mtex.source];

    roughnessTexName = dirPath + "/" + mimage.uri;
  }

  std::string normalTexName;
  if (tm.normalTexture.index >= 0) {
    assert((int)model.textures.size() > tm.normalTexture.index);

    tinygltf::Texture const &mtex = model.textures[tm.normalTexture.index];

    assert(mtex.source >= 0 && (int)model.images.size() > mtex.source);
    tinygltf::Image const &mimage = model.images[mtex.source];

    normalTexName = dirPath + "/" + mimage.uri;
  }

  // tm.doubleSided
  material_ptr mat = std::make_shared<MaterialStd>(
      flatShading ? SHADE_MODE_FLAT : SHADE_MODE_SMOOTH, albedo, metallic,
      roughness, ao, false);
  if (albedoTexName != "" && doLoadTextures) {
    texture_ptr albedoTex = Texture::getNamed(albedoTexName);
    if (albedoTex == nullptr) {
      albedoTex =
          std::make_shared<TextureStd>(albedoTexName, TEXTURE_TYPE_ALBEDO);
      textures[albedoTexName] = albedoTex;
    }
    albedoTex->load();

    mat->setAlbedoTex(albedoTex);
  }

  if (roughnessTexName != "" && doLoadTextures) {
    texture_ptr roughnessTex = Texture::getNamed(roughnessTexName);
    if (roughnessTex == nullptr) {
      roughnessTex = std::make_shared<TextureStd>(roughnessTexName,
                                                  TEXTURE_TYPE_ROUGHNESS);
      textures[roughnessTexName] = roughnessTex;
    }
    roughnessTex->load();
    mat->setRoughnessTex(roughnessTex,
                         roughnessTexName.find("_arm_") != std::string::npos);
  }

  if (normalTexName != "" && doLoadTextures) {
    texture_ptr normalTex = Texture::getNamed(normalTexName);
    if (normalTex == nullptr) {
      normalTex =
          std::make_shared<TextureStd>(normalTexName, TEXTURE_TYPE_NORMAL);
      textures[normalTexName] = normalTex;
    }
    normalTex->load();
    mat->setNormalTex(normalTex);
  }
  return mat;
}

std::vector<mesh_ptr> GLTFLoader::loadMesh(tinygltf::Mesh const &mesh) {
  std::vector<mesh_ptr> meshes;

  /// Primitives are loaded as meshes.
  for (auto &prim : mesh.primitives) {
    if (prim.mode != TINYGLTF_MODE_TRIANGLES) {
      std::cerr << "GTLF: WARNING: non-triangle mesh found\n";
      continue;
    }

    std::vector<uint32_t> indices;
    loadIndices(prim, indices);

    std::vector<vertex> vertices;
    bool tangentsLoaded = false;
    loadVertices(prim, vertices, tangentsLoaded);

    if (!tangentsLoaded)
      calcTangents(vertices, indices);

    if (deduplicateVertices)
      deduplicate(vertices, indices);

    material_ptr mat;

    int mat_idx = (prim.material >= 0) ? prim.material : -1;
    if (modelMaterials.count(mat_idx) == 1) {
      // std::cout << "    Fetching material " << mat_idx << " from cache.\n";
      mat = modelMaterials[mat_idx];
    } else {
      if (mat_idx == -1)
        mat = std::make_shared<MaterialStd>(
            flatShading ? SHADE_MODE_FLAT : SHADE_MODE_SMOOTH,
            vec4{0.8f, 0.8f, 0.8f, 1.0f}, 0.0f, 0.5, 1.0f, false);
      else
        mat = loadMaterial(model.materials[mat_idx]);

      modelMaterials[mat_idx] = mat;
    }

    mesh_ptr const &m = std::make_shared<MeshStd>(vertices, indices, mat);
    meshes.push_back(m);
  }
  return meshes;
}

void GLTFLoader::loadNode(tinygltf::Node const &m_node, int depth,
                          node_ptr root) {
  mat4 tr;
  if (m_node.matrix.size() > 0) {
    tr = mat4(m_node.matrix);
  } else {
    mat4 t, r, s;
    if (m_node.translation.size() > 0) {
      assert(m_node.translation.size() == 3);
      t = mat4::translate(m_node.translation);
    }

    if (m_node.rotation.size() > 0) {
      r = mat4(quat(m_node.rotation));
    }

    if (m_node.scale.size() > 0) {
      s = mat4::scale(vec3(m_node.scale));
    }

    tr = t * r * s;
  }

  std::vector<mesh_ptr> prims;
  if (m_node.mesh >= 0) {
    int mesh_idx = m_node.mesh;
    // Passing node_tr as the transform flattens the meshes.
    // All primitives are loaded as separate meshes.

    if (modelMeshes.count(mesh_idx) == 1) {
      prims = modelMeshes[mesh_idx];
    } else {
      prims = loadMesh(model.meshes[mesh_idx]);
      modelMeshes[mesh_idx] = prims;
    }
  }

  // Light info
  for (auto ext : m_node.extensions) {
    std::cout << m_node.name << " extension: " << ext.first << " "
              << ext.second.Size() << "\n";

    if (ext.first == "KHR_lights_punctual") {
      int num = ext.second.Get("light").GetNumberAsInt();
      assert(num >= 0 && num < (int)model.lights.size());
      assert(num < MAX_LIGHTS);

      tinygltf::Light &m_light = model.lights[num];
      light_ptr light = std::make_shared<Light>(
          num, vec3(m_light.color) * m_light.intensity, m_light.name);
      root->addChild(light);
      return;
    }
  }

  node_ptr node;
  if (prims.size() == 0)
    node = std::make_shared<Empty>(m_node.name);
  else
    node = std::make_shared<Node>(prims, m_node.name);

  node->setLocalTransform(tr);
  root->addChild(node);

  if (m_node.children.size() > 0) {
    for (auto ch : m_node.children) {
      loadNode(model.nodes[ch], depth + 1, node);
    }
  }
}

void GLTFLoader::loadScene(node_ptr root) {
  tinygltf::Scene const &scene = model.scenes[model.defaultScene];

  for (auto idx : scene.nodes) {
    loadNode(model.nodes[idx], 0, root);
  }
}

node_ptr GLTFLoader::load(std::string const &filename) {
  tinygltf::TinyGLTF tiny;
  std::string err;
  std::string warn;

  std::string const gltf_ext = ".gltf";
  dirPath = dirPart(filename);

  std::cout << "Loading " << filename << std::endl;

  if (filename.size() > gltf_ext.size() &&
      std::equal(gltf_ext.rbegin(), gltf_ext.rend(), filename.rbegin())) {
    if (!tiny.LoadASCIIFromFile(&model, &err, &warn, filename))
      throw std::runtime_error("failed to load GLTF model " + filename);
  } else {
    if (!tiny.LoadBinaryFromFile(&model, &err, &warn, filename))
      throw std::runtime_error("failed to load GLTF model " + filename);
  }

  node_ptr root = std::make_shared<Empty>(filename);
  loadScene(root);
  modelMeshes.clear();
  modelMaterials.clear();
  return root;
}