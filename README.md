# Fractal Engine
Fractal Engine is a new and upcoming game engine developed by me! (Blixer12)
although not feature rich as of now, i hope i will be able to use it for a game
i am planning to make that will function like Rec Room!

"Fractal" is a mathematical term that i chose since i always thought
they made some of the coolest patterns, like the mandelbrot set and serpinskis triangle!

# Features
These are some features currently in the project as of now
I am planning to add more, and list a roadmap of it!

## Platforms
Currently Windows and Linux as of now
I plan on supporting MacOS, IOS, and Android
as well as some game consoles like Xbox and what not

> Note: if I do end up supporting consoles,
> I do need to see if you are a part of that
> specific developer program

## Graphics APIs
On a technical note, the only Graphics API
Supported as of now is Vulkan. Plans to add
DirectX12 and 11, OpenGL and possibly Metal
as Viable Graphics APIs hopefully will happen


# Start

## Building the project

### Dependencies
These are some dependencies needed for the project to build and/or run
* The Vulkan SDK: https://vulkan.lunarg.com/
* Clang: https://releases.llvm.org/download.html (For Windows, it is specifically for clang-cl because of c23)
* Make (Optional): https://gnuwin32.sourceforge.net/packages/make.htm

### Building

This is by no means a detailed guide on how to build the engine itself, just a simplification
However it should work for most ang get it hopefully working for anyone
1. Make sure you got the dependencies above
2. Clone the repo [https://github.com/Blixer12/FractalEngine.git](https://github.com/Blixer12/FractalEngine.git)
3. After the cloning, run `BuildAll.bat` if on Windows, Linux is `BuildAll.sh` if Make was installed; if not, run the (maybe deprecated) Build.bat or Build.sh in the subfolders

## Roadmap
Hopefully these features will be in place
* A good feeling 3D game engine that can be used
* Cross-Platform support
* Multiple Graphics APIs
* Fully functioning editor
* Hot reloading for code
* Physics (Obviously)
* Sound (2D and 3D)
* UI System
* Events
* Standard input

## Goal
I want to be able to make games, and more than games
With this engine, i might even do other cool things!

## Other projects
I Do want to make an Operating System
although probably will not be focused on for
the forseeable future, but let me know if anyone wats to see that,
and I mean from scratch, the bootloader all the way to a fuctioning Operating System!
but I dont think i will make a tutorial on it,
because i would need to learn a lot on the UEFI and how kernels like
linux function. 

## Links
- Youtube Channel: [@Blixer12](https://youtube.com/@Blixer12)
- Discord: (Not available yet)
- Website: (Not available yet)

# Credits

> **Note:** Although this project is very similar to the Kohi codebase right now, I am actively building on top of it to make Fractal Engine my own! 
> The Kohi %Game Engine series is a fantastic resource if you want to learn how to build an engine from scratch. Like how i am!

Check out the [Kohi %Game Engine Series on YouTube](https://www.youtube.com/playlist?list=PLv8Ddw9K0JPg1BEO-RS-0MYs423cvLVtj) by Travis Vroman, and don't be afraid to ask me for help, because hopefully i will be the master at codding a game engine!