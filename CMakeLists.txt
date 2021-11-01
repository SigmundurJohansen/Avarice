cmake_minimum_required(VERSION 3.5)
project(avarice_engine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# external libraries
add_subdirectory(external/glm)
add_subdirectory(external/glfw)
add_subdirectory(external/vk-bootstrap)

#library find functions
find_package(Vulkan REQUIRED)

file(GLOB SOURCES "*cpp", "*/*.cpp", "*/*/*.cpp", "*/*/*/*.cpp") #, "*/*/*/*.cpp", "*/*/*/*/*.cpp"

add_library(${PROJECT_NAME} ${SOURCES})


target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        glm
    PRIVATE
        ${Vulkan_INCLUDE_DIRS}
        glfw
        vk-bootstrap
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        glm
    PRIVATE
        ${Vulkan_LIBRARIES}
        glfw
        vk-bootstrap
)