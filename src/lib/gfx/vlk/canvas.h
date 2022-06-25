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
#ifndef _CST_LIB_GFX_VLK_CANVAS_H
#define _CST_LIB_GFX_VLK_CANVAS_H

#include "core/worker.h"
#include "impl/commands.h"
#include "impl/device.h"
#include "impl/renderpass.h"
#include "impl/swapchain.h"
#include "math/ivec2.h"
#include "math/vec4.h"

namespace cst::vlk {

typedef void (*drawCallable)(int imageIndex);

/**
 * Canvas handles images and swap chains and does the
 * actual drawing operations.
 */
class Canvas : public Lockable {
public:
  Canvas(device_ptr dev, ivec2 const &wndSize, bool forceImmediate = false);
  ~Canvas();

  // Returns the size of the framebuffer.
  VkExtent2D getSize() const { return swapchain->getSize(); }

  // Returns the number of swap chain images in this canvas.
  size_t getNumImages() const { return swapchain->getNumImages(); }

  size_t getRunningFrames() const { return runningFrames; }

  swapchain_ptr getSwapchain() const { return swapchain; }

  void waitFences() const;

  // Called when canvas has to resized. The new size might not differ.
  void resized(VkSurfaceKHR surface, ivec2 const &newSize);

  int acquireImage(int frame);

  // Executes the previously built drawing commands.
  void draw(int frame, cmdbuf_ptr cmd, queue_ptr gfxQueue);
  void present(int frame, uint32_t imageIndex, VkQueue presentQueue);

private:

  // (Re-)create a swapchain.
  void createSwapchain(VkSurfaceKHR surface);

  void createSyncs(int numImages);

  device_ptr dev;
  ivec2 wndSize;
  bool forceImmediate;

  swapchain_ptr swapchain;

  int runningFrames;
  std::vector<VkSemaphore> imageAvailableSems;
  std::vector<VkSemaphore> renderFinishedSems;
  std::vector<VkFence> frameFences;
  std::vector<VkFence>
      imageFences; // Temporary pointers, does not own the fences

  std::vector<VkDescriptorSet> globalSets;
  std::vector<VkDescriptorSet> objectSets;
};

typedef std::shared_ptr<Canvas> canvas_ptr;

} // namespace cst::vlk

#endif // _CST_LIB_GFX_VLK_CANVAS_H
