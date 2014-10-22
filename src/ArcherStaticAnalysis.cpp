/**
 * ArcherStaticAnalysis.cpp
 *
 * Created on: Oct 22, 2014
 *     Author: Simone Atzeni
 *    Contact: simone@cs.utah.edu, atzeni1@llnl.gov
 *
 * ArcherStaticAnalysis - This is a module pass that applies
 * different Analysis pass to find data race free or racy
 * code regions.
 * The ouput of this pass is a file that contains blacklisting
 * information for ThreadSanitizer.
 *
 */

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/Constants.h"

#include <list>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace llvm;

namespace {

  template <typename T>
  std::string NumberToString ( T Number )
  {
    std::ostringstream ss;
    ss << Number;
    return ss.str();
  }

  class ArcherStaticAnalysis : public ModulePass
  {
  public:
    static char ID;

    ArcherStaticAnalysis() : ModulePass(ID) {}

    virtual void getAnalysisUsage(AnalysisUsage &AU) const
    {
    }

    virtual bool runOnModule(Module &M)
    {
      return true;
    }
  };

}

char ArcherStaticAnalysis::ID = 0;
static RegisterPass<ArcherStaticAnalysis> X("archer", "Archer Statis Analysis for ThreadSanitizer blacklist generation", false, false);
