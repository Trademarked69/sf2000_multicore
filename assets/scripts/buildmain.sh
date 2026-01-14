#!/bin/bash
cd ../../

make CONSOLE="QuickNES"     CORE=cores/QuickNES_Core # Nintendo Entertainment System / Famicom
make CONSOLE="FCEUmm"       CORE=cores/libretro-fceumm # Famicom Disk System / Nintendo Entertainment System (Fast)
make CONSOLE="Snes9x 2002"  CORE=cores/snes9x2002 # Super Nintendo Entertainment System - Super Famicom
make CONSOLE="Snes9x 2005"  CORE=cores/snes9x2005 # Super Nintendo Entertainment System (Fast) - Super Famicom (Fast)
make CONSOLE="Gambatte"     CORE=cores/libretro-gambatte # Game Boy + Color
make CONSOLE="TGB Dual"     CORE=cores/libretro-tgbdual # Game Boy + Color (2P)
make CONSOLE="gpSP"         CORE=cores/gpsp # Game Boy Advance
make CONSOLE="PicoDrive"    CORE=cores/picodrive MAKEFILE=-fMakefile.libretro # Mega Drive - Genesis / Mega-CD - Sega CD
make CONSOLE="Gearsystem"   CORE=cores/Gearsystem/platforms/libretro # Game Gear / Master System - Mark III
make CONSOLE="M2k"          CORE=cores/libretro-mame2000 # Mame 2000
make CONSOLE="M2k-N"        CORE=cores/libretro-mamenummacwaytausend # Mame 2000 (Extra)
make CONSOLE="PCE-Fast"     CORE=cores/libretro-beetle-pce-fast # PC Engine

true
