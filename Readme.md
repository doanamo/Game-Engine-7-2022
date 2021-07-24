Game Engine
===

Work in progress. Project for a reusable game engine that is currently undergoing heavy research and development process. It's planned to be eventually used in a future game of mine.

Repository
===

* *Build/* - Build configuration files
* *Deploy/* - Files needed to run compiled binaries
* *Example/* - Example project built using this library
* *External/* - Dependencies such as third party libraries
* *Include/* - Include files for engine library
* *Source/* - Source files for engine library
* *Tests/* - Code unit tests and their resources
* *Tools/* - Miscellaneous tools used by engine

Requirements
===
* Windows or Linux (MacOS should work, but not tested)
* Visual Studio 2019, GCC 9 or Clang 9 (or newer)
* CMake (see CMakeLists.txt for specified minimum version)
* All other build dependencies are included

Building
===
Using build scripts (see *Build* directory):
* For Visual Studio on Windows, there are batch scripts that will generate/update solution via CMake and then automatically open IDE
* For Emscripten on Windows, there is batch script that will download Emscripten SDK along with needed build tools, compile the entire engine and then run Example project in browser

For native desktop (manual):
* Run CMake and input main directory to generate makefiles or IDE solution of your choice
* Compile and run Example project

For web browser (manual):
* [Setup Emscripten SDK](https://webassembly.org/getting-started/developers-guide/)
* Run CMake to generate makefiles or IDE solution for cross compilation with following toolchain file from Emscripten SDK: `emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake`
* Compile Example project and then proceed to the output directory (with produced WASM and HTML files)
* Run either `emrun --no_browser --port 8080 .` or `python -m SimpleHTTPServer 8080` and then open `http://localhost:8080`

Contributions
===
Engine is not intended nor anywhere close near being suitable for use yet. For now please refrain for sending pull requests for new features. However, feel free to open issues for dicussion or any questions about this project and its code!

License
===
MIT License

Copyright (c) 2018-2021 Piotr Doan. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
