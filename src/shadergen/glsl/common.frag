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
 
// Output attributes.
layout(location = 0) out vec4 outColor;

// Uniforms.
layout(set = DESC_SET_MATERIAL, binding = DESC_BIND_MAT_BUFFER, std430) uniform MaterialBuffer {
	vec4 albedo;
	vec4 emissive;
	float metallic;
	float roughness;
	float ao;
} mat;

// Albedo
layout(set = DESC_SET_MATERIAL, binding = DESC_BIND_MAT_ALBEDO_TEXTURE) uniform sampler2D albedoSampler;

// Roughness
layout(set = DESC_SET_MATERIAL, binding = DESC_BIND_MAT_ROUGHNESS_TEXTURE) uniform sampler2D roughSampler;

// Normal texture
layout(set = DESC_SET_MATERIAL, binding = DESC_BIND_MAT_NORMAL_TEXTURE) uniform sampler2D normSampler;

// Cubemap
layout(set = DESC_SET_MATERIAL, binding = DESC_BIND_MAT_CUBEMAP) uniform samplerCube cubeSampler;

vec3 normalMap() {
    return normalize(tbn * normalize(texture(normSampler, texCoord).rgb * 2.0 - 1.0));
}


