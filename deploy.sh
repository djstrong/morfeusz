#!/bin/bash

tar -czf - target/ | ssh mlenart@chopin.ipipan.waw.pl "cd public_html && rm -rf morfeusz target && tar -xzf - && mv target morfeusz && chmod a+x morfeusz"
