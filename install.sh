#!/bin/bash
# set -x

# Get the number of cores to speed up make process
if ! type "nproc" > /dev/null; then
    PROCS=$(nprocs)
else
    PROCS=$(cat /proc/cpuinfo | awk '/^processor/{print $3}' | tail -1)
    PROCS=`expr $PROCS + 1`    
fi

echo "Installing LLVM/Clang OpenMP..."
echo

WORKING_DIR=`pwd`
cd ..
BASE=`pwd`/LLVM
mkdir -p ${BASE}
cd $BASE

# LLVM installation directory
LLVM_SRC=${BASE}/llvm_src
CLANG_SRC=${BASE}/llvm_src/tools/clang
LLVMRT_SRC=${BASE}/llvm_src/project/compiler-rt
POLLY_SRC=${LLVM_SRC}/tools/polly
POLLY_COMMIT="830c9e06589cbeda499609bd4ac227905a4562e8"
INTELOMPRT=${BASE}/intelomprt
LLVM_BUILD=${BASE}/llvm_build
mkdir -p ${LLVM_BUILD}
LLVM_DEP=${LLVM_BUILD}/dependencies
mkdir -p ${LLVM_DEP}
CLOOG_SRC=${LLVM_DEP}/cloog_src
LLVM_INSTALL=${HOME}/usr # --prefix
CLOOG_INSTALL=${LLVM_INSTALL}

# Obtaining the sources

# LLVM Sources
echo "Obtaining LLVM OpenMP..."
git clone git@github.com:clang-omp/llvm.git ${LLVM_SRC}

# Clang Sources
echo "Obtaining LLVM/Clang OpenMP..."
git clone -b clang-omp git@github.com:clang-omp/clang.git ${CLANG_SRC}

# Runtime Sources
echo "Obtaining LLVM OpenMP Runtime..."
git clone git@github.com:clang-omp/compiler-rt.git ${LLVMRT_SRC}

# Polly Sources
echo "Obtaining Polly..."
git clone git@github.com:llvm-mirror/polly.git ${POLLY_SRC}
git reset --hard ${POLLY_COMMIT}

# Intel OpenMP Runtime Sources
# echo "Obtaining Intel OpenMP Runtime..."
# wget https://www.openmprtl.org/sites/default/files/libomp_20140926_oss.tgz

# Applying the Patches

# LLVM Patch
echo "Patching LLVM..."
cd ${LLVM_SRC}
patch -p 1 < ${WORKING_DIR}/patches/llvm.patch

# Clang Patch
echo "Patching Clang..."
cd ${CLANG_SRC}
patch -p 1 < ${WORKING_DIR}/patches/clang.patch

# # Polly Patch
# cd ${POLLY_SRC}
# patch -p 1 < ${WORKING_DIR}/patches/polly.patch

# # Intel OpenMP Runtime Patch
# cd ${INTELOMPRT}
# # patch -p 1 < ${WORKING_DIR}/patches/intelomprt.patch

# Compiling and installing Cloog (dependency for Polly)
echo "Building dependencies..."
${POLLY_SRC}/utils/checkout_cloog.sh ${CLOOG_SRC}
cd ${CLOOG_SRC}
./configure --prefix=${CLOOG_INSTALL}
make -j${PROCS} -l${PROCS}
make install

# Compiling and installing LLVM
echo "Building LLVM/Clang OpenMP..."
cd ${LLVM_BUILD}
export CC=gcc
export CXX=g++
cmake -D CMAKE_INSTALL_PREFIX:PATH=${LLVM_INSTALL} -D CMAKE_PREFIX_PATH=${CLOOG_INSTALL} -D LINK_POLLY_INTO_TOOLS:Bool=ON ${LLVM_SRC}
make -j${PROCS} -l${PROCS}
make install

echo
echo "LLVM installation completed."
