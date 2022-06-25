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
#ifndef _CST_LIB_SG_MESHUTIL_H
#define _CST_LIB_SG_MESHUTIL_H

#include "node.h"

namespace cst {

// Create a node with a single mesh with the given vertices, indices and
// textures.
node_ptr createMeshNode(std::string const &name,
                        std::vector<vertex> const &vertices,
                        std::vector<uint32_t> const &indices, material_ptr mat);

// Create a node with a single mesh with the given vertices, indices and
// textures.
node_ptr createMeshNode(std::string const &name,
                        std::vector<vertex> const &vertices,
                        std::vector<uint32_t> const &indices,
                        std::string const &textureRoot,
                        std::string const &albedo, std::string const &normal,
                        std::string const &roughness, std::string const &ao);

// Create a cube suitable for a skybox.
node_ptr createSkyBox(float size, material_ptr mat,
                      std::string const &name = "skybox");

// Create a cube suitable for a skybox. It is textured using a cube
// texture from textureDir.
node_ptr createSkyBox(float size, std::string const &textureDir,
                      std::string const &name = "skybox");

} // namespace cst

#endif // _CST_LIB_SG_MESHUTIL_H
