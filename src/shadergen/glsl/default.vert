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

void main() {
	texCoord = in_texcoord;

	pos_w = (node.transform * vec4(in_pos, 1.0f)).xyz;
	normal_w = normalize((node.transform * vec4(in_normal, 0.0)).xyz);

	// TBN matrix
	const vec3 t = normalize(vec3(node.transform * vec4(in_tangent, 0.0)));
	const vec3 n = normal_w;
	const vec3 t2 = normalize(t - dot(t, n) * n);
	vec3 b = normalize(cross(n, t2));
	tbn	= mat3(t2, b, n);

	gl_Position = glob.project * glob.view * node.transform * vec4(in_pos, 1.0f);

}
