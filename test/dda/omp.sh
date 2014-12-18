#!/bin/bash

program=$1

clang -S -emit-llvm -I$HOME/usr/lib/gcc/x86_64-unknown-linux-gnu/4.9.1/include $program -o $program.s
opt -S -polly-canonicalize $program.s > $program.ll
opt -basicaa -polly-import-jscop -polly-import-jscop-postfix=interchanged+tiled+vector -polly-codegen -polly-vectorizer=polly -enable-polly-openmp $program.ll | opt -O3 > $program.normalopt.ll
llc $program.normalopt.ll -o $program.normalopt.s && gcc $program.normalopt.s -o $program.normalopt.exe -L/g/g90/protze1/INSTALL/chaos_5_x86_64_ib/intel-omp/lib -liomp5

