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
#include "node.h"

#include <iostream>

using namespace cst::vlk;
using namespace cst;

NodeVlk::NodeVlk(node_ptr node, descpool_ptr descPool, int numImages)
    : Node(node) {
  sets = allocSets(numImages, descPool);
  createBuffers(descPool->getDevice(), numImages);

  for (int i = 0; i < numImages; i++)
    copyTransformToBuffer(i);
}

NodeVlk::~NodeVlk() {}

void NodeVlk::createBuffers(device_ptr dev, size_t numImages) {
  bufs.resize(numImages);
  for (size_t i = 0; i < numImages; i++) {
    bufs[i] = createUniformBuffer(dev, sizeof(NodeData), true);
    sets[i]->bind(BIND_NODE_UBO, bufs[i]);
  }
}

void NodeVlk::updateGlobalTransform(mat4 const &p) {
  Node::updateGlobalTransform(p);

  data.transform = getGlobalTransform();
}

void NodeVlk::copyTransformToBuffer(int imageIdx) {
  assert(imageIdx < (int)bufs.size());
  bufs[imageIdx]->copyDirectFrom(&data, sizeof(NodeData));
}

void NodeVlk::buildCommands(CommandBuffer *cmd, int imageIdx,
                            VkPipelineLayout pipeLayout, mat4 const &viewProj,
                            Material **currentMat,
                            VkPipeline *currentPipeline) const {
  assert(imageIdx < (int)bufs.size());

  cmd->bindDescriptorSet(DESC_SET_NODE, sets[imageIdx], pipeLayout);

  const NodeData *nd = &data;

  forMeshes(
      [viewProj, nd, cmd, pipeLayout, currentMat, currentPipeline](mesh_ptr m) {
        MeshVlk *mesh = dynamic_cast<MeshVlk *>(m.get());
        assert(mesh != nullptr);
        std::scoped_lock lock(mesh->mutex());

        mesh->buildCommands(cmd, pipeLayout, currentMat, currentPipeline);
      });
}
