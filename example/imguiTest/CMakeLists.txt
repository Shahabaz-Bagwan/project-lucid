project(imguiTest)

file(GLOB SOURCES "src/*.cpp")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} ${SOURCES})

# Link dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE project-lucid_shared)
