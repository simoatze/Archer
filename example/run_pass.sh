#!/bin/bash -x

#################
# LLVM parameters
#################
export LLVM_PATH=$HOME/usr/bin
export CLANG=$LLVM_PATH/clang
export OPT=$LLVM_PATH/opt
#export OPT=/usr/global/tools/clang/chaos_5_x86_64_ib/clang-omp-3.5.0/bin/opt
export LLC=$LLVM_PATH/llc
export LLVM_DIS=$LLVM_PATH/llvm-dis
export LLVM_LINK=$LLVM_PATH/llvm-link

######################
# LLVM pass parameters
######################
export PASS=../src/libArcher.so
export PASS_FLAG=archer

##############
# Program name
##############
export PROG_NAME=data-race01
export PROG_NEW_NAME=$PROG_NAME\_all
$CLANG -emit-llvm -O0 -g -c $PROG_NAME.c -o $PROG_NAME.bc

################
# Link Program
################
USEDLIBS=Polly.a
$LLVM_LINK -o $PROG_NEW_NAME.bc $PROG_NAME.bc

###################
# Run analysis pass
###################
export PROG_NAME=$PROG_NEW_NAME
$OPT -mem2reg $PROG_NAME.bc -o $PROG_NAME.opt
mv $PROG_NAME.opt $PROG_NAME.bc
$LLVM_DIS -f $PROG_NAME.bc -o $PROG_NAME.ll
$OPT -load $PASS $PROG_NAME.bc -o $PROG_NAME-optimized.bc
$LLVM_DIS -f $PROG_NAME-optimized.bc -o $PROG_NAME-optimized.ll
$LLC $PROG_NAME-optimized.bc
$CLANG $PROG_NAME-optimized.s -O0 -o $PROG_NAME 
#-L/g/g90/protze1/INSTALL/chaos_5_x86_64_ib/intel-omp/lib
