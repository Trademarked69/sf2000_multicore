#!/bin/bash
cd ../../
###
### Fully working
###

# Main Emulators (Always compile)
make CONSOLE="QuickNES"     CORE=cores/working-cores/QuickNES_Core # Nintendo Entertainment System / Famicom
make CONSOLE="FCEUmm"       CORE=cores/libretro-fceumm # Famicom Disk System / Nintendo Entertainment System (Fast)
make CONSOLE="Snes9x 2002"  CORE=cores/working-cores/snes9x2002 # Super Nintendo Entertainment System - Super Famicom
make CONSOLE="Snes9x 2005"  CORE=cores/working-cores/snes9x2005 # Super Nintendo Entertainment System (Fast) - Super Famicom (Fast)
make CONSOLE="Gambatte"     CORE=cores/working-cores/libretro-gambatte # Game Boy + Color
make CONSOLE="TGB Dual"     CORE=cores/libretro-tgbdual # Game Boy + Color (2P)
make CONSOLE="gpSP"         CORE=cores/working-cores/gpsp # Game Boy Advance
make CONSOLE="gpSP-F"       CORE=cores/working-cores/gpsp-ff # Game Boy Advance (Fast)
make clean CONSOLE="gpSP-F" CORE=cores/working-cores/gpsp-ff # Game Boy Advance (Fast) Clean
make CONSOLE="gpSP-FF"      CORE=cores/working-cores/gpsp-ff EXTRA_CFLAGS="-DSF2000_OPTIMIZATION_LEVEL=3" # Game Boy Advance (FFast)
make CONSOLE="PicoDrive"    CORE=cores/picodrive MAKEFILE=-fMakefile.libretro # Mega Drive - Genesis / Mega-CD - Sega CD
make CONSOLE="Gearsystem"   CORE=cores/Gearsystem/platforms/libretro # Game Gear / Master System - Mark III
make CONSOLE="M2k"          CORE=cores/libretro-mame2000 # Mame 2000
make CONSOLE="M2k-N"        CORE=cores/libretro-mamenummacwaytausend # Mame 2000 (Extra)
make CONSOLE="PCE-Fast"     CORE=cores/libretro-beetle-pce-fast # PC Engine
make CONSOLE="menu"         CORE=cores/custom-apps/FrogUI # FrogUI

# Games / Apps
make CONSOLE="2048"         CORE=cores/libretro-2048 MAKEFILE=-fMakefile.libretro # 2048 clone
make CONSOLE="Gong"         CORE=cores/gong MAKEFILE=-fMakefile.libretro # Pong clone
make CONSOLE="PocketCDG"    CORE=cores/libretro-pocketcdg # Karaoke music player
make CONSOLE="GME"          CORE=cores/libretro-gme # Game Music Emu
make CONSOLE="js2000"       CORE=cores/custom-apps/js2000 # JavaScript
make CONSOLE="froggyMP3"    CORE=cores/custom-apps/froggyMP3 # MP3
make CONSOLE="a0_player"    CORE=cores/custom-apps/sf2000-video-player # Xvid (MPEG-4)/MJPEG video player

# Engines
make CONSOLE="PrBoom"       CORE=cores/libretro-prboom # DOOM
make CONSOLE="ECWolf"       CORE=cores/ecwolf/src/libretro # Wolfenstein 3D
make CONSOLE="XRick"        CORE=cores/libretro-xrick # Rick Dangerous
make CONSOLE="Cannonball"   CORE=cores/cannonball # OutRun Engine
make CONSOLE="REminiscence" CORE=cores/REminiscence # Flashback

# PC Emulators
make CONSOLE="Fuse"         CORE=cores/libretro-fuse # ZX Spectrum
make CONSOLE="cap32"        CORE=cores/libretro-cap32 # Amstrad - CPC
make CONSOLE="blueMSX"      CORE=cores/working-cores/libretro-blueMSX # MSX computers
make CONSOLE="Atari 800"    CORE=cores/libretro-atari800lib # Atari - 800
make CONSOLE="EightyOne"    CORE=cores/libretro-81 # ZX81
make CONSOLE="Theodore"     CORE=cores/theodore # Thomson - MO/TO

# Console Emulators
make CONSOLE="Stella 2014"  CORE=cores/libretro-stella2014 # Atari - 2600
make CONSOLE="a5200"        CORE=cores/a5200 # Atari - 5200
make CONSOLE="ProSystem"    CORE=cores/libretro-prosystem # Atari - 7800
make CONSOLE="Gearcoleco"   CORE=cores/Gearcoleco/platforms/libretro # ColecoVision
make CONSOLE="FreeChaF"     CORE=cores/FreeChaF # Fairchild ChannelF / Video Entertainment System
make CONSOLE="FreeIntv"     CORE=cores/FreeIntv # Intellivision
make CONSOLE="vecx"         CORE=cores/libretro-vecx # Vectrex
make CONSOLE="qpsx"         CORE=cores/sf2000-qpsx-playstation-emulator # PlayStation

# Handheld Emulators
make CONSOLE="WonderSwan"   CORE=cores/libretro-beetle-wswan # WonderSwan + Color
make CONSOLE="PokeMini"     CORE=cores/PokeMini # Pokémon Mini
make CONSOLE="GameWatch"    CORE=cores/libretro-gw # Game & Watch
make CONSOLE="RACE"         CORE=cores/RACE # Neo Geo Pocket + Color
make CONSOLE="Handy"        CORE=cores/libretro-handy # Atari - Lynx
make CONSOLE="Potator"      CORE=cores/potator/platform/libretro # Watara Supervision

# Virtual machines / Fantasy consoles
make CONSOLE="JAXE"         CORE=cores/jaxe MAKEFILE=-fMakefile.libretro # XO-CHIP/CHIP-8
make CONSOLE="Retro8"       CORE=cores/retro8 # PICO-8 (Fast)
make CONSOLE="VaporSpec"    CORE=cores/vaporspec/machine MAKEFILE=-fMakefile.libretro # VaporSpec

# Extra (Shouldn't compile unless requested)
#make CONSOLE="Gearboy"      CORE=cores/Gearboy/platforms/libretro # Game Boy + Color
#make CONSOLE="DBLCherryGB"  CORE=cores/libretro-doublecherryGB # Game Boy + Color (2P) (crashes more often)
#make CONSOLE="retro-tennis" CORE=cores/retro-tennis # Pong Clone

###
### Working but issues/untested
###

# Games / Apps
make CONSOLE="MrBoom"       CORE=cores/libretro-mrboom # Bomberman clone
make CONSOLE="Jump 'n Bump" CORE=cores/libretro-jumpnbump # Jump 'n Bump

# Engines
make CONSOLE="NXEngine"     CORE=cores/libretro-nxengine # Cave Story
make CONSOLE="TyrQuake"     CORE=cores/tyrquake # Quake 1

# PC Emulators
make clean CONSOLE="Vice c64sc" CORE=cores/working-cores/libretro-vice EMUTYPE=x64sc # Commodore CLEAN
make CONSOLE="Vice c64sc"   CORE=cores/working-cores/libretro-vice EMUTYPE=x64sc # Commodore
make clean CONSOLE="Vice c64sc" CORE=cores/working-cores/libretro-vice EMUTYPE=x64sc # Commodore CLEAN
make clean CONSOLE="Vice c64"   CORE=cores/working-cores/libretro-vice EMUTYPE=x64 # Commodore CLEAN
make CONSOLE="Vice c64"     CORE=cores/working-cores/libretro-vice EMUTYPE=x64 # Commodore
make clean CONSOLE="Vice c64"   CORE=cores/working-cores/libretro-vice EMUTYPE=x64 # Commodore CLEAN
make clean CONSOLE="Vice vic20" CORE=cores/working-cores/libretro-vice EMUTYPE=xvic # Commodore CLEAN
make CONSOLE="Vice vic20"   CORE=cores/working-cores/libretro-vice EMUTYPE=xvic # Commodore
make clean CONSOLE="Vice vic20" CORE=cores/working-cores/ibretro-vice EMUTYPE=xvic # Commodore CLEAN
make CONSOLE="bk"           CORE=cores/bk-emulator MAKEFILE=-fMakefile.libretro # Elektronika - BK-0010/BK-0011
make CONSOLE="O2EM"         CORE=cores/libretro-o2em # Magnavox - Odyssey2 / Philips Videopac+
make CONSOLE="QUASI88"      CORE=cores/libretro-quasi88 # NEC PC-8000 / PC-8800
make CONSOLE="xmil"         CORE=cores/libretro-xmil/libretro # Sharp X-1

# Console Emulators
make CONSOLE="Beetle SGX"   CORE=cores/libretro-beetle-supergrafx # NEC - PC Engine SuperGrafx
make CONSOLE="Beetle PC-FX" CORE=cores/libretro-beetle-pcfx # NEC - PC-FX
make CONSOLE="Beetle VB"    CORE=cores/libretro-beetle-vb # Virtual Boy
make CONSOLE="Geolith"      CORE=cores/libretro-geolith/libretro # Neo Geo AES / MVS

# Virtual machines / Fantasy consoles
make CONSOLE="arduous"      CORE=cores/arduous # Arduboy
make CONSOLE="Fake-08"      CORE=cores/working-cores/fake-08/platform/libretro # PICO-8
make CONSOLE="LowRes NX"    CORE=cores/lowres-nx/platform/LibRetro # LowRes NX

make CONSOLE="x48"          CORE=cores/libretro-x48 # HP48 Calculator
make CONSOLE="Tama"         CORE=cores/tamalibretro # Tamagotchi P1

# Extra (Shouldn't compile unless requested)
#make CONSOLE="CrocoDS"      CORE=cores/libretro-crocods # Amstrad - CPC
#make CONSOLE="Beetle Lynx"  CORE=cores/libretro-beetle-lynx # Atari - Lynx
#make clean CONSOLE=c64f     CORE=cores/working-cores/libretro-frodo EMUTYPE=frodo
#make CONSOLE=c64f           CORE=cores/working-cores/libretro-frodo EMUTYPE=frodo
#make clean CONSOLE=c64f     CORE=cores/working-cores/libretro-frodo EMUTYPE=frodo
#make clean CONSOLE=c64fc    CORE=cores/working-cores/libretro-frodo EMUTYPE=frodosc
#make CONSOLE=c64fc          CORE=cores/working-cores/libretro-frodo EMUTYPE=frodosc
#make clean CONSOLE=c64fc    CORE=cores/working-cores/libretro-frodo EMUTYPE=frodosc
#make CONSOLE="VBA Next"     CORE=cores/vba-next # gba, Too Slow
#make CONSOLE="mGBA"         CORE=cores/mgba # gba, Too Slow
#make CONSOLE="Nestopia"     CORE=cores/nestopia/libretro # nes, Too Slow
#make CONSOLE="GP-GX"        CORE=cores/Genesis-Plus-GX MAKEFILE=-fMakefile.libretro # Sega (Accurate)

###
### working but major issues, not to release
###

#make -C cores/fbalpha2012/svn-current/trunk platform=sf2000 -fmakefile.libretro generate-files && make CONSOLE=fba CORE=cores/fbalpha2012/svn-current/trunk MAKEFILE=-fmakefile.libretro
#make CONSOLE=mame2003   CORE=cores/libretro-mame2003-plus
#make CONSOLE=mame2003mw CORE=cores/mame2003_midway
#make CONSOLE=atarist    CORE=cores/hatari MAKEFILE=-fMakefile.libretro
#make CONSOLE=geargrafx  CORE=../cores_not_in_git/Geargrafx/platforms/libretro

###
### test cores
###

#make CONSOLE=testadv CORE=cores/libretro-samples/tests/test_advanced
#make CONSOLE=testwav CORE=cores/libretro-samples/audio/audio_playback_wav

true
