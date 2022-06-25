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
#ifndef _CST_LIB_GFX_VLK_NODE_H
#define _CST_LIB_GFX_VLK_NODE_H

#include "mesh.h"
#include "sg/node.h"

namespace cst::vlk {

/**
 * NodeVlk
 */
class NodeVlk : public Node {
public:
  NodeVlk(node_ptr node, descpool_ptr descPool, int numImages);
  ~NodeVlk();

  bool isStaged() const override { return true; }

  void updateGlobalTransform(mat4 const &p) override;

  // Copies global transform to the UBO of imageIdx.
  void copyTransformToBuffer(int imageIdx);

  void buildCommands(CommandBuffer *cmd, int imageIdx,
                     VkPipelineLayout pipeLayout, mat4 const &viewProj,
                     Material **currentMat, VkPipeline *currentPipeline) const;

private:
  void createBuffers(device_ptr dev, size_t numImages);

  NodeData data{};
  std::vector<descset_ptr> sets;
  std::vector<buffer_ptr> bufs;
};

} // namespace cst::vlk

#endif // _CST_LIB_GFX_VLK_NODE_H
