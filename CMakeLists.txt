cmake_minimum_required(VERSION 3.5)
project(AvariceEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# external libraries
add_subdirectory(external/glm)
add_subdirectory(external/vk-bootstrap)
add_subdirectory(external/glfw)
add_subdirectory(external/SDL)
add_subdirectory(external/VulkanMemoryAllocator)

# SDL
set( SDL_STATIC ON CACHE BOOL "" FORCE )
set( SDL_SHARED OFF CACHE BOOL "" FORCE )

#library find functions
find_package(Vulkan REQUIRED)

file(GLOB SOURCES "src/*.cpp", "src/*/*.cpp", "src/rendering/*/*.cpp", "src/rendering/vulkan/*.cpp", "src/rendering/vulkan/*.h",
                    "core/*.h", "src/core/*h"
                    "*.cpp", "*/*.cpp", "*/*/*.cpp")

add_library(${PROJECT_NAME} ${SOURCES} 
            rendering/
            platform/
            core/
            src/rendering/
            src/rendering/SDL/
            src/utils/
            src/
            external/glm/glm/glm.hpp
            )

target_include_directories(${PROJECT_NAME} 
    PUBLIC 
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${CMAKE_DL_LIBS}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/rendering
        ${CMAKE_CURRENT_SOURCE_DIR}/core
        ${Vulkan_INCLUDE_DIRS}
        glm
    PRIVATE
        glfw 
        vk-bootstrap 
        VulkanMemoryAllocator
        external/stb
)

target_link_libraries(${PROJECT_NAME} 
    PUBLIC 
        SDL2main
        SDL2-static
        ${Vulkan_LIBRARIES} 
        glfw
        glm 
    PRIVATE 
        vk-bootstrap 
        VulkanMemoryAllocator
) 

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND rmdir /S /Q shaders)
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND mkdir shaders)

file (GLOB SHADERS shaders/*.frag shaders/*.vert)

foreach(shader ${SHADERS})
    get_filename_component(SHADER_NAME ${shader} NAME)

    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND glslc -c ${shader} -o shaders/${SHADER_NAME}.spv)
endforeach()


