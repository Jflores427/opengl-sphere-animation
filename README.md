# Graduate Course Project – OpenGL Sphere Animation

This project is a **Graduate Course Project** that demonstrates the use of **OpenGL** for rendering and animating a rotating textured sphere with advanced lighting, shading, and particle system features.  
The program reads a triangle mesh representation of a sphere from a text file, applies transformations, textures, and lighting effects, and renders the object in real time.

Credit: Special thanks to **Professor Yi-Jen Chiang** for guidance and resources provided for this project.

https://github.com/user-attachments/assets/e5ef2762-441a-4f4a-bc33-3ad1579e470c

---

## Functionality Summary

This project implements an advanced **OpenGL-based graphics program** featuring:

- **Sphere Rendering & Animation**
  - Loads triangle mesh data from `.txt` files.
  - Renders the sphere with **smooth or flat shading**.
  - Supports **wireframe or filled rendering**.
  - Animates the sphere along a triangular rolling path with physically accurate rotation.

- **Lighting & Shadows**
  - Supports ambient, diffuse, and specular lighting.
  - Configurable **point light** and **spotlight** sources.
  - Implements **shadow projection** on a ground plane.
  - Optionally blends shadows for softer rendering.

- **Texture Mapping**
  - Sphere textures: **1D contour lines** or **2D checkerboard pattern**.
  - Ground plane: optional **2D texture mapping**.
  - Lattice mapping toggle for experimental texture effects.

- **Fog Effects**
  - No fog, linear fog, exponential fog, or exponential-squared fog.

- **Particle System (Fireworks)**
  - Fireworks with randomized velocities and colors.
  - Time-based particle animation controlled via shaders.

- **Scene Elements**
  - **Ground plane** with optional texturing.
  - **Coordinate axes** rendering for scene orientation.

- **User Interaction**
  - **Keyboard controls** for camera movement, toggling animation, textures, fog, shading modes, and lattice effects.
  - **Mouse input** (right-click) to pause/resume animation.
  - **Context menu** for toggling shadows, lighting, fog modes, fireworks, and textures.

- **OpenGL Features Demonstrated**
  - Shaders (vertex & fragment).
  - VBOs (Vertex Buffer Objects).
  - Matrix transformations (Model, View, Projection).
  - Normal matrices for lighting.
  - Blending, fog, and texture units.

---

## Keyboard & Mouse Controls

| Input                  | Action                                                                 |
|-------------------------|------------------------------------------------------------------------|
| `b`                    | Start/stop rolling animation.                                          |
| `x`, `y`, `z`          | Rotate camera around the sphere (different axes).                      |
| `X`, `Y`, `Z`          | Rotate camera in the opposite direction.                               |
| `l`                    | Toggle **lattice mapping** effect.                                     |
| `e`                    | Toggle **eye space perspective** effect.                               |
| `o`                    | Toggle **object space perspective** effect.                            |
| `s`                    | Toggles **slant perspective** on/off.                                  |
| `v`                    | Toggles **vertical perspective** on/off.                               |
| `space`                | Reset view to default orientation.                                     |
| `q` or `Esc`           | Quit the program.                                                      |
| Right-click (mouse)    | Pause/resume animation via context menu.                               |

---

## How to Compile and Run (Microsoft Visual Studio 2019)

Assuming you already followed the **compilation and Visual Studio setup instructions** in SETUP.md, you would only need to do the following within Visual Studio:

1. **Open the Solution**
   - Open the solution file provided in the project folder.

2. **Add Files (if not already included in the Solution Explorer)**
   - Under **Source Files**, add:
     - `InitShader.cpp`
     - `rotate-sphere-texture.cpp`
   - Under **Header Files**, add:
     - `Angel-yjc.h`
     - `CheckError.h`
     - `mat-yjc-new.h`
     - `vec.h`

3. **Run the Program**
   - Build and run `rotate-sphere-texture.cpp` (with or without debugging).
   - At runtime, the program will prompt you for the name of a text file containing the triangle mesh vertices for a sphere.
   - The project includes the following mesh files:
     - `sphere.8.txt`
     - `sphere.128.txt`
     - `sphere.256.txt`
     - `sphere.1024.txt`
   - Example input at runtime:
     ```
     sphere.128.txt
     ```

4. **Visual Studio OpenGL Setup Reminder**
   - Make sure you have already set up the **Include** and **Library** directories in Visual Studio for OpenGL (as described in SETUP.md).
   - This includes linking to the appropriate OpenGL libraries.

---

## Example Usage

1. Run the program (Debug with x64 in Visual Studio).
2. Input a mesh file name (e.g., `sphere.256.txt`).
3. A window will appear displaying a rotating, textured sphere with lighting, shadows, fog, and other visual effects.

---

## Acknowledgments

This project was completed as part of a **Graduate Computer Graphics course**.  
Special credit to **Professor Yi-Jen Chiang** for providing course materials, code templates, and guidance that made this project possible.
