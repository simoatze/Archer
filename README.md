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
<li><a href="#sec-3">3. Configuration</a></li>
<li><a href="#sec-4">4. Usage</a></li>
<li><a href="#sec-5">5. Copyright</a></li>
</ul>
</div>
</div>


# Introduction<a id="sec-1" name="sec-1"></a>

<img src="resources/images/archer_logo.png" style="height:20%;" alt="Archer Logo" title="Archer" align="right" />

**Archer** is a data race detector for OpenMP programs.

# Installation<a id="sec-2" name="sec-2"></a>

**Archer** has been developed under LLVM 3.5 with OpenMP support (for
more information go to <http://clang-omp.github.io>).

## Building<a id="sec-2-1" name="sec-2-1"></a>

First obtain **Archer**, e.g. from Github (<https://github.com/soarlab/Archer>):

    git clone git@github.com:soarlab/Archer.git Archer

Then build **Archer** by running `install.sh`:

    ./install.sh --prefix=llvm_install_path [default: --prefix=/usr]

The installation script will create a folder called *LLVM* at the same
level of *Archer*. 

# Configuration<a id="sec-3" name="sec-3"></a>

# Usage<a id="sec-4" name="sec-4"></a>

# Copyright<a id="sec-5" name="sec-5"></a>
