# Getting Started

This guide will help you build and run **BoronGameEngine**.

# Requirements

* CMake 3.10 or newer
* A C++23-compatible compiler
* Vulkan SDK (supported API versions: 1.0, 1.1, 1.3, 1.4)
* Windows

# Build and Run

## CMake & Console

1. Clone or download this repository.
2. Open a terminal in the directory containing `CMakeLists.txt`.
3. Configure the project:

```bash
cmake -S . -B build
```

4. Build the project in Release mode:

```bash
cmake --build build --config Release
```

The compiled files will be located in the `build` directory.

## Visual Studio 2026

1. Clone or download this repository.
2. Open Visual Studio 2026.
3. Go to **File → Open → CMake Project**.
4. Select the `CMakeLists.txt` file.
5. Press **Run**.

The first launch may take around 30 seconds while required resources are downloaded to `%AppData%` and the .exe is put in the ram as all programs do.
