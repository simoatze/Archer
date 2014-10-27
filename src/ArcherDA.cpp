/**
 * ArcherDA.cpp
 *
 * Created on: Oct 22, 2014
 *     Author: Simone Atzeni
 *    Contact: simone@cs.utah.edu, atzeni1@llnl.gov
 *
 * ArcherDA - This is a module pass that applies
 * different Analysis pass to find data race free or racy
 * code regions.
 * The ouput of this pass is a file that contains blacklisting
 * information for ThreadSanitizer.
 *
 */

#include "llvm/DebugInfo.h"
#include "llvm/Pass.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_ostream.h"
#include "polly/Config/config.h"
#include "polly/Dependences.h"
#include "polly/LinkAllPasses.h"
#include "polly/Options.h"
#include "polly/ScopInfo.h"
#include "polly/ScopPass.h"
  
#include "isl/aff.h"
#include "isl/list.h"
#include "isl/map.h"
#include "isl/set.h"
#include "isl/schedule.h"
#include "isl/union_map.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace llvm;
using namespace polly;

#define DEBUG_TYPE "archer"

namespace {

  template <typename T>
  std::string NumberToString ( T Number )
  {
    std::ostringstream ss;
    ss << Number;
    return ss.str();
  }

  class ArcherDA : public ScopPass
  {
  public:
    static char ID;

    ArcherDA() : ScopPass(ID) {}

    void getAnalysisUsage(AnalysisUsage &AU) const
    {
      ScopPass::getAnalysisUsage(AU);
      AU.addRequired<Dependences>();
    }

    std::string getBlacklistScop(polly::Scop &Scop) {
      polly::Scop *S;
      std::string content;

      S = &Scop;
      std::string FunctionName = S->getRegion().getEntry()->getParent()->getName();

      content = "# function:" + FunctionName;
      content += "\n";

      for (Scop::iterator SI = S->begin(), SE = S->end(); SI != SE; ++SI) {
	ScopStmt *Stmt = *SI;
    
	BasicBlock *BB = Stmt->getBasicBlock();
	for (BasicBlock::iterator BI = BB->begin(), BE = BB->end();
	     BI != BE; ++BI) {
      
	  if (MDNode *N = BI->getMetadata("dbg")) {
	    DILocation Loc(N);                      
	    unsigned Line = Loc.getLineNumber();
	    StringRef File = Loc.getFilename();
	    StringRef Dir = Loc.getDirectory();

	    std::string str;
	    llvm::raw_string_ostream rso(str);
	    BI->print(rso);

	    std::string string = "line:" + std::to_string(Line) + "," + File.str() + "," + Dir.str() + "\n";
	    if(content.find(string) == std::string::npos) {
	      content += string;
	      DEBUG(dbgs() << content << "\n");
	    }
	    DEBUG(dbgs() << File << " - Ignoring Line[" << Line << "]\n");
	  }
	}
      }

      return(content);
    }

    void releaseMemory() {
      
    }

    bool runOnScop(Scop &Scop)
    {
      printf("Entro qui\n\n");
      //polly::Scop *S = &Scop;
      Dependences *D = &getAnalysis<Dependences>();

      // __isl_keep isl_union_map *RAW = D->getDependences(Dependences::TYPE_RAW);
      // __isl_keep isl_union_map *WAR = D->getDependences(Dependences::TYPE_WAR);
      // __isl_keep isl_union_map *WAW = D->getDependences(Dependences::TYPE_WAW);
      // __isl_keep isl_union_map *RED = D->getDependences(Dependences::TYPE_RED);
      // __isl_keep isl_union_map *TCRED = D->getDependences(Dependences::TYPE_TC_RED);

      // if ((RAW != nullptr) || (WAR != nullptr) || (WAW != nullptr) || (RED != nullptr) || (TCRED != nullptr)) {
      // 	DEBUG(dbgs() << "Exiting...\n");
      // 	isl_union_map_free(RAW);
      // 	isl_union_map_free(WAR);
      // 	isl_union_map_free(WAW);
      // 	isl_union_map_free(RED);
      // 	isl_union_map_free(TCRED);
      // 	return false;
      // }

      std::string FileName = "tsan_blacklist.txt"; //ImportDir + "/" + getFileName(S);

      std::string fileContent = getBlacklistScop(Scop);

      dbgs() << "Dependences:\n";
      D->printScop(dbgs());

      std::string ErrInfo;
      tool_output_file F(FileName.c_str(), ErrInfo, llvm::sys::fs::F_Append);

      if (ErrInfo.empty()) {
	F.os() << fileContent;
	F.os().close();
	if (!F.os().has_error()) {
	  errs() << "\n";
	  F.keep();
	  return false;
	}
      } else {    
	errs() << "  error opening file for writing!\n";
	F.os().clear_error();
	return false;
      }
      return true;
    }
  };

}

char ArcherDA::ID = 0;
static RegisterPass<ArcherDA> X("archer", "Archer Data Dependency Analysis for ThreadSanitizer blacklist generation", false, false);
