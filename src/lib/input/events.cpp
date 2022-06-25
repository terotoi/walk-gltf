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
#include "events.h"

using namespace cst;

void EventsProcessor::processEvents()
{
  SDL_Event ev;
  while (SDL_PollEvent(&ev))
  {
    switch (ev.type)
    {

    case SDL_QUIT:
      if (quitHandler)
        quitHandler();
      break;

    case SDL_KEYDOWN:
      if (keyDownHandler)
        keyDownHandler(ev.key.keysym.sym);
      break;

    case SDL_KEYUP:
      if (keyUpHandler)
        keyUpHandler(ev.key.keysym.sym);
      break;

    case SDL_MOUSEMOTION:
      if (mouseMoveHandler)
        mouseMoveHandler(ev.motion.xrel, ev.motion.yrel);
      break;

    case SDL_WINDOWEVENT:
      if (ev.window.event == SDL_WINDOWEVENT_RESIZED && windowResizedHandler)
        windowResizedHandler();
      break;

    default:
      break;
    }
  }
}