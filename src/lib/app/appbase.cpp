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
#include "appbase.h"
#include "core/dispatcher_instance.h"
#include "gfx/vlk/renderer.h"
#include "loader/gltf.h"
#include "math/geometry.h"
#include "sg/empty.h"
#include "sg/light.h"

#include <functional>
#include <iostream>

using namespace cst;
using namespace cst::vlk;
using namespace std::literals::chrono_literals;

static const vec4 CLEAR_COLOR = vec4(0.3f, 0.28f, 0.3f, 1.0f);

bool AppBase::fullscreen = false;
bool AppBase::borderless = false;
bool AppBase::grabMouse = true;
bool AppBase::doPrintFPS = false;

AppBase::AppBase(int reqWidth, int reqHeight) {
  auto vlkRenderer = std::make_unique<vlk::RendererVlk>(
      reqWidth, reqHeight, fullscreen, borderless, grabMouse);
  setupInput();
  renderer = std::move(vlkRenderer);
}

AppBase::~AppBase() {
  std::this_thread::sleep_for(100ms);
  renderer->flush();
  Texture::clearCache();

  destroyQueueDispatcher();
  destroyDispatcher();
}

void AppBase::setupInput() {
  events = std::make_unique<EventsProcessor>();

  events->setQuitHandler([this]() { running = false; });
  events->setKeyDownHandler(
      std::bind(&AppBase::keyDown, this, std::placeholders::_1));
  events->setKeyUpHandler(
      std::bind(&AppBase::keyUp, this, std::placeholders::_1));
  events->setMouseMoveHandler(std::bind(
      &AppBase::mouseMove, this, std::placeholders::_1, std::placeholders::_2));
}

node_ptr AppBase::stageAllAndCollect(node_ptr root) {
  node_ptr new_root = stageAll(root, renderer);
  std::scoped_lock lock(new_root->mutex(), visuals_mux);
  new_root->collectStaged(&visuals);

  sortNodes(visuals);
  return new_root;
}

void AppBase::sortNodes(std::vector<node_ptr> &visuals) {
  // Move any skybox to the end of the list
  std::sort(visuals.begin(), visuals.end(),
            [](node_ptr const &a, node_ptr const &b) {
              bool a_skybox = a->getName() == "skybox";
              bool b_skybox = b->getName() == "skybox";
              if (!a_skybox && b_skybox) {
                return true;
              }
              return false;
            });
}

void AppBase::updateAll(node_ptr root) {
  Renderer *rend = renderer.get();
  root->forEach([rend](node_ptr child) { child->update(rend); }, true);
  root->updateGlobalTransform(mat4());
}

void AppBase::cull(std::vector<node_ptr> const &src,
                   std::vector<node_ptr> &out) {
  mat4 const &viewProj = renderer->getProjectionView();

  for (auto &v : src) {
    if (v->isVisible() && !v->isCulled(viewProj)) {
      out.push_back(v);
    }
  }
}

void AppBase::run() {
  bool do_paint = true;

  events->setWindowResizedHandler([this, &do_paint]() {
    do_paint = false;

    std::this_thread::sleep_for(100ms);

    renderer->flush();
    renderer->windowResized();

    do_paint = true;
  });

  std::chrono::steady_clock::time_point start =
      std::chrono::steady_clock::now();
  float update_prev = 0.0f;
  float fps_prev = 0.0f;

  const size_t RUNNING_FRAMES = 5;

  GatedDispatcher<bool> dispatcher;
  std::condition_variable cv;
  std::counting_semaphore s{RUNNING_FRAMES};

  while (running) {
    events->processEvents();

    std::chrono::steady_clock::time_point now =
        std::chrono::steady_clock::now();
    std::chrono::duration<float, std::milli> elapsed_dur = now - start;
    float elapsed = elapsed_dur.count() / 1000.0f;

    float delta = elapsed - update_prev;
    update_prev = elapsed;

    update(elapsed, delta);

    if (do_paint) {
      if (s.try_acquire()) {

        dispatcher.add(
            [this, &s, &cv]() {
              {
                std::vector<node_ptr> visible;
                {
                  std::scoped_lock lock(visuals_mux);
                  frames++;
                  cull(visuals, visible);
                }
                renderer->render(visible);
              }

              s.release();
            },
            true, "paint");
      } else {
        std::this_thread::sleep_for(5ms);
      }
    }

    if (elapsed - fps_prev > 1.0f) {
      int fps = frames / (elapsed - fps_prev);
      frames = 0;
      fps_prev = elapsed;
      if (doPrintFPS)
        std::cout << "FPS: " << fps << "\n";
    }
  }
}
