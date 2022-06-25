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
#include "texture.h"

using namespace cst::vlk;
using namespace cst;

TextureVlk::TextureVlk(device_ptr dev, std::string const &name, image_ptr image,
                       sampler_ptr sampler, int layers, int mipLevels, TextureType type)
    : Texture(name, type), dev(dev), image(image), sampler(sampler), layers(layers) {
  VkImageViewCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  info.image = *image;
  info.viewType = (layers == 6) ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D;
  info.format = (type == TEXTURE_TYPE_ALBEDO) ? VK_FORMAT_R8G8B8A8_SRGB
                                              : VK_FORMAT_R8G8B8A8_UNORM;
  info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  info.subresourceRange.levelCount = mipLevels;
  info.subresourceRange.layerCount = layers;

  if (vkCreateImageView(*dev, &info, nullptr, &view) != VK_SUCCESS)
    throw std::runtime_error("failed to create an image view");
}

TextureVlk::~TextureVlk() { vkDestroyImageView(*dev, view, nullptr); }

int TextureVlk::getWidth() const {
  throw std::runtime_error("Not implemented");
}

int TextureVlk::getHeight() const {
  throw std::runtime_error("Not implemented");
}

int TextureVlk::getDepth() const {
  throw std::runtime_error("Not implemented");
}

uint8_t *TextureVlk::getPixels() const {
  throw std::runtime_error("Not implemented");
}

void TextureVlk::load() { throw std::runtime_error("Not implemented"); }
