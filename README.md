<div align="center">
  <h1>Nuro Engine</h1>
  <img src="https://github.com/jonkwl/nuro/blob/main/.gthub-media/nuro_banner.jpg" alt="Nuro Banner" width=800>
  <p></p>
  <p><b>A lightweight, bloat-free 3D game engine for fast development and rapid iteration</b></p>
  <p><a href="#contributing">💜 Contributing</a>  |  <a href="#about">💁‍♂️ About Nuro</a>  |  <a href="#building">🔨 Building</a>  |  <a href="#showcase">📷 Showcase</a>  |  <a href="#features">📃 Features</a></p>
  <p></p>
</div>

---

## Disclaimer: Work In Progress! 🚧

Hey! This project is currently **under development.**  
Please take a look at the vision below! 💜

⭐ Feel free to **star the project**, it motivates a lot :)

## Contributing

💁‍♂️ Please join the **Discord-Server** for **contributing, asking for help, reporting bugs and submitting suggestions**:  
📎 https://discord.gg/hHh3m6Vncz

## About
⚡ **Nuro** is a **3D game engine** written in C++ designed for **rapid iteration** and a **seamless development** workflow. It’s supposed to help developers focus on what matters most, with minimal setup and **no interruptions**.

🚀 It's an **intuitive and easy-to-understand** tool for **rapid game prototyping** and quickly building small games, perfect for game jams. Its **synchronous architecture** ensures **zero delays** between developing and testing your game!

❌ It's is ***not*** another high-performance AAA-Engine, there's plenty of awesome options for that! Instead the goal is to deliver a smooth, efficient, and intuitive development experience tailored for smaller projects.

This project is still in an early state and **not ready to use yet**, however please feel encouraged to **help realize the vision** by [contributing](#contributing). To get an overview of the current state of the project, **check out the [showcase](#showcase)**. 📷

<img src="https://github.com/jonkwl/nuro/blob/main/.gthub-media/nuro-example-1.jpg" alt="Screenshot of an example scene in the nuro engine editor" width=1000>

> *Screenshot of an example scene within the Nuro Editor*

## Building

Nuro uses **CMake** and **vcpkg** for building and managing dependencies.

```
# Create a build directory and navigate into it
mkdir build && cd build

# Build the project using CMake and specify the vcpkg toolchain file
cmake .. -DCMAKE_TOOLCHAIN_FILE=[path_to_vcpkg]/scripts/buildsystems/vcpkg.cmake

# Optionally, you can build the project after configuring it
cmake --build .
```

> \[!IMPORTANT]
>
> CMake 4.x pre-release may cause dependency build failures. We recommend using **CMake 3.x stable**.

## Showcase

### More media will be linked here soon!

## Features

- 🖥️ **Intuitive Editor**
  - **Instant game testing** without compiling or resource loading.
  - Multiple **workspaces** tailored for their use cases (Sketch):
    - 📁 *Project Workspace*
      - Manage project settings and resources.
    - ⭕ *General Workspace*
      - All-purpose development, entity and world inspectation etc.
    - 🏭 *Level Building Workspace*
      - Support for fast level prototyping.
    - 🖱️ *Scripting Workspace*
      - Comprehensive overview for the games logic.
    - 🔦 *Lighting Workspace*
      - Fine tuning of the scenes lighting settings.
    - 🔊 *Audio Workspace*
      - Audio mixing and audio source overview.
    - 🖋 *UI Editor Workspace*
      - Reserved for building the ingame user interface.
    - ✨ *Post Processing Workspace*
      - Finetune post processing volumes and color correction.
- 💡 **Physically-Based 3D Renderer**
  - Supports various PBR material effects: Normal-, Height-, Emissive Mapping, and more.
  - Supports both hard shadows and soft shadows.
  - Includes a variety of built-in **post-processing** effects:
    - Screen-Space-Ambient-Occlusion
    - Chromatic Aberration
    - Motion Blur
    - Bloom
    - etc.
  - Utilizing a modern OpenGL 4.x backend to ensure **cross-platform** capabilities.
- 🔄 **Entity-Component-System**
  - The engine's base components use a data-driven entity-component-system to ensure efficiency.
- ⚙️ **Physics using NVIDIA's PhysX**
  - The physics layer ensures support for realistic and performant 3D simulations.

### ...and much more already!
