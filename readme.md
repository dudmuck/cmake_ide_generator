# generate IDE projects with cmake

## usage
1. create build directory as you normally would
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
* Renesas [e2 studio](https://www.renesas.com/us/en/products/software-tools/tools/ide/e2studio.html). only [RA family](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/ra.html) at this time.
* Texas Instruments [Code Composer Studio (CCS)](https://www.ti.com/tool/CCSTUDIO)  only [msp432 family](https://www.ti.com/wireless-connectivity/overview.html) at this time.

Projects would create directly if source cmake project uses gcc, and IDE also uses gcc.

# Potential IDEs to support:
* [Eclipse Embedded CDT](https://projects.eclipse.org/projects/iot.embed-cdt/downloads)

* NXP [MCUXpresso](https://www.nxp.com/design/software/development-software/mcuxpresso-software-and-tools-/mcuxpresso-integrated-development-environment-ide:MCUXpresso-IDE)
* Nuvoton [NuEclipse](https://www.nuvoton.com/tool-and-software/software-development-tool/driver)
* SiLabs [Simplicity Studio](https://www.silabs.com/products/development-tools/software/simplicity-studio)
* more?  Cypress, Maxim, Nordic, Toshiba, etc.

non-eclipse based:
* ARM [Keil µVision](http://www2.keil.com/mdk5/uvision/)
* IAR [Embedded Workbench](https://www.iar.com/iar-embedded-workbench/)
* Microchip [MPLAB X IDE](https://www.microchip.com/mplab/mplab-x-ide) netbeans based
* [Segger Embedded Studio](https://www.segger.com/products/development-tools/embedded-studio) (only for Nordic?)
* more?


# compiling from source
## building on Linux
on distributions using apt package management:
``sudo apt install libxml2-dev libjson-c-dev``
or equivalent packages for your Linux distribution
* `` mkdir build``
* `` cd build``
* `` cmake ..``
* `` make``

## building on Windows
tested using http://mingw-w64.org/doku.php/download/win-builds
because it includes json-c and libxml2 libraries already. 

ensure winbuilds bin directory is in your PATH, in this example c:/winbuilds

* ``mkdir build``
* ``cd build``
* ``cmake -G "MinGW Makefiles" .. -DLIBXML2_LIBRARY=c:/winbuilds/lib64/libxml2.dll.a``
* ``gmake``

if compiling is slow on windows, exclude build directory from anti-malware https://support.microsoft.com/en-us/help/4028485/windows-10-add-an-exclusion-to-windows-security

### Rensas e2studio
Includes code generator, which triggers off configuration.xml, which will dump a bunch of source code into project directory.  These generated files must be ignored since they will duplicate code already in the originating cmake project.
### texas instruments code composer studio
[ARM-CGT](https://www.ti.com/tool/ARM-CGT) from TI has incomplete cmsis headers (ccs_base.h), requiring using gcc-arm in these cases.
### NXP mcuXpresso
First time building newly imported project will pop-up "Project has no MCU settings. Configure now?" because NXP puts their own storageModule ``com.crt.config`` for generating linker script, ``com.crt.config`` will not be stored into cmake project.  MCUXpresso uses gcc-arm with redlib, requiring that the gcc that comes with mcuXpresso needs to be used.

### XML compare for CDT-eclipse (test & verification)
Test utility ``cdt-xml-compare`` permits testing via comparing a control reference file against another file.``cdt-xml-compare`` compares ``.cproject`` files and ``.project`` files.  ``cdt-xml-compare`` has absolute zilch to do with cmake-file-api (json); its only relevant to eclipse XML.  The intent is to find errors in eclipse XML files which would prevent them from being recognized as valid project by IDE.  When project file has error, typically the IDE will only say it cant read it, or just crash rather than give the actual problem.  This compare utility is intended to show where the difference is between a project file created by an IDE (control file), vs. a file created by something else (file under test).  Once the file-under-test passes compare uility, it is expected to load into IDE without error.  Often the file under test will have elements which do not exist in control file; they are printed out in the end of test. ``cdt-xml-compare`` is built, but not installed.

## Translation of board and MCU names
(#trans)
IDEs from MCU vendors might require MCU names to be defined (spelled) correctly in order to function properly.  Some cmake projects may not define ``BOARD``with spelling correct, or MCU part number is not defined precisely ether.


Internal board table is provided for some boards, but other hardware will always exist, and new board could come out tomorrow.  When the file `` translate_board_mcu.json`` is present in the build directory when running this generator, then this json board table will override the internal table.   


