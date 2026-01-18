#!/bin/bash
cd ../../
###
### Fully working
###

# Main Emulators (Always compile)
make clean CONSOLE="QuickNES"     CORE=cores/QuickNES_Core # Nintendo Entertainment System / Famicom
make clean CONSOLE="FCEUmm"       CORE=cores/libretro-fceumm # Famicom Disk System / Nintendo Entertainment System (Fast)
make clean CONSOLE="Snes9x 2002"  CORE=cores/snes9x2002 # Super Nintendo Entertainment System - Super Famicom
make clean CONSOLE="Snes9x 2005"  CORE=cores/snes9x2005 # Super Nintendo Entertainment System (Fast) - Super Famicom (Fast)
make clean CONSOLE="Gambatte"     CORE=cores/libretro-gambatte # Game Boy + Color
make clean CONSOLE="TGB Dual"     CORE=cores/libretro-tgbdual # Game Boy + Color (2P)
make clean CONSOLE="gpSP"         CORE=cores/gpsp # Game Boy Advance
make clean CONSOLE="PicoDrive"    CORE=cores/picodrive make cleanFILE=-fmake cleanfile.libretro # Mega Drive - Genesis / Mega-CD - Sega CD
make clean CONSOLE="Gearsystem"   CORE=cores/Gearsystem/platforms/libretro # Game Gear / Master System - Mark III
make clean CONSOLE="M2k"          CORE=cores/libretro-mame2000 # Mame 2000
make clean CONSOLE="M2k-N"        CORE=cores/libretro-mamenummacwaytausend # Mame 2000 (Extra)
make clean CONSOLE="PCE-Fast"     CORE=cores/libretro-beetle-pce-fast # PC Engine

# Games / Apps
make clean CONSOLE="2048"         CORE=cores/libretro-2048 make cleanFILE=-fmake cleanfile.libretro # 2048 clone
make clean CONSOLE="Gong"         CORE=cores/gong make cleanFILE=-fmake cleanfile.libretro # Pong clone
make clean CONSOLE="PocketCDG"    CORE=cores/libretro-pocketcdg # Karaoke music player
make clean CONSOLE="GME"          CORE=cores/libretro-gme # Game Music Emu

# Engines
make clean CONSOLE="PrBoom"       CORE=cores/libretro-prboom # DOOM
make clean CONSOLE="ECWolf"       CORE=cores/ecwolf/src/libretro # Wolfenstein 3D
make clean CONSOLE="XRick"        CORE=cores/libretro-xrick # Rick Dangerous
make clean CONSOLE="Cannonball"   CORE=cores/cannonball # OutRun Engine
make clean CONSOLE="REminiscence" CORE=cores/REminiscence # Flashback

# PC Emulators
make clean CONSOLE="Fuse"         CORE=cores/libretro-fuse # ZX Spectrum
make clean CONSOLE="cap32"        CORE=cores/libretro-cap32 # Amstrad - CPC
make clean CONSOLE="blueMSX"      CORE=cores/libretro-blueMSX # MSX computers
make clean CONSOLE="Atari 800"    CORE=cores/libretro-atari800lib # Atari - 800
make clean CONSOLE="EightyOne"    CORE=cores/libretro-81 # ZX81
make clean CONSOLE="Theodore"     CORE=cores/theodore # Thomson - MO/TO

# Console Emulators
make clean CONSOLE="Stella 2014"  CORE=cores/libretro-stella2014 # Atari - 2600
make clean CONSOLE="a5200"        CORE=cores/a5200 # Atari - 5200
make clean CONSOLE="ProSystem"    CORE=cores/libretro-prosystem # Atari - 7800
make clean CONSOLE="Gearcoleco"   CORE=cores/Gearcoleco/platforms/libretro # ColecoVision
make clean CONSOLE="FreeChaF"     CORE=cores/FreeChaF # Fairchild ChannelF / Video Entertainment System
make clean CONSOLE="FreeIntv"     CORE=cores/FreeIntv # Intellivision
make clean CONSOLE="vecx"         CORE=cores/libretro-vecx # Vectrex

# Handheld Emulators
make clean CONSOLE="WonderSwan"   CORE=cores/libretro-beetle-wswan # WonderSwan + Color
make clean CONSOLE="PokeMini"     CORE=cores/PokeMini # Pokémon Mini
make clean CONSOLE="GameWatch"    CORE=cores/libretro-gw # Game & Watch
make clean CONSOLE="RACE"         CORE=cores/RACE # Neo Geo Pocket + Color
make clean CONSOLE="Handy"        CORE=cores/libretro-handy # Atari - Lynx
make clean CONSOLE="Potator"      CORE=cores/potator/platform/libretro # Watara Supervision

# Virtual machines / Fantasy consoles
make clean CONSOLE="JAXE"         CORE=cores/jaxe make cleanFILE=-fmake cleanfile.libretro # XO-CHIP/CHIP-8
make clean CONSOLE="Retro8"       CORE=cores/retro8 # PICO-8 (Fast)
make clean CONSOLE="VaporSpec"    CORE=cores/vaporspec/machine make cleanFILE=-fmake cleanfile.libretro # VaporSpec

# Extra (Shouldn't compile unless requested)
#make clean CONSOLE="Gearboy"      CORE=cores/Gearboy/platforms/libretro # Game Boy + Color
#make clean CONSOLE="DBLCherryGB"  CORE=cores/libretro-doublecherryGB # Game Boy + Color (2P) (crashes more often)
#make clean CONSOLE="retro-tennis" CORE=cores/retro-tennis # Pong Clone

###
### Working but issues/untested
###

# Games / Apps
make clean CONSOLE="MrBoom"       CORE=cores/libretro-mrboom # Bomberman clone
make clean CONSOLE="Jump 'n Bump" CORE=cores/libretro-jumpnbump # Jump 'n Bump

# Engines
make clean CONSOLE="NXEngine"     CORE=cores/libretro-nxengine # Cave Story
make clean CONSOLE="TyrQuake"     CORE=cores/tyrquake # Quake 1

# PC Emulators
make clean clean CONSOLE="Vice c64sc" CORE=cores/libretro-vice EMUTYPE=x64sc # Commodore CLEAN
make clean CONSOLE="Vice c64sc"   CORE=cores/libretro-vice EMUTYPE=x64sc # Commodore
make clean clean CONSOLE="Vice c64sc" CORE=cores/libretro-vice EMUTYPE=x64sc # Commodore CLEAN
make clean clean CONSOLE="Vice c64"   CORE=cores/libretro-vice EMUTYPE=x64 # Commodore CLEAN
make clean CONSOLE="Vice c64"     CORE=cores/libretro-vice EMUTYPE=x64 # Commodore
make clean clean CONSOLE="Vice c64"   CORE=cores/libretro-vice EMUTYPE=x64 # Commodore CLEAN
make clean clean CONSOLE="Vice vic20" CORE=cores/libretro-vice EMUTYPE=xvic # Commodore CLEAN
make clean CONSOLE="Vice vic20"   CORE=cores/libretro-vice EMUTYPE=xvic # Commodore
make clean clean CONSOLE="Vice vic20" CORE=cores/libretro-vice EMUTYPE=xvic # Commodore CLEAN
make clean CONSOLE="bk"           CORE=cores/bk-emulator make cleanFILE=-fmake cleanfile.libretro # Elektronika - BK-0010/BK-0011
make clean CONSOLE="O2EM"         CORE=cores/libretro-o2em # Magnavox - Odyssey2 / Philips Videopac+
make clean CONSOLE="QUASI88"      CORE=cores/libretro-quasi88 # NEC PC-8000 / PC-8800
make clean CONSOLE="xmil"         CORE=cores/libretro-xmil/libretro # Sharp X-1

# Console Emulators
make clean CONSOLE="Beetle SGX"   CORE=cores/libretro-beetle-supergrafx # NEC - PC Engine SuperGrafx
make clean CONSOLE="Beetle PC-FX" CORE=cores/libretro-beetle-pcfx # NEC - PC-FX
make clean CONSOLE="Beetle VB"    CORE=cores/libretro-beetle-vb # Virtual Boy
make clean CONSOLE="Geolith"      CORE=cores/libretro-geolith/libretro # Neo Geo AES / MVS

# Virtual machines / Fantasy consoles
make clean CONSOLE="arduous"      CORE=cores/arduous # Arduboy
make clean CONSOLE="Fake-08"      CORE=cores/fake-08/platform/libretro # PICO-8
make clean CONSOLE="LowRes NX"    CORE=cores/lowres-nx/platform/LibRetro # LowRes NX

make clean CONSOLE="x48"          CORE=cores/libretro-x48 # HP48 Calculator
make clean CONSOLE="Tama"         CORE=cores/tamalibretro # Tamagotchi P1

# Extra (Shouldn't compile unless requested)
#make clean CONSOLE="CrocoDS"      CORE=cores/libretro-crocods # Amstrad - CPC
#make clean CONSOLE="Beetle Lynx"  CORE=cores/libretro-beetle-lynx # Atari - Lynx
#make clean clean CONSOLE=c64f     CORE=cores/libretro-frodo EMUTYPE=frodo
#make clean CONSOLE=c64f           CORE=cores/libretro-frodo EMUTYPE=frodo
#make clean clean CONSOLE=c64f     CORE=cores/libretro-frodo EMUTYPE=frodo
#make clean clean CONSOLE=c64fc    CORE=cores/libretro-frodo EMUTYPE=frodosc
#make clean CONSOLE=c64fc          CORE=cores/libretro-frodo EMUTYPE=frodosc
#make clean clean CONSOLE=c64fc    CORE=cores/libretro-frodo EMUTYPE=frodosc
#make clean CONSOLE="VBA Next"     CORE=cores/vba-next # gba, Too Slow
#make clean CONSOLE="mGBA"         CORE=cores/mgba # gba, Too Slow
#make clean CONSOLE="Nestopia"     CORE=cores/nestopia/libretro # nes, Too Slow
#make clean CONSOLE="GP-GX"        CORE=cores/Genesis-Plus-GX make cleanFILE=-fmake cleanfile.libretro # Sega (Accurate)

###
### working but major issues, not to release
###

#make clean -C cores/fbalpha2012/svn-current/trunk platform=sf2000 -fmake cleanfile.libretro generate-files && make clean CONSOLE=fba CORE=cores/fbalpha2012/svn-current/trunk make cleanFILE=-fmake cleanfile.libretro
#make clean CONSOLE=mame2003   CORE=cores/libretro-mame2003-plus
#make clean CONSOLE=mame2003mw CORE=cores/mame2003_midway
#make clean CONSOLE=atarist    CORE=cores/hatari make cleanFILE=-fmake cleanfile.libretro
#make clean CONSOLE=geargrafx  CORE=../cores_not_in_git/Geargrafx/platforms/libretro

###
### test cores
###

#make clean CONSOLE=testadv CORE=cores/libretro-samples/tests/test_advanced
#make clean CONSOLE=testwav CORE=cores/libretro-samples/audio/audio_playback_wav

true
