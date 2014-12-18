#!/bin/bash

program=$1
fopenmp=""
if [ -n "${2}" ]; then
fopenmp="-"$2
fi

clang -DUSE_MPI=0 -g -O0 -S ${fopenmp} -emit-llvm $program -o $program.s
opt -S -polly-canonicalize $program.s > $program.ll
# opt -basicaa -scev-aa -polly-cloog -analyze -q $program.ll
opt -basicaa -scev-aa -polly-dependences -analyze $program.ll
# opt -debug -basicaa -scev-aa -polly-detect -analyze $program.ll
# -polly-export-jscop
