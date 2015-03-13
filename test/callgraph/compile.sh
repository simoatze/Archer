#!/bin/bash

filename=$1

clang -fopenmp -O0 -g -D_GNU_SOURCE -D_DEBUG -D__STDC_CONSTANT_MACROS \
          -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -D_GNU_SOURCE \
          $filename -fsyntax-only \
          -Xclang -load -Xclang ArcherPlugin.so -Xclang \
          -plugin -Xclang archer

#          -I$HOME/usr//include -Itools/clang/include -I$BD/include -Iinclude \

