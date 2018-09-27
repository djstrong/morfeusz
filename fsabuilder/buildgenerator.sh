#!/bin/bash

python morfeusz_builder --input-files=../input/PoliMorfSmall.tab,../input/dodatki.tab \
    --tagset-file=../input/sgjp-morfeusz.tagset \
    --segments-file=../input/segmenty.dat \
    --generator \
    --serialization-method=V2 \
    -o $1
