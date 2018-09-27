#!/bin/bash

echo '#!MORFEUSZ-TAGSET 0.1'
echo
echo '[TAGS]'
echo
echo $'0\tign'
cat input/sgjp-hom.tab input/dodatki.tab | cut -d$'\t' -f3 | sort -u | cat -n | sed -r 's/^\s+//g'
echo
echo '[NAMES]'
echo $'0\t'
cat input/sgjp-hom.tab input/dodatki.tab | cut -d$'\t' -f4 | egrep '.' | sort -u | cat -n | sed -r 's/^\s+//g'
