# generate IDE projects with cmake

## usage
1. create build directory as your normally would, except location of build directory must be **sibling** of source project directory (for IDEs based on Eclipse CDT):
	* ``/home/user/proj-src/some-cmake-project``
	* ``/home/user/proj-src/build-dir-for-that-project``
2. run ``query-cmake`` 
3. run cmake as you normally would
4. run ``<ide>-parse-reply``
5. from your IDE, import project from this build directory.

## whats going on here
cmake provides [cmake-file-api](https://cmake.org/cmake/help/latest/manual/cmake-file-api.7.html), in which cmake generates json, which then can be read to generate IDE project files.

First ``query-cmake`` creates two files in build directory, which tells cmake to generate json.  Then, after cmake is run, ``<ide>-parse-reply`` reads this json and outputs project files for that IDE.

# CMAKE project requirements
Project must have preprocessor definition of ``BOARD``specifying the hardware used.  The correct MCU part number is then found from this board name.   See [translation of board names](#trans)

# Why use IDE?
These IDEs package together compiler and debugger working together in single development environment, relieving the developer from having to individually install each of these, and then set them up.  Additionally, some IDEs are supported directly by microcontroller vendor.

## Why CMAKE for embedded projects?
Too many combinations of hardware variations: target MCU, board type, add-on board type, software options.  Results in infinite number of projects.  Instead, just let cmake create the build system with desired configuration.

# Supported IDEs:
Eclipse CDT based with gcc
* [System Workbench for STM32](https://www.openstm32.org/HomePage) aka Ac6 SW4STM32
* [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html)

Projects would create directly if source cmake project uses gcc, and IDE also uses gcc.

# Potential IDEs to support:
* [Eclipse Embedded CDT](https://projects.eclipse.org/projects/iot.embed-cdt/downloads)
* Texas Instruments [Code Composer Studio (CCS)](https://www.ti.com/tool/CCSTUDIO)
* NXP [MCUXpresso](https://www.nxp.com/design/software/development-software/mcuxpresso-software-and-tools-/mcuxpresso-integrated-development-environment-ide:MCUXpresso-IDE)
*  Renesas [e2 studio](https://www.renesas.com/tw/en/products/synergy/software/tools/e2-studio.html)

* SiLabs [Simplicity Studio](https://www.silabs.com/products/development-tools/software/simplicity-studio)
* more?  Cypress, Maxim, Nordic, Toshiba, etc.

non-eclipse based:
* ARM [Keil µVision](http://www2.keil.com/mdk5/uvision/)
* IAR [Embedded Workbench](https://www.iar.com/iar-embedded-workbench/)
* Microchip [MPLAB X IDE](https://www.microchip.com/mplab/mplab-x-ide)
* more?


# compiling from source
## building on Linux
on distributions using apt package management:
``sudo apt install libxml2-dev libjson-c-dev``
or equivalent packages for your Linux distribution
*`` mkdir build``
*`` cd build``
*`` cmake ..``
*`` make``

## building on Windows
tested using http://mingw-w64.org/doku.php/download/win-builds
because it includes json-c and libxml2 libraries already. 

ensure winbuilds bin directory is in your PATH, in this example c:/winbuilds

* ``mkdir build``
* ``cd build``
* ``cmake -G "MinGW Makefiles" .. -DLIBXML2_LIBRARY=c:/winbuilds/lib64/libxml2.dll.a``
* ``gmake``
## Translation of board and MCU names
(#trans)
IDEs from MCU vendors might require MCU names to be defined (spelled) correctly in order to function properly.  Some cmake projects may not define ``BOARD``with spelling correct, or MCU part number is not defined precisely ether.


Internal board table is provided for some boards, but other hardware will always exist, and new board could come out tomorrow.  When the file `` translate_board_mcu.json`` is present in the build directory when running this generator, then this json board table will override the internal table.   


