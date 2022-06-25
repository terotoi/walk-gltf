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
#include "canvas.h"

#include <cassert>
#include <iostream>

const int DEFAULT_RUNNING_FRAMES = 5;

using namespace cst::vlk;

Canvas::Canvas(device_ptr dev, ivec2 const &wndSize, bool forceImmediate)
    : dev(dev), wndSize(wndSize), forceImmediate(forceImmediate),
      runningFrames(DEFAULT_RUNNING_FRAMES) {
  resized(dev->getSurface(), wndSize);
}

Canvas::~Canvas() {
  swapchain = nullptr;

  for (auto fence : frameFences) {
    vkDestroyFence(*dev, fence, nullptr);
  }

  for (auto sem : renderFinishedSems)
    vkDestroySemaphore(*dev, sem, nullptr);

  for (auto sem : imageAvailableSems)
    vkDestroySemaphore(*dev, sem, nullptr);
}

void Canvas::waitFences() const {
  vkWaitForFences(*dev, frameFences.size(), frameFences.data(), true,
                  UINT64_MAX);
}

void Canvas::resized(VkSurfaceKHR surface, ivec2 const &newSize) {

  if (swapchain == nullptr || (newSize != wndSize)) {
    if (newSize.x() != 0 || newSize.y() == 0)
      createSwapchain(surface);
  }
  wndSize = newSize;
}

void Canvas::createSwapchain(VkSurfaceKHR surface) {
  swapchain_ptr new_chain = std::make_shared<Swapchain>(
      dev, dev->getPhysicalDevice(), surface, dev->getQueueFamilyIndices(),
      VkExtent2D{(uint32_t)wndSize.width(), (uint32_t)wndSize.height()},
      forceImmediate,
      (swapchain != nullptr) ? VkSwapchainKHR(*swapchain) : VK_NULL_HANDLE);

  if (new_chain->getNumImages() > (size_t)runningFrames) {
    runningFrames = new_chain->getNumImages();
    // std::cout << "Running frames: " << runningFrames << "\n";
  }

  createSyncs(new_chain->getNumImages());

  swapchain = new_chain;
}

void Canvas::createSyncs(int numImages) {
  for (auto fence : frameFences)
    vkDestroyFence(*dev, fence, nullptr);
  for (auto sem : renderFinishedSems)
    vkDestroySemaphore(*dev, sem, nullptr);
  for (auto sem : imageAvailableSems)
    vkDestroySemaphore(*dev, sem, nullptr);

  imageAvailableSems.resize(runningFrames, VK_NULL_HANDLE);
  renderFinishedSems.resize(runningFrames, VK_NULL_HANDLE);
  frameFences.resize(runningFrames, VK_NULL_HANDLE);

  imageFences.resize(numImages); // Does not own the fences
  for (int i = 0; i < numImages; i++)
    imageFences[i] = VK_NULL_HANDLE;

  VkSemaphoreCreateInfo sinfo{};
  sinfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo finfo{};
  finfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  finfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (int i = 0; i < runningFrames; i++) {
    if (vkCreateSemaphore(*dev, &sinfo, nullptr, &imageAvailableSems[i]) !=
        VK_SUCCESS)
      throw std::runtime_error("failed to create a semaphore");

    if (vkCreateSemaphore(*dev, &sinfo, nullptr, &renderFinishedSems[i]) !=
        VK_SUCCESS)
      throw std::runtime_error("failed to create a semaphore");

    if (vkCreateFence(*dev, &finfo, nullptr, &frameFences[i]) != VK_SUCCESS)
      throw std::runtime_error("failed to create a fence");
  }
}

int Canvas::acquireImage(int frame) {
  uint32_t fr = frame % runningFrames;

  // Acquire an image
  uint32_t imageIndex;
  VkResult res =
      vkAcquireNextImageKHR(*dev, *swapchain, INT64_MAX, imageAvailableSems[fr],
                            VK_NULL_HANDLE, &imageIndex);

  if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
    std::cerr << "Out of date KHR\n";
    return -1;
  } else if (res == VK_TIMEOUT) {
    std::cerr << "Timeout of acquire image\n";
    return -1;
  } else if (res != VK_SUCCESS) {
    std::cerr << "failed to acquire next image\n";
    return -1;
  }

  if (imageFences[imageIndex] != VK_NULL_HANDLE) {
    vkWaitForFences(*dev, 1, &imageFences[imageIndex], VK_TRUE, UINT64_MAX);
  }
  imageFences[imageIndex] = frameFences[fr];
  return imageIndex;
}

void Canvas::draw(int frame, cmdbuf_ptr cmd, queue_ptr gfxQueue) {
  uint32_t fr = frame % runningFrames;
  vkResetFences(*dev, 1, &frameFences[fr]);
  cmd->submit(gfxQueue, imageAvailableSems[fr], renderFinishedSems[fr],
              frameFences[fr]);
}

void Canvas::present(int frame, uint32_t imageIndex, VkQueue presentQueue) {
  uint32_t fr = frame % runningFrames;

  // Present the image
  VkPresentInfoKHR present{};
  present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present.waitSemaphoreCount = 1;
  present.pWaitSemaphores = &renderFinishedSems[fr];

  VkSwapchainKHR scs[] = {*swapchain};
  present.swapchainCount = 1;
  present.pSwapchains = scs;
  present.pImageIndices = &imageIndex;

  vkQueuePresentKHR(presentQueue, &present);
}
