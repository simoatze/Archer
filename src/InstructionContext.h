/**
 * InstructionContext.h
 *
 * Created on: Feb 28, 2015
 *     Author: Simone Atzeni
 *    Contact: ilaguna@llnl.gov, simone@cs.utah.edu
 *
 *
 */

#include "llvm/Pass.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ToolOutputFile.h"

#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "Common.h"
#include "CommonLLVM.h"

using namespace llvm;

#define DEBUG_TYPE "archer-memoryusage"

namespace {

  class InstructionContext : public ModulePass
  {
  private: 
    /* 
       - Queue of Parallel Functions
       - List of visited functions
       - List of Parallel Instructions
       - List of (Supposed) Sequential Instructions (this list is subject to
       change since if I am visiting a parallel function that calls
       the function that contains these instructions I have to move
       them from the List of Sequential Instruction and to the List
       of parallel instructions)
       - List of Instruction executed both in parallel and sequentially
       (ex.: when a function is called in different places within a
       parallel region and not)
    */

    /**
     * Queue of parallel functions
     */
    std::queue<Value *> parallel_functions;

    /**
     * List of Visited Functions 
     */
    std::set<Value *> visited_functions;

    /**
     * List of Parallel Instructions
     */
    std::set<Instruction *> parallel_instructions;

    /**
     * List of (Supposed) Sequential Instructions
     */
    std::set<Instruction *> sequential_instructions;

    /**
     * List of Instruction executed both in parallel and sequentially
     */
    std::set<Instruction *> seqpar_instructions;

    std::string ls_content;
    std::string fc_content;
    std::string dir;
  
    bool writeFile(std::string modulename, std::string ext, std::string &content);

  public:
    static char ID;
  
  InstructionContext() : ModulePass(ID) {
      ls_content = "";
      fc_content = "";
      dir = ".blacklists";

      if(llvm::sys::fs::create_directory(Twine(dir))) {
	llvm::errs() << "Unable to create \"" << dir << "\" directory.\n";
	exit(-1);
      }
    }

    virtual void getAnalysisUsage(AnalysisUsage &AU) const;

    /// Attempts to instrument the given instructions. It instruments "only"
    /// instructions in which we can find debugging meta-data to obtain line
    /// number information. Returns the number of instrumented instructions.
    void visitCallGraph(Instruction *I, Function *F, Module *M, CallGraph &CG);

    void getInfoInstruction(Instruction *I, Function *F, Module *M, bool isParallel);
    void getInfoInstruction(Instruction *I, std::string val);
  
    virtual bool runOnModule(Module &M);
  
  };

}

char InstructionContext::ID = 0;
static RegisterPass<InstructionContext> X("archer-instcontext", "Archer Instruction Context, returns the list of instructions executed sequentially and the list of instruction executed inside an OpenMP parallel region.", false, false);
