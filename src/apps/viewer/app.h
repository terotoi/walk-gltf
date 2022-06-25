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
#ifndef _CST_APP_VIEWER_APP_H
#define _CST_APP_VIEWER_APP_H

#include "app/appbase.h"
#include "sg/camera.h"

namespace cst::app {

class ViewerApp : public AppBase {
public:
  ViewerApp(int reqWidth, int reqHeight, std::string const &programPath);
  ~ViewerApp();

  void setScene(scene_ptr scene);

  void loadSkybox(std::string const &path);

  void loadModel(std::string const &filename, std::function<void()> cb,
                 bool flatShading, bool deduplicateVertices);

  // Add lights if not already present
  void addLights();

  void main();

  // Public settings
  static bool doLoadTextures; // Load and use textures
private:
  void update(float elapsed, float delta);
  void paint();

  void keyDown(SDL_Keycode key);
  void keyUp(SDL_Keycode key);
  void mouseMove(int mx, int my);

  std::string programPath;
  std::unique_ptr<EventsProcessor> events;

  scene_ptr scene;
  node_ptr root;
  node_ptr model_root;
  camera_ptr camera;
  node_ptr skyBox;

  float rootScale = 1.0f;
  float pitch = 0.0f;
  float yaw = 0.0f;
  float speed = 10.0f;
  bool walkMode = false;
  bool fastMode = false;

  node_ptr lights[MAX_LIGHTS];
  vec3 vel;
};

} // namespace cst::app

#endif // _CST_APP_VIEWER_APP_H
