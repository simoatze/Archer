#!/bin/bash
# set -x

if [ "$(uname)" == "Linux" ]; then
    RED='\e[0;31m'
    NC='\e[0m'
    echoc() { echo -e "${RED}$1${NC}"; }
fi

LLVM_INSTALL=/usr

for i in "$@"
do
    case $i in
	--prefix=*)
	    LLVM_INSTALL="${i#*=}"
	    shift
	    ;;
	*)
	    echo "Usage: ./install.sh [--prefix=PREFIX[/usr]]"
	    exit
	    ;;
    esac
done

echo
echoc "LLVM will be installed under [${LLVM_INSTALL}]"

# Get the number of cores to speed up make process
if ! type "nproc" > /dev/null; then
    PROCS=$(nprocs)
else
    PROCS=$(cat /proc/cpuinfo | awk '/^processor/{print $3}' | tail -1)
    PROCS=`expr $PROCS + 1`    
fi

echo
echoc "Installing LLVM/Clang OpenMP..."

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
POLLY_COMMIT="d24789b6ad270d80509c73fc986d1275895c5b7b"
INTELOMPRT=${BASE}/intelomprt
LLVM_BUILD=${BASE}/llvm_build
mkdir -p ${LLVM_BUILD}
LLVM_DEP=${LLVM_BUILD}/dependencies
mkdir -p ${LLVM_DEP}
CLOOG_SRC=${LLVM_DEP}/cloog_src
CLOOG_INSTALL=${LLVM_DEP}/cloog_install
INTELOMPRT_FILE=libomp_20140926_oss.tgz

# Obtaining the sources

# LLVM Sources
echo
echoc "Obtaining LLVM OpenMP..."
git clone git@github.com:clang-omp/llvm.git ${LLVM_SRC}

# Clang Sources
echo
echoc "Obtaining LLVM/Clang OpenMP..."
git clone -b clang-omp git@github.com:clang-omp/clang.git ${CLANG_SRC}

# Runtime Sources
echo
echoc "Obtaining LLVM OpenMP Runtime..."
git clone git@github.com:clang-omp/compiler-rt.git ${LLVMRT_SRC}

# Polly Sources
echo
echoc "Obtaining Polly..."
git clone git@github.com:llvm-mirror/polly.git ${POLLY_SRC}
cd ${POLLY_SRC}
git checkout ${POLLY_COMMIT}

# Intel OpenMP Runtime Sources
mkdir ${INTELOMPRT}
echo
echo "Obtaining Intel OpenMP Runtime..."
wget --directory-prefix=${INTELOMPRT} https://www.openmprtl.org/sites/default/files/${INTELOMPRT_FILE}

# Applying the Patches

# LLVM Patch
echo
echoc "Patching LLVM..."
cd ${LLVM_SRC}
patch -p 1 < ${WORKING_DIR}/patches/llvm.patch

# Clang Patch
echo
echoc "Patching Clang..."
cd ${CLANG_SRC}
patch -p 1 < ${WORKING_DIR}/patches/clang.patch

# # Polly Patch
echo
echoc "Patching Polly..."
cd ${POLLY_SRC}
patch -p 1 < ${WORKING_DIR}/patches/polly.patch

# # Intel OpenMP Runtime Patch
# echo
# echoc "Patching Intel OpenMP Runtime..."
# cd ${INTELOMPRT}
# patch -p 1 < ${WORKING_DIR}/patches/intelomprt.patch

# Compiling and installing Cloog (dependency for Polly)
echo
echoc "Building dependencies..."
${POLLY_SRC}/utils/checkout_cloog.sh ${CLOOG_SRC}
cd ${CLOOG_SRC}
./configure --prefix=${CLOOG_INSTALL}
make -j${PROCS} -l${PROCS}
make install

# Compiling and installing LLVM
echo
echoc "Building LLVM/Clang OpenMP..."
cd ${LLVM_BUILD}
export CC=gcc
export CXX=g++
cmake -D CMAKE_INSTALL_PREFIX:PATH=${LLVM_INSTALL} -D CMAKE_PREFIX_PATH=${CLOOG_INSTALL} -D LINK_POLLY_INTO_TOOLS:Bool=ON ${LLVM_SRC}
make -j${PROCS} -l${PROCS}
make install

# Compiling and installing Intel OpenMP Runtime
cd ${INTELOMPRT}
tar xzvf ${INTELOMPRT_FILE}
cd libomp_oss/cmake
cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ..
make -j${PROCS} -l${PROCS}
cd ..
cp -r exports ${LLVM_INSTALL}/lib/intelomprt

echo
echo "In order to use LLVM/Clang and the Intel OpenMP Runtime"
echo "set the following path variables:"
echo
echoc "export PATH=${LLVM_INSTALL}:\${PATH}"
echoc "export LD_LIBRARY_PATH=${LLVM_INSTALL}/lib/intelomprt/lin_32e:\${LD_LIBRARY_PATH}"
echo
echoc "or add it in your \"~/.bashrc\"."
echo
echo
echoc "LLVM installation completed."
echo
