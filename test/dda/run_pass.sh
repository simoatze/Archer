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
export PASS=../../lib/libArcher.so
export PASS_FLAG=archer
rm -rf .polly

##############
# Program name
##############
export PROG_NAME=$1
export EXEC_NAME="${PROG_NAME%.*}"

$CLANG -DUSE_MPI=0 -emit-llvm -O0 -g -c $PROG_NAME -o $PROG_NAME.bc

################
# Link Program
################
$LLVM_LINK -o $PROG_NAME.bc $PROG_NAME.bc

###################
# Run analysis pass
###################
$OPT -mem2reg $PROG_NAME.bc -o $PROG_NAME.opt 
mv $PROG_NAME.opt $PROG_NAME.bc
# $LLVM_DIS -f $PROG_NAME.bc -o $PROG_NAME.ll
$OPT -S -polly-canonicalize $PROG_NAME.bc > $PROG_NAME.ll
$OPT -load $PASS -$PASS_FLAG $PROG_NAME.ll -o $PROG_NAME-optimized.bc
$LLVM_DIS -f $PROG_NAME-optimized.bc -o $PROG_NAME-optimized.ll
$LLC $PROG_NAME-optimized.bc
$CLANG $PROG_NAME-optimized.bc -O0 -o $EXEC_NAME -L/g/g90/protze1/INSTALL/chaos_5_x86_64_ib/intel-omp/lib -liomp5
