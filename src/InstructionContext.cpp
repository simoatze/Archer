/**
 * InstructionContext.h
 *
 * Created on: Feb 28, 2015
 *     Author: Simone Atzeni
 *    Contact: ilaguna@llnl.gov, simone@cs.utah.edu
 *
 *
 */

#include "InstructionContext.h"

bool InstructionContext::writeFile(std::string modulename, std::string ext, std::string &content)
{
  std::pair<StringRef, StringRef> filename = StringRef(modulename).rsplit('.');
  std::string FileName;
  FileName = dir + "/" + filename.first.str() + ext;
  content = "# Module:" + filename.first.str() + "\n" + content;
  
  std::string ErrInfo;
  tool_output_file F(FileName.c_str(), ErrInfo, llvm::sys::fs::F_Append);
  
  if (ErrInfo.empty()) {
    F.os() << content;
    F.os().close();
    if (!F.os().has_error()) {
      F.keep();
    }
  } else {    
    errs() << "error opening file for writing!\n";
    F.os().clear_error();
    return false;
  }
  
  return true;
}

void InstructionContext::getAnalysisUsage(AnalysisUsage &AU) const
{
  AU.setPreservesAll();
  AU.addRequired<CallGraphWrapperPass>();
}

/// Attempts to instrument the given instructions. It instruments "only"
/// instructions in which we can find debugging meta-data to obtain line
/// number information. Returns the number of instrumented instructions.
void InstructionContext::visitCallGraph(Instruction *I, Function *F, Module *M, CallGraph &CG)
{
  if(F->getName().find("omp_microtask") != std::string::npos) {
    getInfoInstruction(I, F, M, true);
    return;
  } else if(F->getName().find("main") != std::string::npos) {
    getInfoInstruction(I, F, M, false);
    return;
  }
  
  // Get the call graph node for the function containing the call.
  CallGraphNode *CGN = CG[F];
  
  errs() << "Visiting CallGraph for Function: " << F->getName() << " and Module: " << M->getModuleIdentifier() << "\n";
  
  // Wrong this is the list of called function
  for (CallGraphNode::iterator ti = CGN->begin(); ti != CGN->end(); ++ti) {
    errs() << "For: Visiting CallGraph for Function: " << F->getName() << " and Module: " << M->getModuleIdentifier() << "\n";
    Function *f = ti->second->getFunction();
    if(f != NULL) {
      getInfoInstruction(I, f, M, true);
      
      visitCallGraph(I, f, M, CG);
    }
  }
}

void InstructionContext::getInfoInstruction(Instruction *I, Function *F, Module *M, bool isParallel)
{
  MDNode *N = I->getMetadata("dbg");
  if (N) {
    DILocation Loc(N);
    unsigned Line = Loc.getLineNumber();
    StringRef File = Loc.getFilename();
    StringRef Dir = Loc.getDirectory();
    
    if(isParallel)
      errs() << "Parallel Instruction at line " << Line << " in Module " << M->getModuleIdentifier() << " in File: " << File <<"\n";
    else
      errs() << "Sequential Instruction at line: " << Line << " in Module " << M->getModuleIdentifier() << "\n";
    
    //std::pair<StringRef, StringRef> filename = StringRef(M->getModuleIdentifier()).rsplit('.');
    // if(File.compare(filename.first) == 0) {
    // if(true) {
    // 	val = NumberToString<unsigned>(Line) + "," + FunctionName + "," + File.str() + "," + Dir.str() + "\n";
    // 	errs() << val << "\n";
    // 	if(content->find(val) == std::string::npos) {
    // 	  *content += val;	
    // 	}
    // }
  }
}

void InstructionContext::getInfoInstruction(Instruction *I, std::string val)
{
  MDNode *N = I->getMetadata("dbg");
  
  if (N) {
    DILocation Loc(N);
    unsigned Line = Loc.getLineNumber();
    StringRef File = Loc.getFilename();
    StringRef Dir = Loc.getDirectory();
    
    errs() << val << " Instruction at line " << Line << " in Function " << I->getParent()->getParent()->getName() << " in File: " << File <<"\n";
  }
}

bool InstructionContext::runOnModule(Module &M)
{
  std::string ModuleName = M.getModuleIdentifier();
  
  // errs() << "Module Name: " << ModuleName << "\n";
  
  // For each Function in the Module
  for (auto F = M.begin(), i = M.end(); F != i; ++F) {
    
    // errs() << "Function Name: " << F->getName() << "\n";

    // Ignore function declaration
    if (F->isDeclaration())
	 continue;
   
    // For Each BasicBlock in the 
    for (auto BB = F->begin(), ii = F->end(); BB != ii; ++BB) {
      for (auto I = BB->begin(), iii = BB->end(); I != iii; ++I) {
	Instruction *Inst = &(*I);
	
	//If the function is an ".omp_microtask."
	if(F->getName().find(".omp_microtask.") != std::string::npos) {
	  // If the instruction is a Load/Store
	  // add to the list of Parallel Instructions
	  if (isa<LoadInst>(Inst) || isa<StoreInst>(Inst)) {
	    parallel_instructions.insert(Inst);
	    errs() << "Parallel Load/Store Address: " << Inst << " from Function: " << F->getName() << "\n";
	    // errs() << "START - ###################\n";
	    // getInfoInstruction(Inst, "Parallel Load/StoreSIMONE");
	    // std::string str;
	    // llvm::raw_string_ostream rso(str);
	    // I->print(errs());
	    // errs() << "\nEND - ###################\n";
	  }

	  // If the instruction is a function call
	  // And is not in the list of visited function
	  // Insert Function in Queue of parallel functions
	  if(isa<CallInst>(Inst)) {
	    Function *f = cast<CallInst>(Inst)->getCalledFunction();

	    if(f == NULL) {
	      Value* c = cast<CallInst>(Inst)->getCalledValue();
	      
	      // special case that happens when some clang warnings are reported
	      // we can find out the called function exactly
	      if (ConstantExpr* ce = dyn_cast<ConstantExpr>(c)) {
		if (ce->isCast()) {
		  Value* castValue = ce->getOperand(0);
		  f = dyn_cast<Function>(castValue);
		}
	      }
	    }

	    // Insert function call in queue of parallel functions
	    if(f != NULL) {
	      if(visited_functions.find(f) == visited_functions.end()) {
	      	parallel_functions.push(f);
	      }
	    }
	  }
	} else {
	  if (isa<LoadInst>(Inst) || isa<StoreInst>(Inst)) {
	    getInfoInstruction(Inst, "Sequential Load/Store");
	    sequential_instructions.insert(Inst);
	    errs() << "Sequential Load/Store Address: " << Inst << " from Function: " << F->getName() << "\n";
	  }
	}
      }
    }
  }

  errs() << "Analyzing Queue...\n";
  
  // For each function in the Queue of Parallel Functions
  while (!parallel_functions.empty()) {
    Function *F = (Function *) parallel_functions.front();
    visited_functions.insert(F);
    
    // Visit Instructions
    errs() << "Analyzing function: " << F->getName() << "\n";
    for (auto BB = F->begin(), ii = F->end(); BB != ii; ++BB) {
      for (auto I = BB->begin(), iii = BB->end(); I != iii; ++I) {
	Instruction *Inst = &(*I);
	
	// If the instruction is a Load/Store
	// add to the list of Parallel Instructions
	if (isa<LoadInst>(Inst) || isa<StoreInst>(Inst)) {
	  getInfoInstruction(Inst, "Load/Store");
	  std::set<Instruction *>::iterator it = sequential_instructions.find(Inst);
	  parallel_instructions.insert(Inst);
	  if(it == sequential_instructions.end()) {
	    parallel_instructions.insert(Inst);
	  } else {
	    parallel_instructions.insert(Inst);
	    sequential_instructions.erase(it);
	  }
	  errs() << "Parallel Load/Store Address: " << Inst << " from Function: " << F->getName() << "\n";
	}
	
	// If the instruction is a function call
	// And is not in the list of visited function
	// Insert Function in Queue of parallel functions
	if(isa<CallInst>(Inst)) {
	  getInfoInstruction(Inst, "Call");
	  Function *f = cast<CallInst>(Inst)->getCalledFunction();
	  
	  if(f == NULL) {
	    Value* c = cast<CallInst>(Inst)->getCalledValue();
	    
	    // special case that happens when some clang warnings are reported
	    // we can find out the called function exactly
	    if (ConstantExpr* ce = dyn_cast<ConstantExpr>(c)) {
	      if (ce->isCast()) {
		Value* castValue = ce->getOperand(0);
		f = dyn_cast<Function>(castValue);
	      }
	    }
	  }
	  if(f != NULL) {
	    if(visited_functions.find(f) == visited_functions.end()) {
	      errs() << "Inserting function call: " << f->getName() << " from function: " << F->getName() << "\n";
	      parallel_functions.push(f);
	    }
	  }
	}
      }
    }

    parallel_functions.pop();
  }

  errs() << "Parallel Instructions\n";

  for(auto Inst : parallel_instructions) {
    getInfoInstruction(Inst, "Parallel");
  }

  errs() << "\n";
  errs() << "Sequential Instructions\n";

  for(auto Inst : sequential_instructions) {
    getInfoInstruction(Inst, "Sequential");
  }

  errs() << "\n";
  errs() << "SequentialParallel Instructions\n";

  for(auto Inst : seqpar_instructions) {
    getInfoInstruction(Inst, "SequentialParallel");
  }    
    
  // CallGraph &CG = getAnalysis<CallGraphWrapperPass>().getCallGraph();
    
  // for (auto F = M.begin(), i = M.end(); F != i; ++F) {
  //   // Discard function declarations
  //   if (F->isDeclaration())
  //    	continue;
	
  //   for (auto BB = F->begin(), ii = F->end(); BB != ii; ++BB) {
  // 	for (auto I = BB->begin(), iii = BB->end(); I != iii; ++I) {
  // 	  Instruction *Inst = &(*I);

	  
  // 	  if (isa<LoadInst>(Inst) || isa<StoreInst>(Inst)) {
  // 	    getInfoInstruction(Inst, F, &M, true);
  // 	    if(isa<CallInst>(Inst)) {
  // 	      if(cast<CallInst>(Inst)->getCalledFunction() != NULL) {
  // 	      }
  // 	    }
  // 	   }
  // 	}
  //   }
  // }

  // std::string ModuleName = M.getModuleIdentifier();

  // writeFile(ModuleName, LS_LINES, ls_content);
  // writeFile(ModuleName, FC_LINES, fc_content);

  return true;
}

// errs() << "Printing instruction\n";
// std::string str;
// llvm::raw_string_ostream rso(str);
// I->print(errs());
