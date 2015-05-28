<div id="table-of-contents">
<h2>Table of Contents</h2>
<div id="text-table-of-contents">
<ul>
<li><a href="#sec-1">1. Introduction</a></li>
<li><a href="#sec-2">2. Installation</a>
<ul>
<li><a href="#sec-2-1">2.1. Building</a></li>
</ul>
</li>
<li><a href="#sec-3">3. Usage</a>
<ul>
<li><a href="#sec-3-1">3.1. Archer Options</a></li>
</ul>
</li>
<li><a href="#sec-4">4. FAQ</a></li>
<li><a href="#sec-5">5. Copyright</a></li>
</ul>
</div>
</div>


# Introduction<a id="sec-1" name="sec-1"></a>

<img src="resources/images/archer_logo.png" hspace="5" vspace="5" height="45%" width="45%" alt="Archer Logo" title="Archer" align="right" />

**Archer** is a data race detector for OpenMP programs.


Archer combines static and dynamic techniques to
identify data races in large OpenMP applications, generating low
runtime and memory overhead, while still offering high accuracy and
precision. It builds on the open-source infrastructures LLVM and
ThreadSanitizer to provide portability.

# Installation<a id="sec-2" name="sec-2"></a>

Archer has been developed under LLVM 3.5 with OpenMP support (for
more information go to <http://clang-omp.github.io>).

## Building<a id="sec-2-1" name="sec-2-1"></a>

First obtain Archer, e.g. from Github (<https://github.com/soarlab/Archer>):

    git clone git@github.com:soarlab/Archer.git Archer

Then build Archer by running `install.sh`:

    ./install.sh --prefix=llvm_install_path [default: --prefix=/usr]

The installation script will create a folder called *LLVM* at the same
level of Archer.

# Usage<a id="sec-3" name="sec-3"></a>

## Archer Options<a id="sec-3-1" name="sec-3-1"></a>

usage: clang-archer [-h] [-v] [-d] [&#x2013;log] [-db] [-CC [CC]] [-USE<sub>MPI]</sub>
                    [-MPICC [MPICC]] [-OPT [OPT]] [-LINK [LINK]] [-DIS [DIS]]
                    [-LIB [LIB]] [-PLUGIN<sub>LIB</sub> [PLUGIN<sub>LIB]]</sub>
                    [-OPENMP<sub>INCLUDE</sub> [OPENMP<sub>INCLUDE]]</sub> [-g]
                    [-O0 | -O1 | -O2 | -O3 | -Os | -Oz] [-fopenmp] [-liomp5]
                    [-c] [-o [O]]

Compile your program with Archer support, a data race detector for OpenMP programs.

optional arguments:
  -h, &#x2013;help            show this help message and exit
  -v, &#x2013;version         show program's version number and exit
  -d, &#x2013;debug           Print the compiling commands
  &#x2013;log                 Keep intermediate logs
  -db, &#x2013;disable-blacklisting
                        Disable static analysis and apply ThreadSanitizer
                        instrumentation to the entire program
  -CC [CC]              Change the program used to compile and link the
                        programs
  -USE<sub>MPI</sub>              Link against MPI libraries
  -MPICC [MPICC]        Change the program used to compile and link the MPI
                        programs
  -OPT [OPT]            Change the program used to optmize the programs
  -LINK [LINK]          Change the program used to link the byte code files
  -DIS [DIS]            Change the program used to disassemble the byte code
                        files
  -LIB [LIB]            Set the path where to find Archer libraries
  -PLUGIN<sub>LIB</sub> [PLUGIN<sub>LIB]</sub>
                        Set the path where to find Archer Plugin libraries
  -OPENMP<sub>INCLUDE</sub> [OPENMP<sub>INCLUDE]</sub>
                        Set the path where to find OpenMP headers
  -g                    If the debugging flag is not present in the
                        compilation command it will be added by default
  -O0                   The optimization flags will be forced to '-O0'
                        optimization level for analysis purposes
  -O1
  -O2
  -O3
  -Os
  -Oz
  -fopenmp              OpenMP flag
  -liomp5               OpenMP library
  -c                    Only run preprocess, compile, and assemble steps
  -o [O]                Output filename

# FAQ<a id="sec-4" name="sec-4"></a>

# Copyright<a id="sec-5" name="sec-5"></a>

<img src="resources/images/uofu_logo.png" hspace="5" vspace="5" height="45%" width="45%" alt="UofU Logo" title="University of Utah" align="left" />

<img src="resources/images/llnl_logo.png" hspace="5" vspace="5" height="45%" width="45%" alt="LLNL Logo" title="Lawrence Livermore National Laboratory" align="right" />