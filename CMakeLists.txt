cmake_minimum_required(VERSION 3.7)
project(walk-gltf)

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Werror")

find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(Threads)

set(SOURCES_lib
	src/lib/core/fileutil.cpp
	src/lib/core/dispatcher_instance.cpp
	src/lib/gfx/renderer.cpp
	src/lib/loader/gltf.cpp
	src/lib/input/events.cpp
	src/lib/math/aabb.cpp
	src/lib/math/geometry.cpp
	src/lib/math/ivec2.cpp
	src/lib/math/mat4.cpp
	src/lib/math/mathutil.cpp
	src/lib/math/quat.cpp
	src/lib/math/vec2.cpp
	src/lib/math/vec3.cpp
	src/lib/math/vec4.cpp
	src/lib/math/vertex.cpp
	src/lib/sg/camera.cpp
	src/lib/sg/cubetexture.cpp
	src/lib/sg/light.cpp
	src/lib/sg/node.cpp
	src/lib/sg/nodeutil.cpp
	src/lib/sg/scene.cpp
	src/lib/sg/texture.cpp
	src/lib/app/appbase.cpp)

set(SOURCES_gfx_vlk
	src/lib/gfx/vlk/impl/buffer.cpp
	src/lib/gfx/vlk/impl/commands.cpp
	src/lib/gfx/vlk/impl/descs.cpp
	src/lib/gfx/vlk/impl/device.cpp
	src/lib/gfx/vlk/impl/image.cpp
	src/lib/gfx/vlk/impl/framebuffer.cpp
	src/lib/gfx/vlk/impl/pipeline.cpp
	src/lib/gfx/vlk/impl/queue.cpp
	src/lib/gfx/vlk/impl/renderpass.cpp
	src/lib/gfx/vlk/impl/shader.cpp
	src/lib/gfx/vlk/impl/swapchain.cpp
	src/lib/gfx/vlk/canvas.cpp
	src/lib/gfx/vlk/material.cpp
	src/lib/gfx/vlk/mesh.cpp
	src/lib/gfx/vlk/node.cpp
	src/lib/gfx/vlk/renderer.cpp
	src/lib/gfx/vlk/sampler.cpp
	src/lib/gfx/vlk/texture.cpp
	src/lib/gfx/vlk/queue_dispatcher.cpp)

set(SOURCES_support
	src/support/stb_image.cpp
	src/support/tiny_gltf.cpp
	src/support/vk_mem_alloc.cpp)

set_source_files_properties(src/support/tiny_gltf.cpp PROPERTIES COMPILE_FLAGS -Wno-switch)
set_source_files_properties(src/support/vk_mem_alloc.cpp PROPERTIES COMPILE_FLAGS
	"-Wno-nullability-completeness -Wno-unused-variable")

set(LIB_SOURCES ${SOURCES_lib} ${SOURCES_gfx_vlk} ${SOURCES_support})

set(LIB_INCLUDE_DIR
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/src/lib
	${PROJECT_SOURCE_DIR}/src/support)

### Shaders ###

add_custom_target(spirv_shaders ALL DEPENDS
	shaders/smooth_albedo.frag.spv)

add_custom_command(
	OUTPUT shaders/smooth_albedo.frag.spv
	COMMAND cd ../src/shadergen && ./shadergen.py
		"${CMAKE_CURRENT_BINARY_DIR}/shader_src" "${CMAKE_CURRENT_BINARY_DIR}/shaders"
    DEPENDS src/shadergen/shadergen.py
		src/shadergen/glsl/common.glsl
		src/shadergen/glsl/common.vert
		src/shadergen/glsl/common.frag
		src/shadergen/glsl/default.vert
		src/shadergen/glsl/cube.vert
		src/shadergen/glsl/main_untextured.frag
		src/shadergen/glsl/main_unshaded.frag
		src/shadergen/glsl/main_textured_norm.frag
		src/shadergen/glsl/main_textured_rough_norm.frag
		src/shadergen/glsl/main_textured_arm_norm.frag
		src/shadergen/glsl/main_textured.frag
		src/shadergen/glsl/main_unshaded_textured_cube.frag
		src/shadergen/glsl/pbr.frag
		src/shadergen/glsl/postproc.frag
		)

### Library

add_library(walk ${LIB_SOURCES})
add_dependencies(walk spirv_shaders)
target_include_directories(walk PRIVATE ${LIB_INCLUDE_DIR} ${SDL2_INCLUDE_DIRS})
target_link_libraries(walk ${SDL2_LIBRARIES} Vulkan::Vulkan ${CMAKE_THREAD_LIBS_INIT})

### Viewer application ###

add_executable(walk-gltf
	src/apps/viewer/app.cpp
	src/apps/viewer/input.cpp
	src/apps/viewer/main.cpp)

add_compile_definitions(SHADERS_PATH="${CMAKE_INSTALL_PREFIX}/share/walk-gltf/shaders" BUILD_TYPE="${CMAKE_BUILD_TYPE}")

target_include_directories(walk-gltf PRIVATE ${LIB_INCLUDE_DIR} ${SDL2_INCLUDE_DIRS})
target_link_libraries(walk-gltf walk)

message("Install prefix: " ${CMAKE_INSTALL_PREFIX})

install(TARGETS walk-gltf DESTINATION bin)
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/shaders" DESTINATION ${CMAKE_INSTALL_PREFIX}/share/walk-gltf)

