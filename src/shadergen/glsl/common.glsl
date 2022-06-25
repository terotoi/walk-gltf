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
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_scalar_block_layout : enable

const float PI = 3.1415926538;

const int DESC_SET_GLOBAL = 0;
const int DESC_SET_MATERIAL = 1;
const int DESC_SET_NODE = 2;

const int DESC_BIND_MAT_BUFFER = 0;
const int DESC_BIND_MAT_ALBEDO_TEXTURE = 1;
const int DESC_BIND_MAT_ROUGHNESS_TEXTURE = 2;
const int DESC_BIND_MAT_NORMAL_TEXTURE = 3;
const int DESC_BIND_MAT_CUBEMAP = 4;

const int MAX_LIGHTS = 8; // Must match MAX_LIGHTS in gfx/shader_data.h

/** Uniform buffers **/
layout(set = DESC_SET_GLOBAL, binding = 0, std430) uniform GlobalBuffer {
	mat4 project;
	mat4 view;
	vec4 viewPos;

	uint numLights;
	float time;

	vec4 ambientColor;
	vec4 lightPos[MAX_LIGHTS];
	vec4 lightColor[MAX_LIGHTS];
} glob;

layout(set = DESC_SET_NODE, binding = 0) uniform NodeBuffer {
	mat4 transform;
} node;


