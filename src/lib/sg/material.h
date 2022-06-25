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
#ifndef _CST_LIB_SG_MATERIAL_H
#define _CST_LIB_SG_MATERIAL_H

#include <memory>

#include "core/lockable.h"
#include "math/vec4.h"
#include "texture.h"

namespace cst {

enum ShadeMode { SHADE_MODE_UNSHADED = 0, SHADE_MODE_SMOOTH, SHADE_MODE_FLAT };

/**
 *  Material
 */
class Material : public Lockable {
public:
  Material() {}
  virtual ~Material() {}

  // Returns true if this material is staged (i.e. is ready to be displayed)
  virtual bool isStaged() const = 0;

  virtual ShadeMode getShadeMode() const = 0;

  virtual vec4 const &getAlbedo() const = 0;
  virtual void setAlbedo(vec4 const &color) = 0;

  virtual float getMetallic() const = 0;
  virtual float getRoughness() const = 0;
  virtual float getAO() const = 0;

  virtual bool isDoubleSided() const = 0;

  virtual vec4 const &getEmissiveColor() const = 0;
  virtual void setEmissiveColor(vec4 const &color) = 0;

  virtual void setAlbedoTex(texture_ptr tex) = 0;
  virtual texture_ptr getAlbedoTex() const = 0;

  virtual void setRoughnessTex(texture_ptr tex, bool isArm) = 0;
  virtual texture_ptr getRoughnessTex() const = 0;
  virtual bool isRoughnessArm() const = 0;

  virtual void setNormalTex(texture_ptr tex) = 0;
  virtual texture_ptr getNormalTex() const = 0;

private:
};

typedef std::shared_ptr<Material> material_ptr;

/**
 *  MaterialStd
 */
class MaterialStd : public Material {
public:
  MaterialStd(ShadeMode mode, vec4 const &albedo, float metallic,
              float roughness, float ao, bool doubleSided)
      : shadeMode(mode), albedo(albedo), metallic(metallic),
        roughness(roughness), ao(ao), doubleSided(doubleSided) {}
  ~MaterialStd() {}

  bool isStaged() const override { return false; }

  ShadeMode getShadeMode() const override { return shadeMode; }
  vec4 const &getAlbedo() const override { return albedo; }
  void setAlbedo(vec4 const &color) override { albedo = color; }

  float getMetallic() const override { return metallic; }
  float getRoughness() const override { return roughness; }
  float getAO() const override { return ao; }

  vec4 const &getEmissiveColor() const override { return emissiveColor; }
  void setEmissiveColor(vec4 const &color) override { emissiveColor = color; }

  bool isDoubleSided() const override { return doubleSided; }

  void setAlbedoTex(texture_ptr tex) override { albedoTex = tex; }
  texture_ptr getAlbedoTex() const override { return albedoTex; }

  void setRoughnessTex(texture_ptr tex, bool isArm) override {
    roughnessTex = tex;
    roughnessArm = isArm;
  }

  texture_ptr getRoughnessTex() const override { return roughnessTex; }

  bool isRoughnessArm() const { return roughnessArm; }

  void setNormalTex(texture_ptr tex) override { normalTex = tex; }
  texture_ptr getNormalTex() const override { return normalTex; }

private:
  ShadeMode shadeMode;
  vec4 albedo;
  float metallic;
  float roughness;
  float ao;
  vec4 emissiveColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
  bool doubleSided;

  texture_ptr albedoTex;
  texture_ptr roughnessTex;
  bool roughnessArm = false;
  texture_ptr normalTex;
};

} // namespace cst

#endif //_CST_LIB_SG_MATERIAL_H
