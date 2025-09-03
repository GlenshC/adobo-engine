# Lemon Game Editor

A C++ game editor built with Dear ImGui, supporting entity, scene, and asset management for 2D games.

## Features

- Entity creation and editing
- Scene management (create, delete, switch)
- Hitbox editing (AABB and Circle)
- Asset browser for textures and atlases
- Project save/load functionality

## Folder Structure

```
assets/         # Game assets (shaders, sprites, atlases)
build/          # Build output
example/        # Example game code
include/        # Header files
src/            # Source code (core logic in src/core/)
vendor/         # Third-party libraries (imgui, cglm, etc.)
.vscode/        # VSCode settings
```

## Getting Started

1. **Dependencies:**  
   - [Dear ImGui](vendor/imgui/)  
   - [cglm](vendor/cglm/)  
   - [GLFW](vendor/GLFW/)  
   - [glad](vendor/glad/)  
   - [ImGuiFileDialog](vendor/aiekick/ImGuiFileDialog/)

2. **Build:**  
   Use `build.bat` or your preferred CMake/Makefile setup.

3. **Run:**  
   Launch `testapp.exe` or your built executable.

## Usage

- Use the GUI to create scenes and entities.
- Assign textures and hitboxes to entities.
- Save and load projects using the asset browser.

## Code Reference

- Main editor logic: [`editor.cpp`](src/core/editor.cpp)
- Entity system: [`ecs.h`](src/core/entity/ecs.h)
- Asset management: [`binasset_stl_read.h`](src/binassets/binasset_stl_read.h)

## License

See [LICENSE](LICENSE) for details.
