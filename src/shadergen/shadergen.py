#!/usr/bin/env python3
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
import os, sys
from varyings import *
from pathlib import Path

common = Path('glsl/common.glsl').read_text()
vert_common = Path('glsl/common.vert').read_text()
frag_common = Path('glsl/common.frag').read_text()

vertex_default = Path('glsl/default.vert').read_text()
vertex_cube = Path('glsl/cube.vert').read_text()

untextured_main = Path('glsl/main_untextured.frag').read_text()
unshaded_main = Path('glsl/main_unshaded.frag').read_text()
textured_norm_main = Path('glsl/main_textured_norm.frag').read_text()
textured_rough_norm_main = Path(
  'glsl/main_textured_rough_norm.frag').read_text()
textured_arm_norm_main = Path('glsl/main_textured_arm_norm.frag').read_text()
textured_main = Path('glsl/main_textured.frag').read_text()
unshaded_textured_cube_main = Path(
  'glsl/main_unshaded_textured_cube.frag').read_text()

pbr = Path('glsl/pbr.frag').read_text()
postproc = Path('glsl/postproc.frag').read_text()


def write_shader(name, txt, srcout, binout):
  with open(os.path.join(srcout, name), "w") as f:
    f.write(txt)

  cmd= f'glslangValidator -V --quiet "{os.path.join(srcout, name)}" ' + \
    f'-o {os.path.join(binout, name)}.spv'

  res = os.system(cmd)
  if res != 0:
    raise OSError(f"glslangValidator for {name} failed")


def generate_vertex_shaders(srcout, binout):
  flat_shader = common + vert_common + vertex_output_varyings(True) + \
   vertex_default
  smooth_shader = common + vert_common + vertex_output_varyings(False) + \
   vertex_default
  cube_shader = common + vert_common + vertex_output_varyings(False) + \
   vertex_cube

  write_shader("default_smooth.vert", smooth_shader, srcout, binout)
  write_shader("default_flat.vert", flat_shader, srcout, binout)
  write_shader("cube.vert", cube_shader, srcout, binout)


def generate_fragment_shaders(srcout, binout):
  write_shader(
    "flat.frag", common + fragment_input_varyings(True) + frag_common + pbr +
    postproc + untextured_main, srcout, binout)
  write_shader(
    "smooth.frag", common + fragment_input_varyings(False) + frag_common + pbr +
    postproc + untextured_main, srcout, binout)
  write_shader(
    "unshaded.frag", common + fragment_input_varyings(True) + frag_common +
    pbr + postproc + unshaded_main, srcout, binout)

  write_shader(
    "smooth_albedo_norm.frag", common + fragment_input_varyings(False) +
    frag_common + pbr + postproc + textured_norm_main, srcout, binout)

  write_shader(
    "smooth_albedo_rough_norm.frag", common + fragment_input_varyings(False) +
    frag_common + pbr + postproc + textured_rough_norm_main, srcout, binout)

  write_shader(
    "smooth_albedo_arm_norm.frag", common + fragment_input_varyings(False) +
    frag_common + pbr + postproc + textured_arm_norm_main, srcout, binout)

  write_shader(
    "flat_albedo.frag", common + fragment_input_varyings(True) + frag_common +
    pbr + postproc + textured_main, srcout, binout)

  write_shader(
    "smooth_albedo.frag", common + fragment_input_varyings(False) +
    frag_common + pbr + postproc + textured_main, srcout, binout)

  write_shader(
    "unshaded_albedo_cube.frag", common + fragment_input_varyings(False) +
    frag_common + postproc + unshaded_textured_cube_main, srcout, binout)


def generate_shaders(srcout, binout):
  #print(srcout)
  #print(binout)
  if not os.path.exists(srcout):
    os.makedirs(srcout)

  if not os.path.exists(binout):
    os.makedirs(binout)

  generate_vertex_shaders(srcout, binout)
  generate_fragment_shaders(srcout, binout)


generate_shaders(sys.argv[1], sys.argv[2])
