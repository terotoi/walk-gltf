/*
 Copyright (c) 2022 Tero Oinas

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "fileutil.h"

#include <fstream>
#include <iostream>

std::vector<uint8_t> cst::loadFile(char const *fname) {
  std::ifstream fh(fname, std::ios::binary | std::ios::in | std::ios::ate);
  if (!fh.good())
    throw std::runtime_error(std::string("failed to open ") + fname);
  size_t size = fh.tellg();
  fh.seekg(0);

  std::vector<uint8_t> buf;
  buf.resize(size);

  fh.read((char *)(buf.data()), size);
  if (!fh.good())
    throw std::runtime_error("failed to read file");

  fh.close();
  return buf;
}

std::string cst::dirPart(std::string const &path) {
  const auto slash = path.rfind('/');
  if (slash == std::string::npos)
    return "./";
  else
    return path.substr(0, slash);
}
