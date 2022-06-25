#include "shader.h"
#include "core/fileutil.h"

#include <stdexcept>
#include <iostream>

using namespace cst::vlk;

Shader::Shader(device_ptr dev, std::string const& filename)
    : dev(dev), name(filename) {
  auto code = loadFile(filename.c_str());

  VkShaderModuleCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  info.codeSize = code.size(),
  info.pCode = reinterpret_cast<const uint32_t*>(code.data());

  if (vkCreateShaderModule(*dev, &info, nullptr, &module) != VK_SUCCESS)
    throw std::runtime_error("failed to create a shader module");
}

Shader::~Shader() {
  vkDestroyShaderModule(*dev, module, nullptr);
}
