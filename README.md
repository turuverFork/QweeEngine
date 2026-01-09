# QWEE Engine

A lightweight, modular 3D game engine built in C, designed for performance and simplicity. QWEE Engine provides a complete framework for creating interactive 3D experiences with real-time physics, first-person controls, and hardware-accelerated rendering.

## Why QWEE?

**Quick** - Simple API that gets you prototyping in minutes  
**Wired** - Built with efficiency and performance in mind  
**Expressive** - Clean architecture that's easy to understand  
**Extensible** - Modular design that grows with your project

## Core Architecture

```
┌─────────────────┐
│   Your Game     │
├─────────────────┤
│  Engine Core    │ ← Scene management, main loop
│  Physics        │ ← Collisions, gravity, movement
│  Objects        │ ← Meshes, materials, transforms
│  Camera         │ ← First-person controls
│  Utilities      │ ← Math, time, randomization
└─────────────────┘
```

## Quick Start Example

```c
#include "engine.h"

int main(void)
{
    // Initialize engine
    InitEngine(1280, 720, "My QWEE Game", false);
    
    // Create basic scene
    CreatePlayer("Player", 0.0f, 2.0f, 0.0f, true, true);
    CreatePlane("Ground", 0.0f, 0.0f, 0.0f, 100.0f, 100.0f, GRAY);
    
    // Add some objects
    CreateCube("RedCube", -5.0f, 1.0f, 5.0f, true, true, NULL, RED);
    CreateSphere("GreenSphere", 5.0f, 3.0f, 5.0f, true, true, GREEN, 1.0f);
    
    // Main game loop
    while (!WindowShouldClose())
    {
        float dt = GetDeltaTime();
        UpdatePhysics(dt);
        UpdatePlayerPhysics(dt);
        UpdateEngine(dt);
        RenderAll();
    }
    
    CloseEngine();
    return 0;
}
```

## Key Features

### 🎮 Complete First-Person Experience
- Smooth mouse-controlled camera
- WASD movement with physics
- Jumping and collision detection
- Configurable player parameters

### ⚡ Built-in Physics
- AABB and sphere collision detection
- Gravity and rigidbody dynamics
- Collision resolution and response
- Configurable physics settings

### 🧱 Flexible Object System
- Multiple primitive types (cube, sphere, cylinder, pyramid)
- Physics and collision toggles per object
- Texture support
- Real-time transform updates

### 🔧 Developer Friendly
- Clean, consistent API
- Comprehensive utility functions
- Debug visualization modes
- Extensible architecture

## Technical Specifications

- **Language**: C99
- **Graphics**: Raylib (OpenGL backend)
- **Platforms**: Windows, Linux, macOS
- **Max Objects**: 500 per scene
- **Target Performance**: 60 FPS @ 1280x720
- **Memory**: Lightweight (~64KB object pool)

## Building

```bash
# Basic compilation
clang -o game engine.c physics.c objects.c camera.c utils.c your_game.c \
  -I/opt/homebrew/include \
  -L/opt/homebrew/lib \
  -lraylib \
  -framework IOKit \
  -framework Cocoa \
  -framework OpenGL \
  -lm -std=c99
```

## Controls

- **WASD** - Move player
- **Mouse** - Look around
- **SPACE** - Jump
- **SHIFT** - Run
- **F1** - Toggle wireframe mode
- **R** - Throw physics ball
- **ESC** - Exit

## Project Structure

```
qwee-engine/
├── engine.h/c          # Core engine systems
├── physics.h/c         # Physics implementation
├── objects.h/c         # Object management
├── camera.h/c          # Camera controls
├── utils.h/c           # Utilities and helpers
├── vector_math.h       # Vector operations
└── example.c           # Sample implementation
```

## Requirements

- Raylib 4.0+
- C99 compatible compiler
- OpenGL 3.3+ or OpenGL ES 2.0+

## Roadmap

### v0.1 Alpha (Current)
- Basic engine architecture
- First-person camera
- Physics foundation
- Primitive object system

### v0.2 Planned
- Particle system
- Basic lighting
- Audio support
- Scene serialization

### v0.3 Planned
- GUI system
- Animation support
- Advanced materials
- Performance optimizations

## License

MIT License - see LICENSE file for details

## Contributing

This is an early alpha release. Feedback, bug reports, and suggestions are welcome as we work toward a stable 1.0 release.

---

**QWEE Engine v0.1 Alpha** - Start building your 3D world today
