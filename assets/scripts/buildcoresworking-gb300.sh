#!/bin/bash
cd ../../
###
### Fully working
###

# Main Emulators (Always compile)
make FROGGY_TYPE=GB300V2 CONSOLE="QuickNES"     CORE=cores/working-cores/QuickNES_Core # Nintendo Entertainment System / Famicom
make FROGGY_TYPE=GB300V2 CONSOLE="FCEUmm"       CORE=cores/libretro-fceumm # Famicom Disk System / Nintendo Entertainment System (Fast)
make FROGGY_TYPE=GB300V2 CONSOLE="Snes9x 2002"  CORE=cores/working-cores/snes9x2002 # Super Nintendo Entertainment System - Super Famicom
make FROGGY_TYPE=GB300V2 CONSOLE="Snes9x 2005"  CORE=cores/working-cores/snes9x2005 # Super Nintendo Entertainment System (Fast) - Super Famicom (Fast)
make FROGGY_TYPE=GB300V2 CONSOLE="Gambatte"     CORE=cores/working-cores/libretro-gambatte # Game Boy + Color
make FROGGY_TYPE=GB300V2 CONSOLE="TGB Dual"     CORE=cores/libretro-tgbdual # Game Boy + Color (2P)
make FROGGY_TYPE=GB300V2 CONSOLE="gpSP"         CORE=cores/working-cores/gpsp # Game Boy Advance
make FROGGY_TYPE=GB300V2 CONSOLE="gpSP-F"       CORE=cores/working-cores/gpsp-ff # Game Boy Advance (Fast)
make clean FROGGY_TYPE=GB300V2 CONSOLE="gpSP-F" CORE=cores/working-cores/gpsp-ff # Game Boy Advance (Fast) Clean
make FROGGY_TYPE=GB300V2 CONSOLE="gpSP-FF"      CORE=cores/working-cores/gpsp-ff EXTRA_CFLAGS="-DSF2000_OPTIMIZATION_LEVEL=3" # Game Boy Advance (FFast)
make FROGGY_TYPE=GB300V2 CONSOLE="PicoDrive"    CORE=cores/picodrive MAKEFILE=-fMakefile.libretro # Mega Drive - Genesis / Mega-CD - Sega CD
make FROGGY_TYPE=GB300V2 CONSOLE="Gearsystem"   CORE=cores/Gearsystem/platforms/libretro # Game Gear / Master System - Mark III
make FROGGY_TYPE=GB300V2 CONSOLE="M2k"          CORE=cores/libretro-mame2000 # Mame 2000
make FROGGY_TYPE=GB300V2 CONSOLE="M2k-N"        CORE=cores/libretro-mamenummacwaytausend # Mame 2000 (Extra)
make FROGGY_TYPE=GB300V2 CONSOLE="PCE-Fast"     CORE=cores/libretro-beetle-pce-fast # PC Engine
make FROGGY_TYPE=GB300V2 CONSOLE="menu"         CORE=cores/custom-apps/FrogUI # FrogUI

# Games / Apps
make FROGGY_TYPE=GB300V2 CONSOLE="2048"         CORE=cores/libretro-2048 MAKEFILE=-fMakefile.libretro # 2048 clone
make FROGGY_TYPE=GB300V2 CONSOLE="Gong"         CORE=cores/gong MAKEFILE=-fMakefile.libretro # Pong clone
make FROGGY_TYPE=GB300V2 CONSOLE="PocketCDG"    CORE=cores/libretro-pocketcdg # Karaoke music player
make FROGGY_TYPE=GB300V2 CONSOLE="GME"          CORE=cores/libretro-gme # Game Music Emu
make FROGGY_TYPE=GB300V2 CONSOLE="js2000"       CORE=cores/custom-apps/js2000 # JavaScript
make FROGGY_TYPE=GB300V2 CONSOLE="froggyMP3"    CORE=cores/custom-apps/froggyMP3 # MP3
make FROGGY_TYPE=GB300V2 CONSOLE="a0_player"    CORE=cores/custom-apps/sf2000-video-player # Xvid (MPEG-4)/MJPEG video player

# Engines
make FROGGY_TYPE=GB300V2 CONSOLE="PrBoom"       CORE=cores/libretro-prboom # DOOM
make FROGGY_TYPE=GB300V2 CONSOLE="ECWolf"       CORE=cores/ecwolf/src/libretro # Wolfenstein 3D
make FROGGY_TYPE=GB300V2 CONSOLE="XRick"        CORE=cores/libretro-xrick # Rick Dangerous
make FROGGY_TYPE=GB300V2 CONSOLE="Cannonball"   CORE=cores/cannonball # OutRun Engine
make FROGGY_TYPE=GB300V2 CONSOLE="REminiscence" CORE=cores/REminiscence # Flashback

# PC Emulators
make FROGGY_TYPE=GB300V2 CONSOLE="Fuse"         CORE=cores/libretro-fuse # ZX Spectrum
make FROGGY_TYPE=GB300V2 CONSOLE="cap32"        CORE=cores/libretro-cap32 # Amstrad - CPC
make FROGGY_TYPE=GB300V2 CONSOLE="blueMSX"      CORE=cores/working-cores/libretro-blueMSX # MSX computers
make FROGGY_TYPE=GB300V2 CONSOLE="Atari 800"    CORE=cores/libretro-atari800lib # Atari - 800
make FROGGY_TYPE=GB300V2 CONSOLE="EightyOne"    CORE=cores/libretro-81 # ZX81
make FROGGY_TYPE=GB300V2 CONSOLE="Theodore"     CORE=cores/theodore # Thomson - MO/TO

# Console Emulators
make FROGGY_TYPE=GB300V2 CONSOLE="Stella 2014"  CORE=cores/libretro-stella2014 # Atari - 2600
make FROGGY_TYPE=GB300V2 CONSOLE="a5200"        CORE=cores/a5200 # Atari - 5200
make FROGGY_TYPE=GB300V2 CONSOLE="ProSystem"    CORE=cores/libretro-prosystem # Atari - 7800
make FROGGY_TYPE=GB300V2 CONSOLE="Gearcoleco"   CORE=cores/Gearcoleco/platforms/libretro # ColecoVision
make FROGGY_TYPE=GB300V2 CONSOLE="FreeChaF"     CORE=cores/FreeChaF # Fairchild ChannelF / Video Entertainment System
make FROGGY_TYPE=GB300V2 CONSOLE="FreeIntv"     CORE=cores/FreeIntv # Intellivision
make FROGGY_TYPE=GB300V2 CONSOLE="vecx"         CORE=cores/libretro-vecx # Vectrex
make FROGGY_TYPE=GB300V2 CONSOLE="qpsx"         CORE=cores/sf2000-qpsx-playstation-emulator # PlayStation

# Handheld Emulators
make FROGGY_TYPE=GB300V2 CONSOLE="WonderSwan"   CORE=cores/libretro-beetle-wswan # WonderSwan + Color
make FROGGY_TYPE=GB300V2 CONSOLE="PokeMini"     CORE=cores/PokeMini # Pokémon Mini
make FROGGY_TYPE=GB300V2 CONSOLE="GameWatch"    CORE=cores/libretro-gw # Game & Watch
make FROGGY_TYPE=GB300V2 CONSOLE="RACE"         CORE=cores/RACE # Neo Geo Pocket + Color
make FROGGY_TYPE=GB300V2 CONSOLE="Handy"        CORE=cores/libretro-handy # Atari - Lynx
make FROGGY_TYPE=GB300V2 CONSOLE="Potator"      CORE=cores/potator/platform/libretro # Watara Supervision

# Virtual machines / Fantasy consoles
make FROGGY_TYPE=GB300V2 CONSOLE="JAXE"         CORE=cores/jaxe MAKEFILE=-fMakefile.libretro # XO-CHIP/CHIP-8
make FROGGY_TYPE=GB300V2 CONSOLE="Retro8"       CORE=cores/retro8 # PICO-8 (Fast)
make FROGGY_TYPE=GB300V2 CONSOLE="VaporSpec"    CORE=cores/vaporspec/machine MAKEFILE=-fMakefile.libretro # VaporSpec

# Extra (Shouldn't compile unless requested)
#make FROGGY_TYPE=GB300V2 CONSOLE="Gearboy"      CORE=cores/Gearboy/platforms/libretro # Game Boy + Color
#make FROGGY_TYPE=GB300V2 CONSOLE="DBLCherryGB"  CORE=cores/libretro-doublecherryGB # Game Boy + Color (2P) (crashes more often)
#make FROGGY_TYPE=GB300V2 CONSOLE="retro-tennis" CORE=cores/retro-tennis # Pong Clone

###
### Working but issues/untested
###

# Games / Apps
make FROGGY_TYPE=GB300V2 CONSOLE="MrBoom"       CORE=cores/libretro-mrboom # Bomberman clone
make FROGGY_TYPE=GB300V2 CONSOLE="Jump 'n Bump" CORE=cores/libretro-jumpnbump # Jump 'n Bump

# Engines
make FROGGY_TYPE=GB300V2 CONSOLE="NXEngine"     CORE=cores/libretro-nxengine # Cave Story
make FROGGY_TYPE=GB300V2 CONSOLE="TyrQuake"     CORE=cores/tyrquake # Quake 1

# PC Emulators
make clean FROGGY_TYPE=GB300V2 CONSOLE="Vice c64sc" CORE=cores/working-cores/libretro-vice EMUTYPE=x64sc # Commodore CLEAN
make FROGGY_TYPE=GB300V2 CONSOLE="Vice c64sc"   CORE=cores/working-cores/libretro-vice EMUTYPE=x64sc # Commodore
make clean FROGGY_TYPE=GB300V2 CONSOLE="Vice c64sc" CORE=cores/working-cores/libretro-vice EMUTYPE=x64sc # Commodore CLEAN
make clean FROGGY_TYPE=GB300V2 CONSOLE="Vice c64"   CORE=cores/working-cores/libretro-vice EMUTYPE=x64 # Commodore CLEAN
make FROGGY_TYPE=GB300V2 CONSOLE="Vice c64"     CORE=cores/working-cores/libretro-vice EMUTYPE=x64 # Commodore
make clean FROGGY_TYPE=GB300V2 CONSOLE="Vice c64"   CORE=cores/working-cores/libretro-vice EMUTYPE=x64 # Commodore CLEAN
make clean FROGGY_TYPE=GB300V2 CONSOLE="Vice vic20" CORE=cores/working-cores/libretro-vice EMUTYPE=xvic # Commodore CLEAN
make FROGGY_TYPE=GB300V2 CONSOLE="Vice vic20"   CORE=cores/working-cores/libretro-vice EMUTYPE=xvic # Commodore
make clean FROGGY_TYPE=GB300V2 CONSOLE="Vice vic20" CORE=cores/working-cores/ibretro-vice EMUTYPE=xvic # Commodore CLEAN
make FROGGY_TYPE=GB300V2 CONSOLE="bk"           CORE=cores/bk-emulator MAKEFILE=-fMakefile.libretro # Elektronika - BK-0010/BK-0011
make FROGGY_TYPE=GB300V2 CONSOLE="O2EM"         CORE=cores/libretro-o2em # Magnavox - Odyssey2 / Philips Videopac+
make FROGGY_TYPE=GB300V2 CONSOLE="QUASI88"      CORE=cores/libretro-quasi88 # NEC PC-8000 / PC-8800
make FROGGY_TYPE=GB300V2 CONSOLE="xmil"         CORE=cores/libretro-xmil/libretro # Sharp X-1

# Console Emulators
make FROGGY_TYPE=GB300V2 CONSOLE="Beetle SGX"   CORE=cores/libretro-beetle-supergrafx # NEC - PC Engine SuperGrafx
make FROGGY_TYPE=GB300V2 CONSOLE="Beetle PC-FX" CORE=cores/libretro-beetle-pcfx # NEC - PC-FX
make FROGGY_TYPE=GB300V2 CONSOLE="Beetle VB"    CORE=cores/libretro-beetle-vb # Virtual Boy
make FROGGY_TYPE=GB300V2 CONSOLE="Geolith"      CORE=cores/libretro-geolith/libretro # Neo Geo AES / MVS

# Virtual machines / Fantasy consoles
make FROGGY_TYPE=GB300V2 CONSOLE="arduous"      CORE=cores/arduous # Arduboy
make FROGGY_TYPE=GB300V2 CONSOLE="Fake-08"      CORE=cores/working-cores/fake-08/platform/libretro # PICO-8
make FROGGY_TYPE=GB300V2 CONSOLE="LowRes NX"    CORE=cores/lowres-nx/platform/LibRetro # LowRes NX

make FROGGY_TYPE=GB300V2 CONSOLE="x48"          CORE=cores/libretro-x48 # HP48 Calculator
make FROGGY_TYPE=GB300V2 CONSOLE="Tama"         CORE=cores/tamalibretro # Tamagotchi P1

# Extra (Shouldn't compile unless requested)
#make FROGGY_TYPE=GB300V2 CONSOLE="CrocoDS"      CORE=cores/libretro-crocods # Amstrad - CPC
#make FROGGY_TYPE=GB300V2 CONSOLE="Beetle Lynx"  CORE=cores/libretro-beetle-lynx # Atari - Lynx
#make clean FROGGY_TYPE=GB300V2 CONSOLE=c64f     CORE=cores/working-cores/libretro-frodo EMUTYPE=frodo
#make FROGGY_TYPE=GB300V2 CONSOLE=c64f           CORE=cores/working-cores/libretro-frodo EMUTYPE=frodo
#make clean FROGGY_TYPE=GB300V2 CONSOLE=c64f     CORE=cores/working-cores/libretro-frodo EMUTYPE=frodo
#make clean FROGGY_TYPE=GB300V2 CONSOLE=c64fc    CORE=cores/working-cores/libretro-frodo EMUTYPE=frodosc
#make FROGGY_TYPE=GB300V2 CONSOLE=c64fc          CORE=cores/working-cores/libretro-frodo EMUTYPE=frodosc
#make clean FROGGY_TYPE=GB300V2 CONSOLE=c64fc    CORE=cores/working-cores/libretro-frodo EMUTYPE=frodosc
#make FROGGY_TYPE=GB300V2 CONSOLE="VBA Next"     CORE=cores/vba-next # gba, Too Slow
#make FROGGY_TYPE=GB300V2 CONSOLE="mGBA"         CORE=cores/mgba # gba, Too Slow
#make FROGGY_TYPE=GB300V2 CONSOLE="Nestopia"     CORE=cores/nestopia/libretro # nes, Too Slow
#make FROGGY_TYPE=GB300V2 CONSOLE="GP-GX"        CORE=cores/Genesis-Plus-GX MAKEFILE=-fMakefile.libretro # Sega (Accurate)

###
### working but major issues, not to release
###

#make -C cores/fbalpha2012/svn-current/trunk platform=sf2000 -fmakefile.libretro generate-files && make FROGGY_TYPE=GB300V2 CONSOLE=fba CORE=cores/fbalpha2012/svn-current/trunk MAKEFILE=-fmakefile.libretro
#make FROGGY_TYPE=GB300V2 CONSOLE=mame2003   CORE=cores/libretro-mame2003-plus
#make FROGGY_TYPE=GB300V2 CONSOLE=mame2003mw CORE=cores/mame2003_midway
#make FROGGY_TYPE=GB300V2 CONSOLE=atarist    CORE=cores/hatari MAKEFILE=-fMakefile.libretro
#make FROGGY_TYPE=GB300V2 CONSOLE=geargrafx  CORE=../cores_not_in_git/Geargrafx/platforms/libretro

###
### test cores
###

#make FROGGY_TYPE=GB300V2 CONSOLE=testadv CORE=cores/libretro-samples/tests/test_advanced
#make FROGGY_TYPE=GB300V2 CONSOLE=testwav CORE=cores/libretro-samples/audio/audio_playback_wav

true
