# min. version of cmake required for this project
cmake_minimum_required(VERSION 3.10)

# name of the project
project(project-lucid)

# Required to use vim as cpp ide
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui)

# collect all the .cxx needed for executable
file(
  GLOB
  SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)

# add_subdirectory(thirdparty/opencv)
add_subdirectory(example)

find_package(OpenCV REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Create library
add_library(${PROJECT_NAME}_shared SHARED ${SOURCES})

target_link_libraries(${PROJECT_NAME}_shared PUBLIC ${OpenCV_LIBS} glfw
                                                    ${OPENGL_LIBRARIES})

target_include_directories(
  ${PROJECT_NAME}_shared
  PUBLIC ${OpenCV_INCLUDE_DIRS}
         ${OPENGL_INCLUDE_DIRS}
         ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
         ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui
         ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/backends
         ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(${PROJECT_NAME}_static STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME}_static PUBLIC ${OpenCV_LIBS} glfw
                                                    ${OPENGL_LIBRARIES})

target_include_directories(
  ${PROJECT_NAME}_static
  PUBLIC ${OpenCV_INCLUDE_DIRS}
         ${OPENGL_INCLUDE_DIRS}
         ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
         ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui
         ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/backends
         ${CMAKE_CURRENT_SOURCE_DIR}/include)
