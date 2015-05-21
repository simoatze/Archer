#!/bin/bash
# set -x

if [ "$(uname)" == "Linux" ]; then
    ESCAPE="\e"
else
    ESCAPE="\x1B"
fi

RED=$ESCAPE'[0;31m'
NC=$ESCAPE'[0m'
echoc() { echo -e "${RED}$1${NC}"; }

LLVM_INSTALL=/usr
HTTP=false

for i in "$@"
do
    case $i in
	--prefix=*)
	    LLVM_INSTALL="${i#*=}"
	    shift
	    ;;
	--http)
	    HTTP=true
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
if [ "$(uname)" == "Darwin" ]; then
    PROCS=$(sysctl -a | grep machdep.cpu | grep core_count | awk -F " " '{ print $2 }')
else
    if ! type "nproc" > /dev/null; then
	PROCS=$(nprocs)
    else
	PROCS=$(cat /proc/cpuinfo | awk '/^processor/{print $3}' | tail -1)
	PROCS=`expr $PROCS + 1`    
    fi
fi

echo
echoc "Installing LLVM/Clang OpenMP..."

WORKING_DIR=`pwd`
cd ..
BASE=`pwd`/LLVM
mkdir -p ${BASE}
cd $BASE

# LLVM installation directory
ARCHER_INSTALL=${LLVM_INSTALL}/local/archer
LLVM_SRC=${BASE}/llvm_src
CLANG_SRC=${BASE}/llvm_src/tools/clang
LLVMRT_SRC=${BASE}/llvm_src/projects/compiler-rt
POLLY_SRC=${LLVM_SRC}/tools/polly
POLLY_COMMIT="d24789b6ad270d80509c73fc986d1275895c5b7b"
INTELOMPRT=${BASE}/intelomprt
LLVM_BUILD=${BASE}/llvm_build
mkdir -p ${LLVM_BUILD}
LLVM_DEP=${LLVM_BUILD}/dependencies
mkdir -p ${LLVM_DEP}
CLOOG_SRC=${LLVM_DEP}/cloog_src
CLOOG_INSTALL=${LLVM_DEP}/cloog_install
# INTELOMPRT_VERSION=20131209
INTELOMPRT_VERSION=20140716
INTELOMPRT_FILE=libomp_${INTELOMPRT_VERSION}_oss.tgz

# Obtaining the sources

# LLVM Sources
echo
echoc "Obtaining LLVM OpenMP..."
if [ "$HTTP" == "true" ]; then
    git clone https://github.com/clang-omp/llvm.git ${LLVM_SRC}
else
    git clone git@github.com:clang-omp/llvm.git ${LLVM_SRC}
fi

# Clang Sources
echo
echoc "Obtaining LLVM/Clang OpenMP..."
if [ "$HTTP" == "true" ]; then
    git clone https://github.com/clang-omp/clang.git ${CLANG_SRC}
else
    git clone -b clang-omp git@github.com:clang-omp/clang.git ${CLANG_SRC}
fi

# Runtime Sources
echo
echoc "Obtaining LLVM OpenMP Runtime..."
if [ "$HTTP" == "true" ]; then
    git clone https://github.com/clang-omp/compiler-rt.git ${LLVMRT_SRC}
else
    git clone git@github.com:clang-omp/compiler-rt.git ${LLVMRT_SRC}
fi

# Polly Sources
echo
echoc "Obtaining Polly..."
if [ "$HTTP" == "true" ]; then
    git clone https://github.com/llvm-mirror/polly.git ${POLLY_SRC}
else
    git clone git@github.com:llvm-mirror/polly.git ${POLLY_SRC}
fi
cd ${POLLY_SRC}
# git checkout ${POLLY_COMMIT}
git reset --hard ${POLLY_COMMIT}

# Intel OpenMP Runtime Sources
mkdir ${INTELOMPRT}
echo
echo "Obtaining Intel OpenMP Runtime..."
wget --directory-prefix=${INTELOMPRT} https://www.openmprtl.org/sites/default/files/${INTELOMPRT_FILE}

# Applying the Patch

# LLVM Patch
echo
echoc "Patching LLVM..."
cd ${LLVM_SRC}
patch -p 1 < ${WORKING_DIR}/patch/llvm.patch

# Clang Patch
echo
echoc "Patching Clang..."
cd ${CLANG_SRC}
patch -p 1 < ${WORKING_DIR}/patch/clang.patch

# Polly Patch
echo
echoc "Patching Polly..."
cd ${POLLY_SRC}
patch -p 1 < ${WORKING_DIR}/patch/polly.patch

# Intel OpenMP Runtime Patch
echo
echoc "Patching Intel OpenMP Runtime..."
cd ${CLANG_SRC}
patch -p 1 < ${WORKING_DIR}/patch/clang-intelomprt.patch
cd ${LLVMRT_SRC}
patch -p 1 < ${WORKING_DIR}/patch/compiler-rt-intelomprt.patch
cd ${INTELOMPRT}
tar xzvf ${INTELOMPRT_FILE}
# patch -p 0 < ${WORKING_DIR}/patch/libomp_oss_${INTELOMPRT_VERSION}.patch

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
# Compiling and installing Archer Plugins
# Copying ArcherPlugin in "tools/clang/example" directory
cp -R ${WORKING_DIR}/plugins/ArcherPlugin ${LLVM_SRC}/tools/clang/examples
echo "add_subdirectory(ArcherPlugin)" >> ${LLVM_SRC}/tools/clang/examples/CMakeLists.txt
cd ${LLVM_BUILD}
export CC=gcc
export CXX=g++
cmake -D CMAKE_INSTALL_PREFIX:PATH=${LLVM_INSTALL} -D CMAKE_PREFIX_PATH=${CLOOG_INSTALL} -D LINK_POLLY_INTO_TOOLS:Bool=ON ${LLVM_SRC}
make [REQUIRES_RTTI=1]* -j${PROCS} -l${PROCS}
echoc "Building Archer Plugins..."
make ArcherPlugin
cp ${LLVM_BUILD}/lib/ArcherPlugin.so ${LLVM_INSTALL}/lib
echoc "Installing LLVM/Clang OpenMP..."
make install

# Compiling and installing Intel OpenMP Runtime
echoc "Building Intel OpenMP Runtime..."
cd ${INTELOMPRT}
cd libomp_oss/cmake
cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ..
make -j${PROCS} -l${PROCS}
cp -r ${INTELOMPRT}/libomp_oss/exports/lin_32e/lib ${LLVM_INSTALL}/lib/intelomprt
# Installing Instrumented Intel OpenMP Runtime (temporary until patch)
cp ${WORKING_DIR}/intelomprt/*.so ${LLVM_INSTALL}/lib/intelomprt

# Compiling and installing Archer
echoc "Building Archer..."
cd ${WORKING_DIR}
mkdir -p ${ARCHER_INSTALL}
cp -R ${WORKING_DIR}/bin ${ARCHER_INSTALL}
cp -R ${WORKING_DIR}/lib ${ARCHER_INSTALL}

echo
echo "In order to use LLVM/Clang, the Intel OpenMP Runtime and Archer"
echo "set the following path variables:"
echo
echoc "export PATH=${LLVM_INSTALL}/bin:${LLVM_INSTALL}/local/archer/bin:\${PATH}"
echoc "export LD_LIBRARY_PATH=${LLVM_INSTALL}/bin:${LLVM_INSTALL}/lib/intelomprt:${LLVM_INSTALL}/local/archer/lib:\${LD_LIBRARY_PATH}"
echo
echo "or add it in your \"~/.bashrc\"."
echo
echo
echoc "LLVM installation completed."
echo
