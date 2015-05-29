#!/bin/bash
# set -e
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
LLVM_TRUNK=false

# CC and CXX
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
	--llvm-trunk)
	    LLVM_TRUNK=true
	    shift
	    ;;
	*)
	    echo "Usage: ./install.sh [--prefix=PREFIX[/usr] [--http (to use HTTP git url)] [--llvm-trunk (install the last version of LLVM/Clang, currently supports only OpenMP 3.1)]"
	    exit
	    ;;
    esac
done

echo
echoc "LLVM will be installed at [${LLVM_INSTALL}]"

# Saving installation patch
echo ${LLVM_INSTALL} > .install_path

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

# Software Repositories
LLVM_REPO=""
CLANG_REPO=""
LLVMRT_REPO=""
POLLY_REPO=""
LLVM_COMMIT=""
LLVMRT_COMMIT=""	 
CLANG_COMMIT=""
POLLY_COMMIT=""
REPO_RESET=false
if [ "$HTTP" == "true" ]; then
    if [ "$LLVM_TRUNK" == "true" ]; then
	LLVM_REPO="https://github.com/clang-omp/llvm_trunk"
	CLANG_REPO="https://github.com/clang-omp/clang_trunk"
	LLVMRT_REPO="https://github.com/clang-omp/compiler-rt_trunk"
	POLLY_REPO="https://github.com/llvm-mirror/polly.git"
    else
	LLVM_REPO="https://github.com/clang-omp/llvm.git"
	CLANG_REPO="-b clang-omp https://github.com/clang-omp/clang.git"
	LLVMRT_REPO="https://github.com/clang-omp/compiler-rt.git"
	POLLY_REPO="https://github.com/llvm-mirror/polly.git"
	REPO_RESET=true
    fi
else
    if [ "$LLVM_TRUNK" == "true" ]; then
	LLVM_REPO="git@github.com:clang-omp/llvm_trunk.git"
	CLANG_REPO="git@github.com:clang-omp/clang_trunk.git"
	LLVMRT_REPO="git@github.com:clang-omp/compiler-rt_trunk.git"
	POLLY_REPO="git@github.com:llvm-mirror/polly.git"
    else
	LLVM_REPO="git@github.com:clang-omp/llvm.git"
	CLANG_REPO="-b clang-omp git@github.com:clang-omp/clang.git"
	LLVMRT_REPO="git@github.com:clang-omp/compiler-rt.git"
	POLLY_REPO="git@github.com:llvm-mirror/polly.git"
	REPO_RESET=true
    fi
fi

# Software Version
if [ "$REPO_RESET" == "true" ]; then
    LLVM_COMMIT="e45b045553e027cbe400cbb8ac8c264abbbfaf83"
    LLVMRT_COMMIT="5ebcef4a170007700aa01e4561fc802b951e5951" 
    CLANG_COMMIT="b2fc5b326873b2f3bb919d11fe7f74981b517a79"
    POLLY_COMMIT="d24789b6ad270d80509c73fc986d1275895c5b7b"
fi

# LLVM installation directory
ARCHER_INSTALL=${LLVM_INSTALL}/local/archer
LLVM_SRC=${BASE}/llvm_src
CLANG_SRC=${BASE}/llvm_src/tools/clang
LLVMRT_SRC=${BASE}/llvm_src/projects/compiler-rt
POLLY_SRC=${LLVM_SRC}/tools/polly
INTELOMPRT=${BASE}/intelomprt
LLVM_BUILD=${BASE}/llvm_build
mkdir -p ${LLVM_BUILD}
LLVM_DEP=${LLVM_BUILD}/dependencies
mkdir -p ${LLVM_DEP}
CLOOG_SRC=${LLVM_DEP}/cloog_src
# CLOOG_INSTALL=${LLVM_DEP}/cloog_install
CLOOG_INSTALL=${LLVM_INSTALL}
INTELOMPRT_VERSION=20140716
INTELOMPRT_FILE=libomp_${INTELOMPRT_VERSION}_oss.tgz

# Obtaining the sources

# LLVM Sources
echo
echoc "Obtaining LLVM OpenMP..."
git clone ${LLVM_REPO} ${LLVM_SRC}
cd ${LLVM_SRC}
git reset --hard ${LLVM_COMMIT}

# Clang Sources
echo
echoc "Obtaining LLVM/Clang OpenMP..."
git clone ${CLANG_REPO} ${CLANG_SRC}
cd ${CLANG_SRC}
git reset --hard ${CLANG_COMMIT}

# Runtime Sources
echo
echoc "Obtaining LLVM OpenMP Runtime..."
git clone ${LLVMRT_REPO} ${LLVMRT_SRC}
cd ${LLVMRT_SRC}
git reset --hard ${LLVMRT_COMMIT}

# Polly Sources
echo
echoc "Obtaining Polly..."
git clone ${POLLY_REPO} ${POLLY_SRC}
cd ${POLLY_SRC}
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
patch -p 1 < ${WORKING_DIR}/patch/libomp_oss_${INTELOMPRT_VERSION}.patch
mv libomp_oss libomp_oss_patched
tar xzvf ${INTELOMPRT_FILE}

# Compiling and installing Cloog (dependency for Polly)
echo
echoc "Building dependencies..."
${POLLY_SRC}/utils/checkout_cloog.sh ${CLOOG_SRC}
cd ${CLOOG_SRC}
CC=$(which gcc) CXX=$(which g++) ./configure --prefix=${CLOOG_INSTALL}
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
CC=$(which gcc) CXX=$(which g++) cmake -D CMAKE_INSTALL_PREFIX:PATH=${LLVM_INSTALL} -D CMAKE_PREFIX_PATH=${CLOOG_INSTALL} -D LINK_POLLY_INTO_TOOLS:Bool=ON ${LLVM_SRC}
make [REQUIRES_RTTI=1]* -j${PROCS} -l${PROCS}
echoc "Building Archer Plugins..."
make ArcherPlugin
cp ${LLVM_BUILD}/lib/ArcherPlugin.so ${LLVM_INSTALL}/lib
echoc "Installing LLVM/Clang OpenMP..."
make install

# Compiling and installing Intel OpenMP Runtime
echoc "Building Intel OpenMP Runtime..."
cd ${INTELOMPRT}/libomp_oss
mkdir build && cd build
CC=$(which gcc) CXX=$(which g++) cmake ..
make -j${PROCS} -l${PROCS}
cp -r ${INTELOMPRT}/libomp_oss/exports/lin_32e/lib/libiomp5.* ${LLVM_INSTALL}/lib/intelomprt
cd ${INTELOMPRT}/libomp_oss_patched
mkdir build && cd build
CC=$(which gcc) CXX=$(which g++) cmake ..
make -j${PROCS} -l${PROCS}
make common
cp ${INTELOMPRT}/libomp_oss_patched/exports/lin_32e/lib/libiomp5.dbg ${LLVM_INSTALL}/lib/intelomprt/libiomp5_tsan.dbg
cp ${INTELOMPRT}/libomp_oss_patched/exports/lin_32e/lib/libiomp5.so ${LLVM_INSTALL}/lib/intelomprt/libiomp5_tsan.so
# Installing Instrumented Intel OpenMP Runtime (temporary until patch)
# cp ${WORKING_DIR}/intelomprt/*.so ${LLVM_INSTALL}/lib/intelomprt

# Compiling and installing Archer
echoc "Building Archer..."
cd ${WORKING_DIR}/src
make
mkdir -p ${ARCHER_INSTALL}
cd ${WORKING_DIR}
cp -R ${WORKING_DIR}/bin ${ARCHER_INSTALL}
cp -R ${WORKING_DIR}/lib ${ARCHER_INSTALL}
mkdir ${ARCHER_INSTALL}/include
cp ${INTELOMPRT}/libomp_oss/cmake/omp.h ${ARCHER_INSTALL}/include

echo
echo "In order to use LLVM/Clang, the Intel OpenMP Runtime and Archer"
echo "set the following path variables:"
echo
echoc "export PATH=${LLVM_INSTALL}/bin:${LLVM_INSTALL}/local/archer/bin:\${PATH}"
echoc "export LD_LIBRARY_PATH=${LLVM_INSTALL}/bin:${LLVM_INSTALL}/lib/intelomprt:${LLVM_INSTALL}/local/archer/lib:\${LD_LIBRARY_PATH}"
echo
echo "or add them previous line in your \"~/.bashrc\"."
echo
echo
echoc "LLVM installation completed."
echo
