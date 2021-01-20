
cmake -DVENDOR=st -DBOARD=stm32l475_discovery -DCOMPILER=arm-gcc ..


windows:
cmake -G "MinGW Makefiles" -DVENDOR=st -DBOARD=stm32l475_discovery -DCOMPILER=arm-gcc ..
cmake -G "MinGW Makefiles" -DVENDOR=ti -DBOARD=cc3220_launchpad -DCOMPILER=arm-ti ..

cmake -G Ninja -DVENDOR=microchip -DBOARD=curiosity_pic32mzef -DCOMPILER=xc32 -DMCHP_HEXMATE_PATH="C:\Program Files (x86)\Microchip\MPLABX\v5.40\mplab_platform\bin" -DAFR_TOOLCHAIN_PATH="C:\Program Files\Microchip\xc32\v2.50\bin" ..
cmake -G "MinGW Makefiles" -DVENDOR=microchip -DBOARD=curiosity_pic32mzef -DCOMPILER=xc32 -DMCHP_HEXMATE_PATH="C:\Program Files (x86)\Microchip\MPLABX\v5.40\mplab_platform\bin" -DAFR_TOOLCHAIN_PATH="C:\Program Files\Microchip\xc32\v2.50\bin" ..

non-working:
make -G Ninja -DVENDOR=ti -DBOARD=cc3220_launchpad -DCOMPILER=arm-ti ..
