#!/bin/bash
./buildcoresworking-sf2000.sh
cd ../../
make clean
make SCREEN_SWAP=TRUE # Screen swap bisrv
cd assets/scripts

./cleancoresworking.sh

./buildcoresworking-gb300.sh
cd ../../
make clean
make FROGGY_TYPE=GB300V2 SCREEN_SWAP=TRUE # Screen swap bisrv
cd assets/scripts

true
