
# Overview

This project is a implementation of CPU ray-tracer (without acceleration structures), for the purposes of assessing second year computer graphics students at Victoria University of Wellington. It was designed and implemented by myself (Joshua Scott) in consultation with Ben Allen.

The project is structured as the "completed solution" for the assignment. Typically we would remove parts of the solution (specified by comment flags) and the result would act as the skeleton code for the students to complete. This allows the students to work on the technical aspects of ray-tracing without having to spend the majority of their time coding a framework. The framework also provides an intuitive interface, allowing students to interactively interact with visual output of their renders, and markers to quickly assess completed selections of assignment.



# Features
 - Clear code styling and heavy commenting
 - Cross platform and portable
 - Easy compilation and execution
 - Intuitive interface
 - Smart interpolation using temporal coherence



# Setup

The project requires [CMake](https://cmake.org/) to build before compiling. The recommended way to build to project is to create a build folder then using CMake to create to project inside it. The following sections are a guides to building common project types.
```sh
$ mkdir build
```

This project also requires OpenGL v3.3 and a suitable C++17 compiler.


**Please note that `work/res/textures/checkerboard.jpg` (512x512) has been omitted for this submission.**



## Linux (Command Line)

The simplest way to set up a basic project is to run the following commands.
```sh
$ cd build
$ cmake ../work
$ make
$ cd ..
```

If the project builds without errors the executable should be located in the `build/bin/` directory and can be run with:
```sh
$ ./build/bin/a4
```


## Windows (Visual Studio)

This project requires at least Visual Studio 2015. You can get [Visual Studio Community 2020](https://www.visualstudio.com/downloads/) for free from Microsoft.

| Product |  XX  |
|:-------:|:----:|
| Visual Studio 2015 | 14 |
| Visual Studio 2017 | 15 |
| Visual Studio 2019 | 16 |

Run the `cmake` command for Visual Studio with the appropriate version number (XX).
```sh
> cmake -G "Visual Studio XX" ..\work
```

Or if you are building for 64-bit systems in Visual Studio 2015/2017.
```sh
> cmake -G "Visual Studio XX Win64" ..\work
```

After opening the solution (`.sln`) you will need to set some additional variables before running.
 - `Solution Explorer > base > [right click] > Set as StartUp Project`
 - `Solution Explorer > base > [right click] > Properties > Configuration Properties > Debugging`
 -- Select `All Configurations` from the configuration drop-down
 -- Set `Working Directory` to `$(SolutionDir)..`