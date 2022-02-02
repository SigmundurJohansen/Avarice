cmake_minimum_required(VERSION 3.5)
project(AvariceEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# external libraries
add_subdirectory(external/glm)
add_subdirectory(external/glfw)
add_subdirectory(external/vk-bootstrap)


#library find functions
find_package(Vulkan REQUIRED)

file(GLOB SOURCES "src/*.cpp", "src/*/*.cpp", "src/*/*/*.cpp", "src/*/*/*/*.cpp",
                    "*.cpp", "*/*.cpp", "*/*/*.cpp", "*/*/*/*.cpp")


add_library(${PROJECT_NAME} ${SOURCES} 
            src/rendering/vulkan/vulkan_pipeline_builder.cpp src/rendering/vulkan/vulkan_pipeline_builder.h 
            src/rendering/vulkan/vulkan_utilities.h src/rendering/vulkan/vulkan_utilities.cpp
            src/rendering/vulkan/vulkan_renderer.h src/rendering/vulkan/vulkan_renderer.cpp)

target_include_directories(${PROJECT_NAME} 
    PUBLIC 
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/rendering
        ${CMAKE_CURRENT_SOURCE_DIR}/inputs
        ${CMAKE_DL_LIBS}
        glm
    PRIVATE
        ${Vulkan_INCLUDE_DIRS}
        glfw 
        vk-bootstrap 
)

target_link_libraries(${PROJECT_NAME} 
    PUBLIC 
        ${Vulkan_LIBRARIES} 
        vk-bootstrap 
        glm 
    PRIVATE 
        glfw 
) 
       
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND rmdir /S /Q shaders)
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND mkdir shaders)

file (GLOB SHADERS shaders/*.frag shaders/*.vert)

foreach(shader ${SHADERS})
    get_filename_component(SHADER_NAME ${shader} NAME)

    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND glslc -c ${shader} -o shaders/${SHADER_NAME}.spv)
endforeach()

