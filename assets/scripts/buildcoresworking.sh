#!/bin/bash

./buildcoresworking-sf2000.sh
make clean
make SCREEN_SWAP=TRUE # Screen swap bisrv

./cleancoresworking.sh

./buildcoresworking-gb300.sh
make clean
make FROGGY_TYPE=GB300V2 SCREEN_SWAP=TRUE # Screen swap bisrv

true
