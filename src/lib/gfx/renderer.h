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
#ifndef _CST_LIB_GFX_RENDERER_H
#define _CST_LIB_GFX_RENDERER_H

#include "math/ivec2.h"
#include "math/mat4.h"
#include "sg/node.h"

#include <SDL.h>

namespace cst {

/**
 * Renderer is a base class for all renderers.
 */
class Renderer {
public:
  Renderer() {}
  virtual ~Renderer() {}

  // Returns the current size of the viewport.
  virtual ivec2 const &getViewSize() const = 0;

  // Sets the clear color.
  virtual void setClearColor(vec4 const &color) = 0;
  virtual void setAmbientLight(vec4 const &color) = 0;
  virtual void setLight(uint32_t n, vec3 const &pos, vec3 const &color) = 0;

  virtual void setElapsed(float time) = 0;
  virtual void setView(mat4 const &projectMat, mat4 const &viewMat,
                       vec3 const &viewPos) = 0;

  // Returns the projection * view matrix.
  virtual mat4 const &getProjectionView() const = 0;

  // Stages a node for display. Unstaged nodes are not rendered.
  // NOTE: the node must be locked before calling this function.
  virtual node_ptr stage(node_ptr node) = 0;

  /** Called when the window was resized. */
  virtual void windowResized() = 0;

  /**
   * Renders a scene.
   *
   * @param renlist of list of mesh/transform pairs to render.
   */
  virtual void render(std::vector<node_ptr> const &nodes) = 0;

  /**
   * @brief Flushes the renderer's queue.
   */
  virtual void flush() = 0;

private:
};

typedef std::shared_ptr<Renderer> renderer_ptr;

// Returns the Renderer singleton.
Renderer *getRenderer();

// Sets the Renderer singleton.
void setRenderer(Renderer *renderer);

// Stage a node all its children.
node_ptr stageAll(node_ptr node, renderer_ptr renderer);

inline void setClearColor(vec4 const &clearColor) {
  getRenderer()->setClearColor(clearColor);
}

inline void setAmbientLight(vec4 const &color) {
  getRenderer()->setAmbientLight(color);
}

// Set projection and view matrices and view position for rendering.
inline void setView(mat4 const &proj, mat4 const &view, vec3 const &viewPos) {
  getRenderer()->setView(proj, view, viewPos);
}

} // namespace cst

#endif // _CST_LIB_GFX_RENDERER_H
