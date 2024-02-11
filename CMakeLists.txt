cmake_minimum_required(VERSION 3.22)
project(Brain)
set(CMAKE_CXX_STANDARD 23)
add_executable(Brain)
# Run as GUI application
if (WIN32)
    target_link_options(Brain PRIVATE -mwindows)
endif (WIN32)

# ------------
# Set up tools
# ------------

# GLAD
# Add glad.c to use GLAD functions.
target_sources(Brain PRIVATE include/glad.c)
# include `./include/` subdirectory for GLAD headers.
target_include_directories(Brain PRIVATE include)

# GLFW
# We only care about using GLFW, no docs, tests or examples.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# Add GLFW subdirectory. We build glfw from source and link.
add_subdirectory(glfw-3.3.9)
# link GLFW library.
target_link_libraries(Brain glfw)

# Copy Shaders to build directory
file(GLOB SHADERS
        "src/*.vsh"
        "src/*.fsh"
        "src/*.gsh"
)
foreach (SHADER ${SHADERS})
    if (WIN32)
        # configure_file(${SHADER} "test")
        add_custom_command(TARGET Brain PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} ${CMAKE_CURRENT_BINARY_DIR})
    elseif (UNIX AND NOT APPLE)
        file(COPY ${SHADER} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    elseif (APPLE)
        # create symbolic link for *.vs *.fs *.gs
        get_filename_component(SHADERNAME ${SHADER} NAME)
        makeLink(${SHADER} ${CMAKE_CURRENT_BINARY_DIR}/${SHADERNAME} ${NAME})
    endif (WIN32)
endforeach (SHADER)

# ----------------
# Set up our stuff
# ----------------

# Add new files like this: target_sources(Brain PRIVATE main.cpp file1.cpp file2.cpp)
target_sources(Brain PRIVATE src/main.cpp)
