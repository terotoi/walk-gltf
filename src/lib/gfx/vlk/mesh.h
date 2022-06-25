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
#ifndef _CST_LIB_GFX_VLK_MESH_H
#define _CST_LIB_GFX_VLK_MESH_H

#include "sg/mesh.h"
#include "impl/buffer.h"
#include "impl/commands.h"
#include "impl/queue.h"
#include "material.h"
#include "math/vertex.h"

#include <cassert>
#include <vulkan/vulkan.h>

namespace cst::vlk {

/**
 * MeshVlk
 */
class MeshVlk : public Mesh {
public:
  MeshVlk(mesh_ptr mesh, device_ptr dev, cmdpool_ptr cmdPool,
          descpool_ptr materialPool, queue_ptr queue);
  ~MeshVlk();

  bool isStaged() const override { return true; }

  void buildCommands(CommandBuffer *cmd, VkPipelineLayout pipeLayout,
                     Material **currentMat, VkPipeline *currentPipeline) const;

  std::vector<vertex> const &getVertices() const override;
  std::vector<uint32_t> const &getIndices() const override;

private:
  unsigned int numIndices;
  buffer_ptr vertexBuf;
  buffer_ptr indexBuf;
};

} // namespace cst::vlk

#endif // _CST_LIB_GFX_VLK_MESH_H
