# Adobo Game Engine

## Overview

Lemon Game Engine is a modular 2D/3D engine written in C, designed for rapid development and prototyping. It uses OpenGL for rendering, GLFW for window/input management, and GLAD for OpenGL loading. The engine is organized for flexibility, making it easy to extend or integrate into your own projects.

## Features

- Modular architecture for 2D and 3D games
- Entity Component System (ECS)
- Scene and asset management
- OpenGL-based renderer (sprite batching, shaders, textures)
- Built-in editor with ImGui integration
- Collision detection (AABB, Circle)
- Cross-platform support (Windows, Linux, macOS)
- CMake-based build system

## Getting Started

1. **Clone the repository:**
   ```bash
   git clone https://github.com/yourusername/lemon-game.git
   cd lemon-game
   ```

2. **Install dependencies:**
   - [GLFW](https://www.glfw.org/)
   - [GLAD](https://glad.dav1d.de/)
   - [CMake](https://cmake.org/)
   - [cglm](https://github.com/recp/cglm)
   - [Dear ImGui](https://github.com/ocornut/imgui)

   *(Most dependencies are included in the `vendor/` folder.)*

3. **Explore the source code:**
   - Main entry: `src/main.cpp`
   - Core engine: `src/core/`
   - Renderer: `src/renderer/`
   - Utilities: `src/util/`
   - Game logic: `src/game/`

## Build Instructions

### Using CMake

**Windows (MSVC):**
```bash
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

**Linux/macOS (GCC/Clang):**
```bash
cmake -S . -B build
cmake --build build --config Release
```

**MinGW/Clang (Windows):**
```bash
make release
```

### Run the Engine

After building, run the executable from the `build/` directory:
```bash
./testapp
```

## Roadmap

- Refactor asset loading and memory management
- Add runtime loader and hot-reloading
- Expand physics and collision systems
- Improve editor features and usability
- Add support for more file formats (JSON, XML)
- Enhance documentation and code samples

---

*For issues, suggestions, or contributions, please open a pull request or issue on GitHub.*
