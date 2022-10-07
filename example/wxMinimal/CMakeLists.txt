# Name the project
project(wxMinimal)

file(GLOB SOURCES
    "src/*.cpp"
)

# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Define the build target for the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link required libraries to the executable
target_link_libraries(${PROJECT_NAME} PRIVATE project-lucid_shared)
