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

using namespace cst;
using namespace cst::app;

void ViewerApp::keyDown(SDL_Keycode key) {
  switch (key) {
  case SDLK_w:
    vel = vel + vec3(0.0f, 0.0f, -1.0f);
    break;

  case SDLK_s:
    vel = vel + vec3(0.0f, 0.0f, 1.0f);
    break;

  case SDLK_a:
    vel = vel + vec3(-1.0f, 0.0f, 0.0f);
    break;

  case SDLK_d:
    vel = vel + vec3(1.0f, 0.0f, 0.0f);
    break;

  case SDLK_r:
    vel = vel + vec3(0.0f, 1.0f, 0.0f);
    break;

  case SDLK_f:
    vel = vel + vec3(0.0f, -1.0f, 0.0f);
    break;

  case SDLK_5:
  case SDLK_KP_5:
    walkMode = !walkMode;
    break;

  case SDLK_PLUS:
  case SDLK_KP_PLUS:
    speed += 2.5f;
    break;

  case SDLK_MINUS:
  case SDLK_KP_MINUS:
    speed -= 2.5f;
    if (speed <= 0.0f)
      speed = 1.0f;
    break;

  case SDLK_LSHIFT:
    fastMode = true;
    break;

  case SDLK_7:
  case SDLK_KP_7:
    rootScale += 0.01f;
    if (rootScale > 2.0f)
      rootScale = 2.0f;
    break;

  case SDLK_4:
  case SDLK_KP_4:
    rootScale = 1.0f;
    break;

  case SDLK_1:
  case SDLK_KP_1:
    rootScale -= 0.01f;
    if (rootScale < 0.1f)
      rootScale = 0.1f;
    break;
  }

  if (vel.len() > 0)
    vel = vel.normalize();
}

void ViewerApp::keyUp(SDL_Keycode key) {
  switch (key) {
  case SDLK_w:
  case SDLK_s:
    vel.d[2] = 0.0f;
    break;

  case SDLK_a:
  case SDLK_d:
    vel.d[0] = 0.0f;
    break;

  case SDLK_r:
  case SDLK_f:
    vel.d[1] = 0.0f;
    break;

  case SDLK_ESCAPE:
    quit();
    break;

  case SDLK_LSHIFT:
    fastMode = false;
    break;
  }

  if (vel.len() > 0)
    vel = vel.normalize();
}

void ViewerApp::mouseMove(int mx, int my) {
  const float mouseSensitivity = 0.002f;
  yaw += -mx * mouseSensitivity;
  pitch += -my * mouseSensitivity;
}
