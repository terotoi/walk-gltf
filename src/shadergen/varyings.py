# Copyright (c) 2022 Tero Oinas
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is furnished to do
# so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

def vertex_output_varyings(flat):
	txt = """
/** Output varyings **/
layout(location = 0) out vec2 texCoord;
"""

	if flat:
		txt += "layout(location = 1) out flat vec3 normal_w;"
	else:
		txt += "layout(location = 1) out vec3 normal_w;"

	txt += """
layout(location = 2) out vec3 pos_w;
layout(location = 3) out vec3 pos_local;
layout(location = 4) out mat3 tbn;

out gl_PerVertex {
	vec4 gl_Position;
};
"""
	return txt

def fragment_input_varyings(flat):
	txt = """
// Input varyings.
layout(location = 0) in vec2 texCoord;
"""

	if flat:
		txt += "layout(location = 1) in flat vec3 normal_w;"
	else:
		txt += "layout(location = 1) in vec3 normal_w;"

	txt += """
layout(location = 2) in vec3 pos_w;
layout(location = 3) in vec3 pos_local;
layout(location = 4) in mat3 tbn;
"""
	return txt

