#!/bin/sh

# run ./xxx.sh re to remake

make $1 -j8 DEBUG=1 DEFINE=SHOWFPS && time ./ft_vox | python3 debug/progs/showFPS.py

exit 0