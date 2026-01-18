#!/bin/bash
cd ../../libs/libretro-common && git submodule update --init . && cd ../../
cd assets/scripts && ./clonemain.sh && ./clonemain.sh && ./buildmain.sh # Clone twice to make sure all submodules are cloned
