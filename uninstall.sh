#!/bin/bash
# set -e
# set -x

echo "Uninstalling Archer..."
while read line
do
    rm -rf $line/lib/intelomprt
    rm -rf $line/local/archer
    rm -rf $line/include/omp.h
    # if [ -d "../LLVM" ]; then
    # 	if [ -d "../LLVM/llvm_build" ]; then
    # 	    cd ../LLVM/llvm_build
    # 	    make uninstall
    # 	fi
    # fi
done < .install_path
