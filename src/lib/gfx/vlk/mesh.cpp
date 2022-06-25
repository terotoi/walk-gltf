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
#include "mesh.h"

#include <iostream>

using namespace cst::vlk;
using namespace cst;

MeshVlk::MeshVlk(mesh_ptr mesh, device_ptr dev, cmdpool_ptr cmdPool,
                 descpool_ptr materialPool, queue_ptr queue)
    : Mesh(mesh) {
  std::vector<vertex> const &vertices = mesh->getVertices();
  std::vector<uint32_t> const &indices = mesh->getIndices();
  numIndices = indices.size();

  vertexBuf =
      createBufferFrom(dev, cmdPool, queue, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                       vertices.data(), sizeof(vertices[0]) * vertices.size());

  indexBuf =
      createBufferFrom(dev, cmdPool, queue, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                       indices.data(), sizeof(indices[0]) * indices.size());
}

MeshVlk::~MeshVlk() {
}

std::vector<vertex> const &MeshVlk::getVertices() const {
  throw std::runtime_error("MeshVlk does not have the vertices anymore");
}

std::vector<uint32_t> const &MeshVlk::getIndices() const {
  throw std::runtime_error("MeshVlk does not have the indices anymore");
}

int numMaterialSwaps = 0;

void MeshVlk::buildCommands(CommandBuffer *cmd, VkPipelineLayout pipeLayout,
                            Material **currentMat,
                            VkPipeline *currentPipeline) const {

  MaterialVlk *mat = dynamic_cast<MaterialVlk *>(getMaterial().get());
  assert(mat != nullptr);

  if (mat != *currentMat) {
    numMaterialSwaps++;
    *currentMat = mat;

    std::scoped_lock lock(mat->mutex());
    mat->buildCommands(cmd, pipeLayout, currentPipeline);
  }

  cmd->drawIndexed(vertexBuf, indexBuf, numIndices);
}
