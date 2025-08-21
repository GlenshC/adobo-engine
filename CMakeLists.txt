cmake_minimum_required(VERSION 3.14)
project(testapp C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ===============================
# 🛠️ Build Options
# ===============================
option(BUILD_CLI "Build as CLI (console) app" ON)

# ===============================
# 🧩 GLFW: Use prebuilt if available, else fetch
# ===============================
set(GLFW_ROOT_DIR "${CMAKE_SOURCE_DIR}/vendor/GLFW")
set(GLFW_INCLUDE_DIR "${GLFW_ROOT_DIR}/include")
set(GLFW_LIB_DIR "")

if (MSVC OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND WIN32 AND NOT MINGW))
    set(GLFW_LIB_DIR "${GLFW_ROOT_DIR}/lib-vc2022")
else()
    set(GLFW_LIB_DIR "${GLFW_ROOT_DIR}/lib-mingw-w64")
endif()

find_library(GLFW_LIB glfw3 HINTS "${GLFW_LIB_DIR}")

if (GLFW_LIB AND EXISTS "${GLFW_INCLUDE_DIR}/GLFW/glfw3.h")
    message(STATUS "Using local prebuilt GLFW: ${GLFW_LIB}")
    add_library(glfw STATIC IMPORTED)
    set_target_properties(glfw PROPERTIES
        IMPORTED_LOCATION "${GLFW_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${GLFW_INCLUDE_DIR}"
    )
else()
    message(STATUS "Prebuilt GLFW not found — using FetchContent")
    include(FetchContent)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.3.8
    )
    FetchContent_MakeAvailable(glfw)
endif()

# ===============================
# Find OpenGL
# ===============================
find_package(OpenGL REQUIRED)

# ===============================
# ✨ Compiler flags
# ===============================
add_compile_options(-fmacro-prefix-map=${CMAKE_SOURCE_DIR}/=)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    endif()
    add_compile_options(-Wall -Wextra -Werror )
    set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W4 /WX)
    set(CMAKE_CXX_FLAGS_DEBUG   "/Od /Zi")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
endif()


# ===============================
# 🔧 Global macros (if needed)
# ===============================
add_compile_definitions(ADOBO_GAME_ENGINE)

# ===============================
# 🧱 Libraries
# ===============================
add_library(glad STATIC vendor/glad/src/gl.c)
set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)
target_include_directories(glad PUBLIC vendor/glad/include)

add_subdirectory(vendor/cglm/ EXCLUDE_FROM_ALL)
# add_subdirectory(vendor/binassets)

# ===============================
# 🚀 Executable
# ===============================
add_executable(${PROJECT_NAME}
    src/main.cpp
    
    src/game/game.cpp
    src/game/gui.cpp
    
    src/core/entity/ecs.cpp
    src/core/app.cpp
    src/core/platform.cpp
    src/core/input.cpp
    src/core/clock.cpp

    src/core/math/collisions.cpp

    src/renderer/sprite2D.cpp
    src/renderer/renderer.cpp

    src/renderer/texture.cpp
    src/renderer/shader.cpp
    
    src/util/mem_arena.cpp
    src/util/string.cpp
    src/util/debug_state.cpp

    vendor/binassets/src/binasset_read.cpp
    vendor/binassets/src/binasset_json.cpp
)
    
    
#imgui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/vendor/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${IMGUI_DIR})
set(IMGUI_BACKEND_SOURCES
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
target_sources(imgui PRIVATE ${IMGUI_BACKEND_SOURCES})
target_include_directories(imgui PUBLIC ${IMGUI_DIR}/backends)
target_link_libraries(imgui PUBLIC glfw)

target_compile_definitions(${PROJECT_NAME}
  PRIVATE
    $<$<CONFIG:Debug>:DEBUG_ENABLED>
)

if (NOT BUILD_CLI)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    src
    include
    vendor/imgui
    vendor/imgui/backends
    vendor/glad/include
    vendor/stb_image/include
    vendor/cglm/include
    vendor/binassets/include
)

target_link_libraries(${PROJECT_NAME}
    cglm
    glad
    glfw
    imgui
    OpenGL::GL
)

