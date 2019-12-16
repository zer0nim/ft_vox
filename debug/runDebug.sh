#!/bin/sh

# run ./xxx.sh re to remake

make $1 -j8 DEBUG=1 && time ./ft_vox

exit 0