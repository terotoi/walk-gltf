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
#include "app.h"
#include "core/dispatcher_instance.h"
#include "gfx/vlk/impl/pipeline.h"
#include "loader/gltf.h"
#include "math/geometry.h"
#include "sg/empty.h"
#include "sg/light.h"
#include "sg/nodeutil.h"

#include <filesystem>
#include <functional>
#include <iostream>

#ifndef SHADERS_PATH
#define SHADERS_PATH "SHADERS_PATH not defined"
#endif

using namespace cst;
using namespace cst::vlk;
using namespace cst::app;

using namespace std::literals::chrono_literals;
using std::filesystem::is_directory;

static const vec4 DEFAULT_CLEAR_COLOR = vec4(0.28f, 0.38f, 0.48f, 1.0f);

bool ViewerApp::doLoadTextures = true;

ViewerApp::ViewerApp(int reqWidth, int reqHeight,
                     std::string const &programPath)
    : AppBase(reqWidth, reqHeight), programPath(programPath) {

  if (std::filesystem::is_directory(programPath + "/shaders"))
    setShaderPath(programPath + "/shaders");
  else if (std::filesystem::is_directory(SHADERS_PATH))
    setShaderPath(SHADERS_PATH);

  setClearColor(DEFAULT_CLEAR_COLOR);
  setAmbientLight(vec4(0.03f));

  setScene(std::make_shared<EmptyScene>());
}

ViewerApp::~ViewerApp() {}

void ViewerApp::setScene(scene_ptr scene) {
  if (this->scene != nullptr)
    throw std::runtime_error("ViewerApp::setScene: scene is not nullptr");
  this->scene = scene;
  camera = std::make_shared<Camera>();
  camera->setLocalPosition(vec3{0.0f, 1.7f, 5.0f});

  root = scene->getRoot();
  root->addChild(camera);

  model_root = std::make_shared<Empty>();
  root->addChild(model_root);
}

void ViewerApp::loadSkybox(std::string const &path) {
  if (doLoadTextures)
    skyBox = createSkyBox(40.0f, path, "skybox");
  else
    skyBox = createSkyBox(40.0f,
                          std::make_shared<MaterialStd>(
                              SHADE_MODE_UNSHADED, vec4(0.1f, 0.3f, 0.9f, 1.0f),
                              0.01f, 0.01f, 1.0f, true),
                          "skybox");
  skyBox->setLocalPosition(vec3{0.0f, 0.0f, 0.0f});
  root->addChild(skyBox);
}

void ViewerApp::loadModel(std::string const &modelName,
                          std::function<void()> cb, bool flatShading,
                          bool deduplicateVertices) {

  getDispatcher()->add([this, modelName, cb, flatShading,
                        deduplicateVertices]() {
    GLTFLoader loader(flatShading, deduplicateVertices, doLoadTextures, true);
    auto model = loader.load(modelName);

    {
      std::scoped_lock lock(model_root->mutex());
      model_root->addChild(model);
    }
    cb();

    root = stageAllAndCollect(root);
    skyBox = root->find("skybox");
  });
}

void ViewerApp::addLights() {
  // Check if lights are already added
  std::scoped_lock lock(root->mutex());
  int numLights = 0;
  root->forEach(
      [&numLights, this](node_ptr node) {
        if (std::dynamic_pointer_cast<Light>(node) != nullptr) {
          if (numLights < MAX_LIGHTS)
            lights[numLights] = std::dynamic_pointer_cast<Light>(node);
          numLights++;
        }
      },
      true);

  if (numLights < MAX_LIGHTS) {
    std::cout << "Number of lights present: " << numLights << ", adding "
              << (MAX_LIGHTS - numLights) << std::endl;

    for (int i = numLights; i < MAX_LIGHTS; i++) {
      auto light = std::make_shared<Light>(i, vec3(100.0f, 100.0f, 100.0f));
      light->setLocalPosition(vec3(-20.0f + i * 5.0f, 3.0f, -20.0f + i * 5.0f));
      root->addChild(light);
      lights[i] = light;
    }
  }
}

void ViewerApp::update(float elapsed, float delta) {
  mat4 rot = mat4::rot_y(yaw) * mat4::rot_x(pitch);

  {
    std::scoped_lock lock(camera->mutex());

    camera->setPerspective(70.0f, 0.1f, 500.0f);
    camera->setLocalTransform(mat4::translate(camera->getLocalPosition()) *
                              rot);
    camera->move(vel * delta * speed * (fastMode ? 2.0f : 1.0f), walkMode);
  }

  if (skyBox != nullptr) {
    std::scoped_lock lock(skyBox->mutex());
    skyBox->setLocalTransform(mat4::translate(camera->getLocalPosition()));
  }

  {
    std::scoped_lock lock(model_root->mutex());
    model_root->setLocalTransform(mat4::scale(rootScale));
  }

  scene->update(elapsed, delta);
  updateAll(root);

  AppBase::update(elapsed, delta);
}
