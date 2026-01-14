#!/bin/bash
cd ../../libs/libretro-common && git submodule update --init . && cd ../../
cd assets/scripts && ./clonemain.sh && ./clonemain.sh && ./buildmain.sh
