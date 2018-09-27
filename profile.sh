#!/bin/bash

#~ rm -rf profbuild
#~ mkdir -p profbuild
#~ cd profbuild
#~ cmake -D INPUT_DICTIONARIES=/home/wkieras/input/dodatki.tab,../input/PoliMorfSmall.tab -D CMAKE_BUILD_TYPE=Debug -D CMAKE_CXX_FLAGS="-g -O2" -D CMAKE_SHARED_LINKER_FLAGS="-lprofiler" -D CMAKE_EXE_LINKER_FLAGS="-lprofiler" ..
#~ make
rm -f /tmp/morfeusz.prof
export LD_PRELOAD="/usr/lib/libprofiler.so"
export CPUPROFILE="/tmp/morfeusz.prof"
morfeusz/morfeusz_analyzer --dict sgjp --dict-dir /home/mlenart/opt/morfeusz/buildall/Linux-i386-false/_CPack_Packages/Linux/DEB/morfeusz2-2.0.0_sgjp-Linux-i386/usr/share/morfeusz/dictionaries < /mnt/storage/morfeusz/sents10k > /dev/null
### pprof --gv profbuild/morfeusz/morfeusz_analyzer /tmp/morfeusz.prof
