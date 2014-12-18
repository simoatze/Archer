export BD=$HOME/compilers/llvm35omp/LLVM/llvm_build
cd $BD && make ArcherDDAParser
clang -fopenmp -O0 -g -D_GNU_SOURCE -D_DEBUG -D__STDC_CONSTANT_MACROS \
          -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -D_GNU_SOURCE \
          -I$BD/tools/clang/include -Itools/clang/include -I$BD/include -Iinclude \
          $HOME/compilers/llvm35omp/Archer/test/dda/flow-dependence-raw-loop.c -fsyntax-only \
          -Xclang -load -Xclang $BD/lib/ArcherDDAParser.so -Xclang \
          -plugin -Xclang archerddaparser
