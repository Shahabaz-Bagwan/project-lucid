# Use the variable PROJECT_NAME for changing the target name
get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
set(PROJECT_NAME ${ProjectId})

cmake_minimum_required(VERSION 3.1)
project(${PROJECT_NAME})

file(GLOB SOURCES "src/*.cpp")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME}
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE project-lucid_shared)
