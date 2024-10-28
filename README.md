# SPH Fluid Simulation

This project is a Smoothed Particle Hydrodynamics (SPH) fluid simulation implemented in C++. It models fluid dynamics by simulating particles and their interactions.

## Prerequisites

- **Visual Studio Code** installed on Windows
- **CMake** installed and added to your system PATH
- A C++ compiler (e.g., MinGW-w64 or Microsoft Visual C++ Build Tools)

## Build Instructions (Windows with VSCode)

Follow these steps to build and run the project using Visual Studio Code on Windows.

### 1. Set Up CMake Configuration and Compiler

Ensure that CMake and your preferred C++ compiler are properly installed. You may need to install the CMake and C/C++ extensions in VSCode if you haven't already.

### 2. Generate Build Files

Open a terminal in the root directory of the project and run:

```bash
cmake -Bbuild . -DCMAKE_BUILD_TYPE=Debug
```

- `-Bbuild` specifies the build directory.
- `.` indicates the source directory is the current directory.
- `-DCMAKE_BUILD_TYPE=Debug` sets the build type to Debug.
the dependencies listed in Dependency.cmake will be downloaded.

### 3. Run the Simulation

Press `Ctrl + F5` in Visual Studio Code to run the simulation without debugging.
to start the simulation press c
to reset press r
right-click mouse to move around with wasd
