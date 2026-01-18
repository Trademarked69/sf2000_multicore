#!/bin/bash
cd ../../

### Clean before building
make clean CONSOLE="QuickNES"     CORE=cores/QuickNES_Core # Nintendo Entertainment System / Famicom
make clean CONSOLE="FCEUmm"       CORE=cores/libretro-fceumm # Famicom Disk System / Nintendo Entertainment System (Fast)
make clean CONSOLE="Snes9x 2002"  CORE=cores/snes9x2002 # Super Nintendo Entertainment System - Super Famicom
make clean CONSOLE="Snes9x 2005"  CORE=cores/snes9x2005 # Super Nintendo Entertainment System (Fast) - Super Famicom (Fast)
make clean CONSOLE="Gambatte"     CORE=cores/libretro-gambatte # Game Boy + Color
make clean CONSOLE="TGB Dual"     CORE=cores/libretro-tgbdual # Game Boy + Color (2P)
make clean CONSOLE="gpSP"         CORE=cores/gpsp # Game Boy Advance
make clean CONSOLE="PicoDrive"    CORE=cores/picodrive MAKEFILE=-fMakefile.libretro # Mega Drive - Genesis / Mega-CD - Sega CD
make clean CONSOLE="Gearsystem"   CORE=cores/Gearsystem/platforms/libretro # Game Gear / Master System - Mark III
make clean CONSOLE="M2k"          CORE=cores/libretro-mame2000 # Mame 2000
make clean CONSOLE="M2k-N"        CORE=cores/libretro-mamenummacwaytausend # Mame 2000 (Extra)
make clean CONSOLE="PCE-Fast"     CORE=cores/libretro-beetle-pce-fast # PC Engine
make clean CONSOLE="menu"         CORE=cores/FrogUI # FrogUI

### Build SF2000
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
make CONSOLE="menu"         CORE=cores/FrogUI # FrogUI

make clean
make SCREEN_SWAP=TRUE # Screen swap bisrv

### Clean SF2000
make clean CONSOLE="QuickNES"     CORE=cores/QuickNES_Core # Nintendo Entertainment System / Famicom
make clean CONSOLE="FCEUmm"       CORE=cores/libretro-fceumm # Famicom Disk System / Nintendo Entertainment System (Fast)
make clean CONSOLE="Snes9x 2002"  CORE=cores/snes9x2002 # Super Nintendo Entertainment System - Super Famicom
make clean CONSOLE="Snes9x 2005"  CORE=cores/snes9x2005 # Super Nintendo Entertainment System (Fast) - Super Famicom (Fast)
make clean CONSOLE="Gambatte"     CORE=cores/libretro-gambatte # Game Boy + Color
make clean CONSOLE="TGB Dual"     CORE=cores/libretro-tgbdual # Game Boy + Color (2P)
make clean CONSOLE="gpSP"         CORE=cores/gpsp # Game Boy Advance
make clean CONSOLE="PicoDrive"    CORE=cores/picodrive MAKEFILE=-fMakefile.libretro # Mega Drive - Genesis / Mega-CD - Sega CD
make clean CONSOLE="Gearsystem"   CORE=cores/Gearsystem/platforms/libretro # Game Gear / Master System - Mark III
make clean CONSOLE="M2k"          CORE=cores/libretro-mame2000 # Mame 2000
make clean CONSOLE="M2k-N"        CORE=cores/libretro-mamenummacwaytausend # Mame 2000 (Extra)
make clean CONSOLE="PCE-Fast"     CORE=cores/libretro-beetle-pce-fast # PC Engine
make clean CONSOLE="menu"         CORE=cores/FrogUI # FrogUI

### Build GB300V2
make FROGGY_TYPE=GB300V2 CONSOLE="QuickNES"     CORE=cores/QuickNES_Core # Nintendo Entertainment System / Famicom
make FROGGY_TYPE=GB300V2 CONSOLE="FCEUmm"       CORE=cores/libretro-fceumm # Famicom Disk System / Nintendo Entertainment System (Fast)
make FROGGY_TYPE=GB300V2 CONSOLE="Snes9x 2002"  CORE=cores/snes9x2002 # Super Nintendo Entertainment System - Super Famicom
make FROGGY_TYPE=GB300V2 CONSOLE="Snes9x 2005"  CORE=cores/snes9x2005 # Super Nintendo Entertainment System (Fast) - Super Famicom (Fast)
make FROGGY_TYPE=GB300V2 CONSOLE="Gambatte"     CORE=cores/libretro-gambatte # Game Boy + Color
make FROGGY_TYPE=GB300V2 CONSOLE="TGB Dual"     CORE=cores/libretro-tgbdual # Game Boy + Color (2P)
make FROGGY_TYPE=GB300V2 CONSOLE="gpSP"         CORE=cores/gpsp # Game Boy Advance
make FROGGY_TYPE=GB300V2 CONSOLE="PicoDrive"    CORE=cores/picodrive MAKEFILE=-fMakefile.libretro # Mega Drive - Genesis / Mega-CD - Sega CD
make FROGGY_TYPE=GB300V2 CONSOLE="Gearsystem"   CORE=cores/Gearsystem/platforms/libretro # Game Gear / Master System - Mark III
make FROGGY_TYPE=GB300V2 CONSOLE="M2k"          CORE=cores/libretro-mame2000 # Mame 2000
make FROGGY_TYPE=GB300V2 CONSOLE="M2k-N"        CORE=cores/libretro-mamenummacwaytausend # Mame 2000 (Extra)
make FROGGY_TYPE=GB300V2 CONSOLE="PCE-Fast"     CORE=cores/libretro-beetle-pce-fast # PC Engine
make FROGGY_TYPE=GB300V2 CONSOLE="menu"         CORE=cores/FrogUI # FrogUI

make clean
make FROGGY_TYPE=GB300V2 SCREEN_SWAP=TRUE # Screen swap bisrv

true
