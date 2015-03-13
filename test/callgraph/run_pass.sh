#!/bin/bash
# -x

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
export PASS=$HOME/compilers/llvm35omp/Archer/lib/libArcher.so
export PASS_FLAG=archer-instcontext
rm -rf .polly

##############
# Program name
##############
export PROG_NAME=$1
export EXEC_NAME="${PROG_NAME%.*}"

$CLANG -fopenmp -DUSE_MPI=0 -I$HOME/usr/lib/gcc/x86_64-unknown-linux-gnu/4.9.1/include -emit-llvm -O0 -g -c $PROG_NAME -o $PROG_NAME.bc

################
# Link Program
################
#$LLVM_LINK -o $PROG_NAME.bc $PROG_NAME.bc

###################
# Run analysis pass
###################
#$OPT -mem2reg $PROG_NAME.bc -o $PROG_NAME.opt 
#mv $PROG_NAME.opt $PROG_NAME.bc
$LLVM_DIS -f $PROG_NAME.bc -o $PROG_NAME.ll
#$OPT -S -polly-canonicalize $PROG_NAME.bc > $PROG_NAME.ll
$OPT -load $PASS -$PASS_FLAG $PROG_NAME.ll -o $PROG_NAME.bc
#$LLVM_DIS -f $PROG_NAME.bc -o $PROG_NAME.ll
$LLC $PROG_NAME.bc
$CLANG $PROG_NAME.bc -O0 -o $EXEC_NAME -L$HOME/usr/lib/intelomprt -liomp5
