cmake_minimum_required(VERSION 3.14)
project(PhysicsObject VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define source files (adjust these to match your project structure)
file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp")
file(GLOB_RECURSE PROJECT_HEADERS "include/*.h")

# Add executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Find SFML
# Point CMake to the SFML installation directory
set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/SFML-3.0.0/lib/cmake/SFML")

# Uncomment the line below if you want to use static SFML libraries
# set(SFML_STATIC_LIBRARIES TRUE)

# Find SFML components (adjust components as needed for your project)
find_package(SFML 3 COMPONENTS System Window Graphics Network Audio REQUIRED)

# Link SFML libraries to your target
target_link_libraries(${PROJECT_NAME} PRIVATE
    SFML::System
    SFML::Window
    SFML::Graphics
    SFML::Network
    SFML::Audio
)

# Copy SFML DLLs to output directory (for Windows)
if(WIN32)
    # Ensure the directories exist
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>)

    # Copy the required DLLs
    if(NOT SFML_STATIC_LIBRARIES)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:SFML::System>
                $<TARGET_FILE:SFML::Window>
                $<TARGET_FILE:SFML::Graphics>
                $<TARGET_FILE:SFML::Network>
                $<TARGET_FILE:SFML::Audio>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    endif()
endif()

# Set the output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
file(COPY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${PROJECT_SOURCE_DIR}/build)