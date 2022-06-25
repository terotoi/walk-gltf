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
#include "swapchain.h"

#include <algorithm>
#include <cassert>
#include <iostream>

using namespace cst::vlk;

Swapchain::Swapchain(device_ptr dev, VkPhysicalDevice physDev,
                     VkSurfaceKHR surface,
                     std::vector<uint32_t> queueFamilyIndices,
                     VkExtent2D const &size, bool forceImmediate,
                     VkSwapchainKHR old)
    : dev(dev) {
  VkSurfaceCapabilitiesKHR caps;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDev, surface, &caps);

  // Formats
  uint32_t numFormats;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physDev, surface, &numFormats, nullptr);
  std::vector<VkSurfaceFormatKHR> formats(numFormats);
  vkGetPhysicalDeviceSurfaceFormatsKHR(physDev, surface, &numFormats,
                                       formats.data());


  if(numFormats == 0)
    throw std::runtime_error("No surface formats found");

  bool foundFormat = false;
  VkColorSpaceKHR colorSpace;

  for(auto const &f : formats) {
    if(f.format == VK_FORMAT_B8G8R8A8_UNORM) {
      format = f.format;
      colorSpace = f.colorSpace;
      foundFormat = true;
      break;
    }
  }

  if (!foundFormat)
    throw std::runtime_error(
        "failed to find a suitable image format for a swapchain");

  // Presentation modes
  uint32_t numModes;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physDev, surface, &numModes,
                                            nullptr);
  std::vector<VkPresentModeKHR> modes(numModes);
  vkGetPhysicalDeviceSurfacePresentModesKHR(physDev, surface, &numModes,
                                            modes.data());

  VkPresentModeKHR presentMode;

  if (forceImmediate) {
    std::cout << "Forcing immediate presentation mode.\n";
    presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
  } else {
    presentMode = VK_PRESENT_MODE_FIFO_KHR;
    if (std::find(modes.begin(), modes.end(), VK_PRESENT_MODE_MAILBOX_KHR) !=
        modes.end()) {
      presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
      std::cout << "Using mailbox presentation mode.\n";
    } else if (std::find(modes.begin(), modes.end(),
                         VK_PRESENT_MODE_FIFO_RELAXED_KHR) != modes.end()) {
      presentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
      std::cout << "Using relaxed FIFO presentation mode.\n";
    } else
      std::cout << "Defaulting to FIFO presentation mode.\n";
  }

  VkSwapchainCreateInfoKHR info{};
  info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  info.surface = surface;
  info.oldSwapchain = old;

  info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  info.imageFormat = format;
  info.imageColorSpace = colorSpace;
  info.presentMode = presentMode;

  VkExtent2D extent{};
  extent.width = caps.currentExtent.width;   // size.width;
  extent.height = caps.currentExtent.height; // size.height;
  info.imageExtent = extent;
  this->size = extent;

  info.minImageCount = caps.minImageCount + 1;
  if (caps.maxImageCount > 0 && info.minImageCount > caps.maxImageCount)
    info.minImageCount = caps.maxImageCount;
  info.preTransform = caps.currentTransform;
  info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  info.clipped = VK_TRUE;

  info.imageArrayLayers = 1;
  info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
    info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    info.queueFamilyIndexCount = queueFamilyIndices.size();
    info.pQueueFamilyIndices = queueFamilyIndices.data();
  } else
    info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkResult res;
  if ((res = vkCreateSwapchainKHR(*dev, &info, nullptr, &sc)) != VK_SUCCESS) {
    std::cerr << "vkCreateSwapchainKHR() => " << res << std::endl;
    throw std::runtime_error("failed to create a swapchain");
  }

  // Query the swap chain images
  uint32_t numImages;
  vkGetSwapchainImagesKHR(*dev, sc, &numImages, nullptr);
  images.resize(numImages);
  vkGetSwapchainImagesKHR(*dev, sc, &numImages, images.data());

  createImageViews();
}

Swapchain::~Swapchain() {
  for (auto iv : imageViews)
    vkDestroyImageView(*dev, iv, nullptr);

  vkDestroySwapchainKHR(*dev, sc, nullptr);
}

void Swapchain::createImageViews() {
  imageViews.resize(images.size());

  for (size_t i = 0; i < images.size(); i++) {
    VkImageViewCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.image = images[i];
    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.format = format;

    info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.layerCount = 1;
    if (vkCreateImageView(*dev, &info, nullptr, &imageViews[i]) != VK_SUCCESS)
      throw std::runtime_error("failed to create an image view");
  }
}

std::vector<framebuffer_ptr>
Swapchain::createFramebuffers(renderpass_ptr renderPass) {
  std::vector<framebuffer_ptr> fbs(imageViews.size());

  for (size_t i = 0; i < imageViews.size(); i++) {
    fbs[i] = std::make_shared<Framebuffer>(
        dev, size, format, VK_SAMPLE_COUNT_8_BIT, *renderPass, imageViews[i]);
  }

  return fbs;
}
