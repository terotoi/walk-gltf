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
#ifndef _CST_LIB_GFX_SHADER_DATA_H
#define _CST_LIB_GFX_SHADER_DATA_H

#include "math/mat4.h"

#define DESC_SET_GLOBAL 0
#define DESC_SET_MATERIAL 1
#define DESC_SET_NODE 2

#define BIND_GLOBAL_UBO 0
#define BIND_MAT_UBO 0
#define BIND_MAT_ALBEDO_TEXTURE 1
#define BIND_MAT_ROUGHNESS_TEXTURE 2
#define BIND_MAT_NORMAL_TEXTURE 3
#define BIND_MAT_CUBEMAP 4

#define BIND_NODE_UBO 0

#define MAX_LIGHTS 8

namespace cst::vlk {

// GlobalData is in the format ofthe global UBOs.
struct GlobalData {
  alignas(16) mat4 project;
  alignas(16) mat4 view;
  alignas(16) vec4 viewPos;

  alignas(4) uint32_t numLights;
  alignas(4) float time;

  alignas(16) vec4 ambientColor;
  alignas(16) vec4 lightPos[MAX_LIGHTS];
  alignas(16) vec4 lightColor[MAX_LIGHTS];
};

// MaterialData is in format of the material UBOs.
struct MaterialData {
  alignas(16) vec4 albedo;
  alignas(16) vec4 emissiveColor;
  alignas(4) float metallic;
  alignas(4) float roughness;
  alignas(4) float ao;
};

// NodeData is in format of the node UBOs.
struct NodeData {
  alignas(16) mat4 transform; 
};

} // namespace cst::vlk

#endif // _CST_LIB_GFX_SHADER_DATA_H
