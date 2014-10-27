#!/bin/bash
# set -x

# Get the number of cores to speed up make process
if ! type "nproc" > /dev/null; then
    PROCS=$(nprocs)
else
    PROCS=$(cat /proc/cpuinfo | awk '/^processor/{print $3}' | tail -1)
    PROCS=`expr $PROCS + 1`    
fi

WORKING_DIR=`pwd`

cd ..

# LLVM installation directory
LLVM_INSTALL=${HOME}/usr # --prefix
BASE=`pwd`
LLVM_SRC=${BASE}/llvm_src
CLANG_SRC=${BASE}/llvm_src/tools/clang
LLVMRT_SRC=${BASE}/llvm_src/project/compiler-rt
POLLY_SRC=${LLVM_SRC}/tools/polly
CLOOG_SRC=${LLVM_DEP}/cloog_src
INTELOMPRT=${BASE}/intelomprt
LLVM_BUILD=${BASE}/llvm_build
LLVM_DEP=${LLVM_BUILD}/dependencies
CLOOG_INSTALL=${INSTALL_DIR}/usr

# Obtaining the sources

# LLVM Sources
echo "Obtaining LLVM OpenMP..."
git clone https://github.com/clang-omp/llvm ${LLVM_SRC}

# Clang Sources
echo "Obtaining LLVM/Clang OpenMP..."
git clone -b clang-omp https://github.com/clang-omp/clang llvm/tools/clang ${CLANG_SRC}

# Runtime Sources
echo "Obtaining LLVM OpenMP Runtime..."
git clone https://github.com/clang-omp/compiler-rt llvm/projects/compiler-rt ${LLVMRT_SRC}

# Polly Sources
echo "Obtaining Polly..."
git clone http://llvm.org/git/polly.git ${POLLY_SRC}

# Intel OpenMP Runtime Sources
# echo "Obtaining Intel OpenMP Runtime..."
# wget https://www.openmprtl.org/sites/default/files/libomp_20140926_oss.tgz

# Applying the Patches

# LLVM Patch
cd ${LLVM_SRC}
patch -p 1 < ../Archer/patches/llvm.patch

# Clang Patch
cd ${CLANG_SRC}
patch -p 1 < ../Archer/patches/clang.patch

# # Polly Patch
# cd ${POLLY_SRC}
# patch -p 1 < ../Archer/patches/polly.patch

# # Intel OpenMP Runtime Patch
# cd ${INTELOMPRT}
# # patch -p 1 < ../Archer/patches/intelomprt.patch

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
