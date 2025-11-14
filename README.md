# Asteroid Patrol

A 3D space survival game where players pilot a spaceship through an asteroid field. The objective is to destroy incoming asteroids before they collide with the ship using laser and missile weapons.

*Developed for COSC3406 - Advanced Game Design and Development*

## Features Implemented

### Core Mechanics (100%)
- ✅ **Physics-based ship controls** with smooth acceleration and deceleration
- ✅ **Dual camera system** (first-person and third-person) with toggle switching
- ✅ **Laser weapon system** with ray-sphere collision detection
- ✅ **Missile weapon system** with sphere-sphere collision detection
- ✅ **Scene graph hierarchy** with parent-child transformations
- ✅ **Particle explosion effects** with shader-based rendering (500 particles per explosion)
- ✅ **Professional code organization** with multiple source files

### Controls
- **W/S** - Forward/Backward movement
- **A/D** - Left/Right strafe
- **Arrow Keys** - Rotate ship (pitch/yaw)
- **SPACE** - Fire laser
- **M** - Fire missile
- **C** - Toggle camera view (first-person/third-person)
- **Q** - Quit

## Project Structure

```
AsteroidPatrol_Clean/
├── include/              # Header files
│   ├── model.h          # Model data structure
│   ├── scene_node.h     # Scene graph base class
│   ├── ship.h           # Player ship class
│   ├── camera.h         # Camera system
│   ├── laser.h          # Laser weapon
│   ├── missile.h        # Missile weapon
│   ├── asteroid.h       # Asteroid with collision detection
│   ├── geometry.h       # 3D shape generation
│   ├── particle_system.h # Particle explosion effects
│   ├── game_state.h     # Game state management
│   ├── hud.h            # HUD and menu system
│   └── starfield.h      # Background starfield
├── src/                  # Implementation files
│   ├── scene_node.cpp
│   ├── ship.cpp
│   ├── camera.cpp
│   ├── laser.cpp
│   ├── missile.cpp
│   ├── asteroid.cpp
│   ├── geometry.cpp
│   ├── particle_system.cpp
│   ├── game_state.cpp
│   ├── hud.cpp
│   └── starfield.cpp
├── shaders/              # GLSL shader files
│   ├── particle_vp.glsl # Particle vertex shader
│   ├── particle_gp.glsl # Particle geometry shader
│   └── particle_fp.glsl # Particle fragment shader
├── main.cpp              # Main game loop (575 lines)
├── CMakeLists.txt        # CMake configuration
├── BUILD.bat             # Build script for Visual Studio
└── README.md             # This file
```

## Building the Project

### Requirements
- **Visual Studio 2022** (with C++ development tools)
- **OpenGL 4.0+**
- **External Libraries:**
  - GLFW 3 (window management)
  - GLEW (OpenGL extension loading)
  - GLM (mathematics library)

### Library Setup
The project requires external OpenGL libraries. You have two options:

**Option 1: Set Environment Variable (Recommended)**
```batch
# Set GAME_LIBS_PATH to your libraries folder
set GAME_LIBS_PATH=C:/path/to/your/Libraries
```

**Option 2: Edit CMakeLists.txt**
Modify line 15 in `CMakeLists.txt` to point to your local Libraries folder.

### Building

**Option 1: Using Build Script (Recommended)**
```batch
BUILD.bat
```

**Option 2: Using CMake**
```batch
mkdir build
cd build
cmake ..
cmake --build .
```

## Code Organization

The project follows a professional multi-file structure for maintainability and scalability.

### File Count:
- 12 header files (.h)
- 11 implementation files (.cpp)
- 3 shader files (.glsl)
- 1 main file (main.cpp)
- **Total: 27 source files**

### Benefits:
- Easy to navigate and maintain
- Clear separation of concerns
- Facilitates team collaboration
- Industry-standard structure

## References

This project builds upon code from:
1. **Assignment 1** - Hierarchical transformations and shape generation
2. **Assignment 2** - Camera controls, scene graph, physics-based movement
3. **Assignment 3** - Lighting and texture systems (to be integrated)
4. **CameraDemo** - Camera system reference (provided by Prof. Azami)
5. **SceneGraphDemo** - Scene hierarchy reference (provided by Prof. Azami)
6. **IlluminationDemo** - Lighting reference (provided by Prof. Azami)
7. **ParticleDemo** - Particle system and shader-based rendering (provided by Prof. Azami)

## Technical Details

### Scene Graph Architecture
The game uses a hierarchical scene graph where each object (ship, asteroids, weapons) is a node that can have children. World transformations are calculated by combining parent and child transforms.

### Collision Detection
- **Laser-Asteroid:** Ray-sphere intersection using mathematical formula
- **Missile-Asteroid:** Sphere-sphere intersection using distance comparison
- **Reference:** Real-Time Rendering textbook

### Physics System
- Smooth acceleration towards desired velocity
- Gradual deceleration when no input
- Maximum speed clamping
- Frame-rate independent updates using delta time

### Particle System
- **Shader-based rendering** using vertex, geometry, and fragment shaders
- **500 particles per explosion** distributed in spherical pattern
- **Geometry shader billboarding** converts points to camera-facing quads
- **Physics simulation** in vertex shader with gravity and velocity
- **Multiple simultaneous explosions** (up to 50 active at once)
- Adapted from Prof. Azami's ParticleDemo

## Features Completed (Beyond Core Requirements)

- ✅ HUD system (score, health display)
- ✅ Game states (menu, pause, game over)
- ✅ Particle effects for explosions
- ✅ Starfield background

## Future Enhancements (Optional)

- [ ] Sound effects
- [ ] Difficulty progression
- [ ] Power-ups
- [ ] Persistent high scores
- [ ] More weapon types

## Team Members

- Sheshank
- Prajuvin
- Balaji

## Acknowledgments

Special thanks to Professor Rosa Azami for guidance and provided demo code references (CameraDemo, SceneGraphDemo, IlluminationDemo, ParticleDemo).
