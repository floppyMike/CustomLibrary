# How to install
## Visual Studio
This library works best if you use it on a C++17 blank project.

### Install the library.
For the library itself.
```
git clone https://github.com/floppyMike/CustomLibrary.git
```
The library requires the SDL2 library which can be found here: https://www.libsdl.org/download-2.0.php
Optionaly for additional functionality you should consider these following libraries:
1. [Image support](https://www.libsdl.org/projects/SDL_image/)
2. [Text support](https://www.libsdl.org/projects/SDL_ttf/)
3. [Music support](https://www.libsdl.org/projects/SDL_mixer/)

### Adding the Includes.
In your project go to "Project/Properties/C++/General/Additional Include Directories" and add the include paths of this library, the SDL2 library and optionally the additional libraries.

### Adding the .lib files.
Go to "Project/Properties/Linker/General/Additional Library Directories" and add the .lib paths of the SDL2 libary and the optional libraries.

Later go to "Project/Properties/Linker/Input/Additional Dependencies" and list the file names of the .lib files.

### Including the .dll files.
Copy all the .dll files from the SDL2 library and the optional libraries into the project directory.

---

**If you're having difficulties with the installation proccess look at the example [Test](https://github.com/floppyMike/CustomLibrary/tree/master/Test).**
