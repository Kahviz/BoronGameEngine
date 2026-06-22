<img width="320" height="320" alt="BoronEngine Icon" src="https://github.com/user-attachments/assets/7474a87c-c063-4231-ac28-19a865d87842" />

# BoronEngine

**A game engine under development since 12.11.2025**

**Status: Work in progress** – There may be **bugs**

## Features
- Two rendering backends: **Vulkan** and **DirectX11**
- Windows support (Linux coming soon)
- Asset loading via Assimp
- Controller input with SDL3 (rumble support)
- ImGui for debugging / tools
- STB Image for icon and texture loading

## Libraries Used
- **Assimp** – 3D model loading  
- **SDL3** – Controller input / rumble  
- **ImGui** – Graphical user interface  
- **stb_image** – Image loading (icons, textures)
- **BoronMathLibrary ( BML )** -simd-Math and non simd-math
## How to Switch Renderer

1. Open `GLOBALS.h`
2. Set `DIRECTX11` to `1` to enable (or `0` to disable)
3. Set `VULKAN` to `1` to enable (or `0` to disable)

## Building (Visual Studio Community 2026)

1. Download or clone this repository
2. In VS, go to **File → Open → CMake Project**
3. Press **Run** and wait ~30 seconds while resources are cloned to `%AppData%`

## Requirements

- Windows
- Visual Studio Community 2026
- CMake
- Vulkan SDK

## Contact

Discord: `kahviz_`

Website: https://boronengine.netlify.app/

You can also join the BoronEngine Discord server through the website.

## Support the Project

**Star** this repository – it helps a lot!  
**Watch** for updates!

**Join BoronCommunity** https://discord.gg/6jkx4fgth6

Questions or ideas? Feel free to reach out.
