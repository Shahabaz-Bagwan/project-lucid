# Name the project
project(wxImage)

# Define a variable containing a list of source files for the project
file(GLOB SOURCES
    "src/*.cpp"
)

# Define the build target for the executable
add_executable(${PROJECT_NAME} ${SOURCES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Link required libraries to the executable
target_link_libraries(${PROJECT_NAME} PRIVATE project-lucid_shared)
