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
#include "app.h"

using namespace cst;
using namespace cst::app;

static void printHelp(std::string const &progname) {
  std::cout << "Usage: " << progname << " [options] filename" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -f          Full screen\n";
  std::cout << "  -b          Borderless window\n";
  std::cout << "  -g          Don't grab mouse\n";
  std::cout << "  -s [path]   Use a skybox from the given directory\n";
  std::cout << "              Uses the following filenames: px.jpg, nx.jpg, py.jpg, ny.jpg, pz.jpg, ng.jp\n";
  std::cout << "  -l          Do not add extra lights\n";
  std::cout << "  -fps        Print FPS to stdout\n";
  std::cout << "  -n          Force flat shading\n";
  std::cout << "  -x          Deduplicate vertices\n";
  std::cout << "  -t          Do not load textures\n";
  std::cout << "  -h          Print this help" << std::endl;
}

int main(int argc, char **argv) {
  std::string rendererName = "vlk";
  bool fullScreen = false;
  bool borderless = false;
  bool grabMouse = true;
  bool flatShading = false;
  bool deduplicateVertices = true;
  bool doLoadTextures = true;
  bool doAddExtraLights = true;
  bool doPrintHelp = false;
  bool doPrintFPS = false;
  std::string modelName;
  std::string skyboxPath;

  for (int i = 1; i < argc; i++) {
    std::string const arg(argv[i]);

    if (arg == "-f")
      fullScreen = !fullScreen;
    else if (arg == "-b")
      borderless = !borderless;
    else if (arg == "-g")
      grabMouse = false;
    else if (arg == "-s" && argc > i + 1) {
      skyboxPath = argv[i + 1];
    } else if (arg == "-l")
      doAddExtraLights = !doAddExtraLights;
    else if (arg == "-n")
      flatShading = !flatShading;
    else if (arg == "-x") {
      deduplicateVertices = !deduplicateVertices;
    } else if (arg == "-t") {
      doLoadTextures = !doLoadTextures;
    } else if (arg == "-l") {
      doAddExtraLights = !doAddExtraLights;
    } else if (arg == "-h") {
      doPrintHelp = true;
    } else if (arg == "-fps") {
      doPrintFPS = !doPrintFPS;
    } else if (arg[0] != '-') {
      modelName = arg;
    }
  }

  if (doPrintHelp || modelName.empty()) {
    printHelp(argv[0]);
    return 0;
  }

  // Find the path of the executable
  std::string argvPath(argv[0]);
  std::string const programPath  =
      argvPath.substr(0, argvPath.find_last_of("/\\")) + "/";

  ViewerApp::fullscreen = fullScreen;
  ViewerApp::borderless = borderless;
  ViewerApp::grabMouse = grabMouse;
  ViewerApp::doPrintFPS = doPrintFPS;
  ViewerApp::doLoadTextures = doLoadTextures;

  try {
    ViewerApp app(0, 0, programPath);

    std::function<void()> cb = []() {};

    if (doAddExtraLights)
      cb = std::bind(&ViewerApp::addLights, &app);

    if (skyboxPath != "")
      app.loadSkybox(skyboxPath);
    app.loadModel(modelName, cb, flatShading, deduplicateVertices);

    app.run();
  } catch (std::runtime_error const &error) {
    std::cerr << "Runtime error: " << error.what() << std::endl;
  }
  return 0;
}
