# Compilation and Installation Instructions for OpenGL on Windows

## A. Compilation Steps for Visual C++ (VC++) 20xx
(Steps are similar for various VC++ versions; example shown with **Visual Studio 2019**)

1. **Start Visual Studio 2019**
   - From the Start menu, launch *Visual Studio 2019*.  
   - From "Get started" on the right, click **Continue without code** at the bottom.

2. **Create a New Project**
   - On the top toolbar, go to **File → New → Project**.  
   - On the right, click **All languages** and select **C++**.  
   - Choose **Empty Project** → click **Next**.  
   - In the next window:
     - Set the parent folder location (e.g., `...\OpenGLSphereAnimation`) as **Location**.  
     - Set a project name (e.g., `Example`) as **Project name**.  
     - Select **Place solution and project in the same directory**.  
   - Click **Create**.  
   - A new folder (e.g., `...\OpenGLSphereAnimation\Example`) is created for the project.

   > ⚠️ If you already have an existing folder (e.g., `...\OpenGLSphereAnimation\Example`) with source files, rename it (e.g., `Example-temp`), create a new project folder, move the files back in, and delete the temporary folder.

3. **Open an Existing Project**
   - Instead of Step 2, open the folder with your existing project (e.g., `...\OpenGLSphereAnimation\Example`) and double-click the `.sln` or `.vcxproj` file.

4. **Move Files to Project Folder**
   - Move all source (`.c`, `.cpp`), header (`.h`), shader (`.glsl`), and input data files into the project folder.  

5. **Add Files to Visual Studio Project**
   - In the **Solution Explorer**, right-click **Source Files** → **Add → Existing Item** → select all `.c` / `.cpp` files.  
   - Do the same for `.h` files under **Header Files**.  
   - Shader files (`*.glsl`) only need to be in the folder (don’t add to project).  

6. **Switch to 64-bit Mode**
   - On the toolbar’s second row, locate the dropdowns: left shows `Debug`, right shows `x86` by default.  
   - Click **x86** and change to **x64**.  
   - This ensures 64-bit versions of `*.lib` and `*.dll` from FreeGLUT and GLEW are used.  

   > If using 32-bit, leave it as `x86`.

7. **Declare Library Dependencies (GLEW, FreeGLUT, OpenGL)**
   - Go to **Debug → [project name] Debug Properties**.  
   - Under **Configuration Properties**, select **All Configurations**.  
   - Navigate to **Linker → Input**.  
   - In **Additional Dependencies**, add:  
     ```
     glew32.lib; freeglut.lib; opengl32.lib; glu32.lib;
     ```
   - Click **OK**.

8. **Build the Project**
   - Go to **Build → Build [project name]** (or **Rebuild [project name]**).

9. **Run the Program**
   - Go to **Debug → Start Without Debugging** (or press `Ctrl+F5`).

---

### Additional Notes / Tips

1. **Fix for Unsafe Function Warnings (`fscanf`, `fopen`, etc.)**
   - VC++ 2019 may show warnings like:  
     ```
     'fscanf': / 'fopen': This function or variable may be unsafe.
     ```
   - To fix, define `_CRT_SECURE_NO_WARNINGS`.  
   - Steps:  
     - Go to **Debug → [project name] Debug Properties**.  
     - Under **C/C++ → Preprocessor**, edit **Preprocessor Definitions**.  
     - Add:  
       ```
       _CRT_SECURE_NO_WARNINGS;
       ```
     - Click **OK**.

2. **Console Window Disappearing After Program Stops**
   - (Fixed in VC++ 2019, but double-check.)  
   - To keep console open:  
     - Go to **Debug → [project name] Debug Properties**.  
     - Under **Linker → System**, find **Subsystem**.  
     - Set to:  
       ```
       Console (/Subsystem:Console)
       ```

---

## B. Installation on Your Own Computer

1. **Install Visual Studio Community 2019**
   - A free download is available from the course website.  
   - Save and run the installer.

2. **Install FreeGLUT and GLEW**
   - Download both packages from the course website.  
   - Extract the archives and copy the following files (using **64-bit versions**):

   - **Headers** (`.h` files):  
     - From FreeGLUT: `glut.h`, `freeglut.h`, `freeglut_ext.h`, `freeglut_std.h`  
     - From GLEW: `glew.h`, `wglew.h`  
     - Copy to:  
       ```
       C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\um\gl\
       ```

   - **Libraries** (`.lib` files):  
     - From FreeGLUT: `freeglut.lib`  
     - From GLEW: `glew32.lib`  
     - Copy to:  
       ```
       C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64\
       ```

   - **Dynamic Libraries** (`.dll` files):  
     - From FreeGLUT: `freeglut.dll`  
     - From GLEW: `glew32.dll`  
     - Copy to:  
       ```
       C:\Windows\System32\
       ```

   > ✅ Ensure `C:\Windows\System32\` is in your PATH.  
   > You can also locate the correct folders by searching for existing files like `gl.h`, `glu.h`, `opengl32.lib`, or `opengl32.dll`.

---

