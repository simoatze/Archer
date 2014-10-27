#!/bin/bash
# set -x

# Get the number of cores to speed up make process
if ! type "nproc" > /dev/null; then
    PROCS=$(nprocs)
else
    PROCS=$(cat /proc/cpuinfo | awk '/^processor/{print $3}' | tail -1)
    PROCS=`expr $PROCS + 1`    
fi

LLVM_SRC=`pwd`
cd ..
INSTALL_DIR=${HOME}
BASE=`pwd`
LLVM_BUILD=${BASE}/llvm_build
LLVM_DEP=${LLVM_BUILD}/dependencies
POLLY_SRC=${LLVM_SRC}/tools/polly
CLOOG_SRC=${LLVM_DEP}/cloog_src
#CLOOG_INSTALL=${LLVM_DEP}/cloog_install
CLOOG_INSTALL=${INSTALL_DIR}/usr

# LLVM installation directory
LLVM_INSTALL=${INSTALL_DIR}/usr

# Compiling and installing Cloog (dependency for Polly)
${POLLY_SRC}/utils/checkout_cloog.sh ${CLOOG_SRC}
cd ${CLOOG_SRC}
./configure --prefix=${CLOOG_INSTALL}
make -j${PROCS} -l${PROCS}
make install

# Compiling and installing LLVM
mkdir ${LLVM_BUILD}
cd ${LLVM_BUILD}
export CC=gcc
export CXX=g++
cmake -D CMAKE_INSTALL_PREFIX:PATH=${LLVM_INSTALL} -D CMAKE_PREFIX_PATH=${CLOOG_INSTALL} -D LINK_POLLY_INTO_TOOLS:Bool=ON ${LLVM_SRC}
make -j${PROCS} -l${PROCS}
make install

echo
echo "LLVM installation completed."
