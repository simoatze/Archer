#!/bin/bash

program=$1
fopenmp=""
if [ -n "${2}" ]; then
fopenmp="-"$2
fi

clang -DUSE_MPI=0 -I$HOME/usr/lib/gcc/x86_64-unknown-linux-gnu/4.9.1/include -g -O0 -S ${fopenmp} -emit-llvm $program -o $program.s
opt -S -polly-canonicalize $program.s > $program.ll
opt -basicaa -scev-aa -polly-cloog -analyze -q $program.ll
# opt -basicaa -scev-aa -polly-dependences -analyze $program.ll
# opt -basicaa -scev-aa -polly-scops -polly-detect -polly-show -analyze $program.ll
# -debug -polly-export-jscop -polly-show
