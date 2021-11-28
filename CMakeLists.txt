cmake_minimum_required(VERSION 3.5)
project(avarice_engine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# external libraries
add_subdirectory(external/glm)
add_subdirectory(external/glfw)
add_subdirectory(external/vk-bootstrap)


#library find functions
find_package(Vulkan REQUIRED)

file(GLOB SOURCES "*.cpp", "*/*.cpp", "*/*/*.cpp", "*/*/*/*.cpp") #, "*/*/*/*.cpp", "*/*/*/*/*.cpp"

add_library(${PROJECT_NAME} ${SOURCES})


target_include_directories(${PROJECT_NAME} 
    PUBLIC 
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${Vulkan_INCLUDE_DIRS} 
        ${CMAKE_DL_LIBS}
        glm 
    PRIVATE 
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

# shader compilation
file(GLOB SHADERS shaders/*.frag, shaders/*.vert)
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND mkdir ${CMAKE_BINARY_DIR}/shaders)
foreach(shader ${SHADERS})
    get_filename_component(SHADER_NAME ${shader} NAME)
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND glslc -c ${shader} -o ${CMAKE_BINARY_DIR}/${SHADER_NAME}.spv)
endforeach()
