# this file is NSIS script for creating installer for windows
# only to be used until install(CODE [[file(GET_RUNTIME_DEPENDENCIES)]]) works

# define the name of the installer
Outfile "ide_generator_installer.exe"
 
# define the directory to install to, 
#InstallDir $PROGRAMFILES64
InstallDir "$PROGRAMFILES64\cmake_ide_generator"
 
# default section
Section
 
# define the output path for this file
SetOutPath $INSTDIR
 
# define what to install and place it in the output path

File C:\src\cmake_ide_generator\build\query-cmake.exe
File C:\src\cmake_ide_generator\build\stm32cubeide-parse-reply.exe
File C:\src\cmake_ide_generator\build\sw4stm32-parse-reply.exe
File C:\src\cmake_ide_generator\build\ccs-parse-reply.exe
File C:\src\cmake_ide_generator\build\e2studio-parse-reply.exe
File C:\src\cmake_ide_generator\translate_board_mcu.json 
File C:\winbuilds\bin\libjson-c-2.dll
File C:\winbuilds\bin\libxml2-2.dll
File C:\winbuilds\bin\libiconv.dll
File C:\winbuilds\bin\libz-1.dll
File C:\winbuilds\bin\libwinpthread-1.dll

EnVar::AddValue "path" $INSTDIR

SectionEnd
