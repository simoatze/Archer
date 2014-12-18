#!/bin/bash

program=$1

clang -emit-llvm -S $program
opt -basicaa -da -da-delinearize=true -analyze $program.s
