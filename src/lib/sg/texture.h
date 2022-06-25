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
#ifndef _CST_LIB_SG_TEXTURE_H
#define _CST_LIB_SG_TEXTURE_H

#include "core/lockable.h"
#include "math/vec4.h"
#include "support/stb_image.h"

#include <memory>

namespace cst {

enum TextureType {
  TEXTURE_TYPE_ALBEDO = 1,
  TEXTURE_TYPE_ROUGHNESS,
  TEXTURE_TYPE_NORMAL,
  TEXTURE_TYPE_ARM
};

/**
 * Texture is a base class (interface) for texture implementations.
 */
class Texture : public Lockable {
public:
  /**
   * @param name name of the texture
   */
  Texture(std::string const &name, TextureType type) : name(name), type(type) {}

  Texture(Texture const &tex) : name(tex.name) {}

  virtual ~Texture() {}

  /**
   * Returns the name of the texture.
   */
  std::string const &getName() const { return name; }

  /**
   * Returns the type of the texture.
   */
  TextureType getType() const { return type; }

  virtual int getWidth() const = 0;
  virtual int getHeight() const = 0;
  virtual int getDepth() const = 0;

  // Returns the number of layers in the texture.
  // For normal textures, this is 1.
  // For cubemaps, this is 6
  virtual int getLayers() const = 0;

  bool isCubeMap() const { return getLayers() == 6; }

  virtual uint8_t *getPixels() const = 0;

  // Returns true if this texture is a visual texture and is staged.
  virtual bool isStaged() const = 0;

  // Load this texture into memory.
  virtual void load() = 0;

  // Returns a name texture from cache.
  static std::shared_ptr<Texture> getNamed(std::string const &name);

  // Stores a texture in a cache.
  static void storeNamed(std::shared_ptr<Texture> tex);

  // Clears the texture cache.
  static void clearCache();

  static bool calculateAverageColors;
private:
  std::string name;
  TextureType type;
};

typedef std::shared_ptr<Texture> texture_ptr;

/**
 * TextureStd is a texture with a name and possibly a pixmap in system memory.
 */
class TextureStd : public Texture {
public:
  /**
   * @param filename filename of the texture
   */
  TextureStd(std::string const &filename, TextureType type)
      : Texture(filename, type), filename(filename) {}
  ~TextureStd();

  int getWidth() const override { return width; };

  int getHeight() const override { return height; };

  int getDepth() const override { return depth; };

  int getLayers() const override { return 1; };

  uint8_t *getPixels() const override;

  bool isStaged() const override { return false; }

  void load() override;

private:
  std::string filename;
  int width, height, depth;
  int layers = 1;
  stbi_uc *pix = nullptr;
  vec4 color;
};

} // namespace cst

#endif // _CST_LIB_SG_TEXTURE_H