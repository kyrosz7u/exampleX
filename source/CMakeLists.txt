file(GLOB_RECURSE SOURCE_FILES "./*.cpp")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_FILES})
include_directories(../include)
add_library(${RENDER_TARGET_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${RENDER_TARGET_NAME} PUBLIC ${vulkan_lib})
target_link_libraries(${RENDER_TARGET_NAME} PUBLIC glfw)
target_link_libraries(${RENDER_TARGET_NAME} PUBLIC spdlog)
target_link_libraries(${RENDER_TARGET_NAME} PUBLIC ${assimp_lib})
target_link_libraries(${RENDER_TARGET_NAME} PUBLIC imgui)
target_link_libraries(${RENDER_TARGET_NAME} PUBLIC stb)

target_include_directories(${RENDER_TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${vulkan_include}>)
target_include_directories(${RENDER_TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${glfw_include}>)
target_include_directories(${RENDER_TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${assimp_include}>)
target_include_directories(${RENDER_TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${imgui_include}>)
target_include_directories(${RENDER_TARGET_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${RENDER_TARGET_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/shaders/generated/cpp)
