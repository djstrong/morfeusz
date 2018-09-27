#!/bin/bash

set -exo pipefail

cd morfeusz
echo 'Ala ma kota i żółć' | DYLD_LIBRARY_PATH=. ./morfeusz
cd ../jmorfeusz
DYLD_LIBRARY_PATH=../morfeusz:. java -cp *.jar pl.sgjp.morfeusz.app.App
cd ../pymorfeusz
DYLD_LIBRARY_PATH=../morfeusz:. python test.py
cd ..
