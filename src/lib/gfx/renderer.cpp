
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
#include "renderer.h"

using namespace cst;

static Renderer *instance = nullptr;
static std::mutex instance_mux;

Renderer *cst::getRenderer() {
  std::scoped_lock lock(instance_mux);
  if(instance == nullptr)
    throw new std::runtime_error("Renderer not initialized");
  return instance;
}

void cst::setRenderer(Renderer *renderer) {
  std::scoped_lock lock(instance_mux);
  if(instance != nullptr)
    throw new std::runtime_error("Renderer already initialized");
  instance = renderer;
}

node_ptr cst::stageAll(node_ptr root, renderer_ptr renderer) {
  Renderer *rend = renderer.get();

  root->mapChildren([rend](node_ptr child) {
    return rend->stage(child);
  });

  std::scoped_lock lock(root->mutex());
  return renderer->stage(root);
}

