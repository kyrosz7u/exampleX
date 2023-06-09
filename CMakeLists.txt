cmake_minimum_required(VERSION 3.22)
project(XExample)

set(CMAKE_CXX_STANDARD 17)
set(RENDER_TARGET_NAME XRender)
set(TARGET_NAME XExample)
#set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_BUILD_TYPE "Debug")

set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/3rdparty)
set(SHADER_COMPILE_TARGET ShaderCompile)
set(vulkan_include ${THIRD_PARTY_DIR}/VulkanSDK/include)
set(glfw_include ${THIRD_PARTY_DIR}/glfw/include)
set(assimp_include ${THIRD_PARTY_DIR}/assimp/include)
set(imgui_include ${THIRD_PARTY_DIR}/imgui/include)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-D_DEBUG)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-D_RELEASE)
endif()

add_definitions(-DMULTI_THREAD_RENDERING)
add_definitions(-DFORWARD_RENDERING)

if(APPLE)
    set(vulkan_lib ${THIRD_PARTY_DIR}/VulkanSDK/lib/MacOS/libvulkan.1.dylib)
    set(assimp_lib ${THIRD_PARTY_DIR}/assimp/lib/MacOS/arm64/libassimp.5.dylib)
    set(glslangValidator_executable ${THIRD_PARTY_DIR}/VulkanSDK/bin/MacOS/glslangValidator)
    set(GLFW_PLATFORM "COCOA")
    add_compile_definitions("VK_LAYER_PATH=${THIRD_PARTY_DIR}/VulkanSDK/bin/MacOS")
    add_compile_definitions("VK_ICD_FILENAMES=${THIRD_PARTY_DIR}/VulkanSDK/bin/MacOS/MoltenVK_icd.json")
elseif(WIN32)
    set(vulkan_lib ${THIRD_PARTY_DIR}/VulkanSDK/lib/Win32/vulkan-1.lib)
    set(assimp_lib ${THIRD_PARTY_DIR}/assimp/lib/Win32/assimp-vc142-mt.lib)
    set(assimp_dll ${THIRD_PARTY_DIR}/assimp/dll/assimp-vc142-mt.dll)
    set(assimp_runtime_dll ${CMAKE_CURRENT_BINARY_DIR}/assimp-vc142-mt.dll)
    set(glslangValidator_executable ${THIRD_PARTY_DIR}/VulkanSDK/bin/Win32/glslangValidator.exe)
    set(GLFW_PLATFORM "WIN32")
    add_compile_definitions("VK_LAYER_PATH=${THIRD_PARTY_DIR}/VulkanSDK/bin/Win32")
elseif(UNIX AND NOT APPLE)
#    set(vulkan_lib ${THIRD_PARTY_DIR}/VulkanSDK/lib/Linux/libvulkan.so.1)
#    set(glslangValidator_executable ${THIRD_PARTY_DIR}/VulkanSDK/bin/Linux/glslangValidator)
    set(GLFW_PLATFORM "X11")
    add_compile_definitions("VK_LAYER_PATH=${THIRD_PARTY_DIR}/VulkanSDK/bin/Linux")
else()
    message(FATAL_ERROR "Unknown Platform")
endif()

#configure_file(glfw_config.h.in glfw_config.h @ONLY)

add_subdirectory(3rdparty)
add_subdirectory(shaders)
add_subdirectory(source)

#add_library(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES})
# 查找 include 目录下的所有 .h 文件
file(GLOB_RECURSE RENDER_INCLUDES "include/*.h")

# 去掉路径中的根目录
foreach(INCLUDE_PATH ${RENDER_INCLUDES})
    # 将根目录替换为空字符串
    string(REPLACE "${CMAKE_SOURCE_DIR}/include" "" INCLUDE_PATH ${INCLUDE_PATH})
    # 将修改后的路径添加到列表中
    list(APPEND RENDER_INCLUDES_NEW ${INCLUDE_PATH})
endforeach()

# 将新的列表赋值给 RENDER_INCLUDES
#set(RENDER_INCLUDES ${RENDER_INCLUDES_NEW})
#message(${RENDER_INCLUDES})

include_directories(include)
add_executable(${TARGET_NAME} main.cpp)
add_dependencies(${RENDER_TARGET_NAME} ${SHADER_COMPILE_TARGET})
target_link_libraries(${TARGET_NAME} PUBLIC ${RENDER_TARGET_NAME})

# 拷贝assimp dll到输出目录
if(WIN32)
    add_custom_command(
            TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${assimp_dll} ${assimp_runtime_dll}
            COMMENT "Copying assimp-vc142-mt.dll to output directory"
    )
    add_custom_target(copy_assimp_dll ALL DEPENDS ${DLL_TARGET})
endif()

