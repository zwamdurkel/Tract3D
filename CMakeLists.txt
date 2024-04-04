cmake_minimum_required(VERSION 3.22)
set(NAME main)
project(${NAME} VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 23)
add_executable(${NAME})
# Run as GUI application
if (WIN32)
    target_link_options(${NAME} PRIVATE -mwindows -static -Wl,-allow-multiple-definition)
endif (WIN32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# ----------------
# Set up our stuff
# ----------------

set(ICON data/main.rc)
# Add new files like this: target_sources(${NAME} PRIVATE main.cpp file1.cpp file2.cpp)
target_sources(${NAME} PRIVATE ${ICON} src/main.cpp src/GLFWWrapper.cpp src/ImGuiWrapper.cpp src/path.cpp src/TractDataWrapper.cpp src/RayTraceWrapper.cpp src/AbstractWrapper.cpp src/Bvh.cpp)

# Copy Shaders to build directory
file(GLOB SHADERS
        "src/*.vsh"
        "src/*.fsh"
        "src/*.geom"
        "src/*.comp"
)
foreach (SHADER ${SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    configure_file(${SHADER} ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME} COPYONLY)
endforeach (SHADER)
configure_file("data/Ruda-Regular.ttf" "${CMAKE_CURRENT_BINARY_DIR}/Ruda-Regular.ttf" COPYONLY)
configure_file("data/fa-solid-900.ttf" "${CMAKE_CURRENT_BINARY_DIR}/fa-solid-900.ttf" COPYONLY)
file(GLOB INPUTS
        "data/tck files/*.tck"
)
foreach (INPUT ${INPUTS})
    get_filename_component(INPUT_NAME ${INPUT} NAME)
    configure_file(${INPUT} ${CMAKE_CURRENT_BINARY_DIR}/examples/${INPUT_NAME} COPYONLY)
endforeach (INPUT)

# ------------
# Set up tools
# ------------

# GLAD
# Add glad.c to use GLAD functions.
target_sources(${NAME} PRIVATE external/glad/glad.c)
# Include `./external/` subdirectory for GLAD headers.
target_include_directories(${NAME} PRIVATE external)

# GLFW
# We only care about using GLFW, no docs, tests or examples.
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
if (WIN32)
    add_custom_command(TARGET ${NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${NAME}> $<TARGET_FILE_DIR:${NAME}>
            COMMAND_EXPAND_LISTS
    )
endif (WIN32)

# Add GLFW subdirectory. We build glfw from source and link.
add_subdirectory(external/glfw-3.3.9)
# Link GLFW library.
target_link_libraries(${NAME} PRIVATE glfw)

# ImGui
file(GLOB IMGUI
        "external/imgui-1.90.3/*.cpp"
        "external/imgui-1.90.3/backends/*.cpp"
        "external/imgui-1.90.3/misc/cpp/*.cpp"
)
target_include_directories(${NAME} PUBLIC external/imgui-1.90.3 external/imgui-1.90.3/backends)
target_sources(${NAME} PRIVATE ${IMGUI})

# Native-File-Dialog
add_subdirectory(external/nativefiledialog)
target_link_libraries(${NAME} PRIVATE nfd)

# ----------------
# Installer Config
# ----------------

# Set files to include
install(TARGETS ${NAME} DESTINATION bin)
install(FILES ${SHADERS} DESTINATION bin)
install(FILES $<TARGET_RUNTIME_DLLS:${NAME}> DESTINATION bin)
install(FILES "data/Ruda-Regular.ttf" DESTINATION bin)
install(FILES "data/fa-solid-900.ttf" DESTINATION bin)
install(FILES ${INPUTS} DESTINATION bin/examples)

# Settings
set(FULL_NAME "Tract 3D")
include(InstallRequiredSystemLibraries)
set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_NAME ${FULL_NAME})
set(CPACK_PACKAGE_VENDOR "Willem Ploegstra, Finnean van den Boorn, Aloys Riswick")
set(CPACK_PACKAGE_DESCRIPTION "Tract-based 3D animation tool")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${FULL_NAME})
set(CPACK_PACKAGE_EXECUTABLES ${NAME} ${FULL_NAME})
set(CPACK_CREATE_DESKTOP_LINKS ${NAME})

# NSIS specific
set(CPACK_NSIS_CREATE_ICONS_EXTRA "
    CreateShortCut \\\"$DESKTOP\\\\${FULL_NAME}.lnk\\\" \\\"$INSTDIR\\\\bin\\\\${NAME}.exe\\\"
")
set(CPACK_NSIS_DELETE_ICONS_EXTRA "
    Delete \\\"$DESKTOP\\\\${FULL_NAME}.lnk\\\"
")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MUI_HEADERIMAGE "${CMAKE_SOURCE_DIR}\\\\data\\\\banner.bmp")
set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP "${CMAKE_SOURCE_DIR}\\\\data\\\\side_banner.bmp")
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}\\\\data\\\\icon.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}\\\\data\\\\icon.ico")
set(CPACK_NSIS_MUI_FINISHPAGE_RUN "${NAME}.exe")
set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\${NAME}.exe")
set(CPACK_NSIS_IGNORE_LICENSE_PAGE ON)
include(CPack)
