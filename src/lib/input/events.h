/*
 Copyright (c) 2022 Tero Oinas

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef _CST_LIB_INPUT_EVENTS_H
#define _CST_LIB_INPUT_EVENTS_H

#include <SDL_events.h>
#include <functional>

namespace cst {

/**
 * EventsProcessor
 */
class EventsProcessor {
public:
  EventsProcessor() {}

  void setKeyDownHandler(std::function<void(SDL_Keycode key)> f) {
    keyDownHandler = f;
  }

  void setKeyUpHandler(std::function<void(SDL_Keycode key)> f) {
    keyUpHandler = f;
  }

  void setMouseMoveHandler(std::function<void(int dx, int dy)> f) {
    mouseMoveHandler = f;
  }

  void setQuitHandler(std::function<void()> f) { quitHandler = f; }
  void setWindowResizedHandler(std::function<void()> f) {
    windowResizedHandler = f;
  }

  void processEvents();

private:
  std::function<void(SDL_Keycode)> keyDownHandler;
  std::function<void(SDL_Keycode)> keyUpHandler;
  std::function<void(int dx, int dy)> mouseMoveHandler;
  std::function<void()> quitHandler;
  std::function<void()> windowResizedHandler;
};

} // namespace cst

#endif // _CST_LIB_INPUT_EVENTS_H
