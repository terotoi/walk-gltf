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
#ifndef _CST_LIB_APP_APP_H
#define _CST_LIB_APP_APP_H

#include "core/dispatcher.h"
#include "gfx/renderer.h"
#include "gfx/shader_data.h"
#include "input/events.h"
#include "sg/node.h"
#include "sg/scene.h"

namespace cst {

class AppBase {
public:
  AppBase(int reqWidth, int reqHeight);
  ~AppBase();

  // Calls update(Renderer *) for root and all its child nodes recursively.
  // After this it calls root->updateGlobalTransform(mat4());
  void updateAll(node_ptr root);

  // Stage root and all nodes under it. Returns the new staged root.
  // Also collects all visual nodes into visuals variable.
  node_ptr stageAllAndCollect(node_ptr root);

  void main();
  void run();

  virtual void update(float elapsed, float delta) {
    renderer->setElapsed(elapsed);
  }

  void quit() { running = false; }

  // Application settings;
  static bool fullscreen;
  static bool borderless;
  static bool grabMouse;
  static bool doPrintFPS;
private:
  void setupInput();

  // Sort a list of rendederable nodes.
  // Currently skyboxes are moved to the back of the list.
  void sortNodes(std::vector<node_ptr> &visuals);

  void cull(std::vector<node_ptr> const &src, std::vector<node_ptr> &out);

  virtual void keyDown(SDL_Keycode key) = 0;
  virtual void keyUp(SDL_Keycode key) = 0;
  virtual void mouseMove(int mx, int my) = 0;

  renderer_ptr renderer;
  std::unique_ptr<EventsProcessor> events;

  bool running = true;
  int frames = 0;

  std::mutex visuals_mux;
  std::vector<node_ptr> visuals;
};

} // namespace cst

#endif // _CST_LIB_APP_APP_H
