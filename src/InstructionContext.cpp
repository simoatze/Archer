/**
 * InstructionContext.cpp
 *
 * Created on: Feb 28, 2015
 *     Author: Simone Atzeni
 *    Contact: simone@cs.utah.edu
 *
 *
 */

#include "InstructionContext.h"

bool InstructionContext::writeFile(std::string header, std::string filename, std::string ext, std::string &content)
{
  content = header + "\n" + content;
  
  std::string ErrInfo;
  std::string FileName = filename + ext;
  tool_output_file F(FileName.c_str(), ErrInfo, llvm::sys::fs::F_RW);
  
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
  // AU.setPreservesAll();
  // AU.addRequired<CallGraphWrapperPass>();
}

std::string InstructionContext::getInfoInstruction(Instruction *I, std::string header)
{
  std::string str;
  
  MDNode *N = I->getMetadata("dbg");
  if (N) {
    DILocation Loc(N);
    unsigned Line = Loc.getLineNumber();
    StringRef File = Loc.getFilename();
    StringRef Dir = Loc.getDirectory();
    
    str = NumberToString<unsigned>(Line) + "," + I->getParent()->getParent()->getName().str() + "," + File.str() + "," + Dir.str();
    
    // errs() << header << str << "\n";
    
    return str;
  }
 
  // errs() << "No debugging information found. Make sure you are compiling with \"-g\" flag.\n";
  return "";
  // exit(-1);
}

std::string InstructionContext::getInfoFunction(Function *F, std::string header)
{
  std::string str;

  for (auto BB = F->begin(), ii = F->end(); BB != ii; ++BB) {
    for (auto I = BB->begin(), iii = BB->end(); I != iii; ++I) {
      MDNode *N = I->getMetadata("dbg");
      
      if (N) {
	DILocation Loc(N);
	StringRef File = Loc.getFilename();
	StringRef Dir = Loc.getDirectory();
	
	str = F->getName().str() + "," + File.str() + "," + Dir.str();
	
	// errs() << header << str << "\n";
	
	return str;
      }
    }
  }

  // errs() << "No debugging information found. Make sure you are compiling with \"-g\" flag.\n";
  return "";
  // exit(-1);
}

bool InstructionContext::runOnModule(Module &M)
{
  std::string content;
  std::string str;
  //std::string ModuleName = M.getModuleIdentifier();  

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
	    // errs() << "Parallel Load/Store Address: " << Inst << " from Function: " << F->getName() << "\n";
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
	    // insert instruction in list of sequential instructions
	    // only if it is not already in the list of
	    // parallel instructions
	    // This check here is probably not needed
	    std::set<Instruction *>::iterator it = parallel_instructions.find(Inst);
	    if(it == parallel_instructions.end()) {
	      sequential_instructions.insert(Inst);
	    }
	  }
	}
      }
    }
  }

  // errs() << "Analyzing Queue...\n";
  
  // For each function in the Queue of Parallel Functions
  while (!parallel_functions.empty()) {
    Function *F = (Function *) parallel_functions.front();
    visited_functions.insert(F);
    
    // Visit Instructions
    // errs() << "Analyzing function: " << F->getName() << "\n";
    for (auto BB = F->begin(), ii = F->end(); BB != ii; ++BB) {
      for (auto I = BB->begin(), iii = BB->end(); I != iii; ++I) {
	Instruction *Inst = &(*I);
	
	// If the instruction is a Load/Store
	// add to the list of Parallel Instructions
	if (isa<LoadInst>(Inst) || isa<StoreInst>(Inst)) {
	  parallel_instructions.insert(Inst);
	  std::set<Instruction *>::iterator it = sequential_instructions.find(Inst);
	  if(it != sequential_instructions.end()) {
	    sequential_instructions.erase(it);
	  }
	  // errs() << "Parallel Load/Store Address: " << Inst << " from Function: " << F->getName() << "\n";
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
	  if(f != NULL) {
	    if(visited_functions.find(f) == visited_functions.end()) {
	      // errs() << "Inserting function call: " << f->getName() << " from function: " << F->getName() << "\n";
	      parallel_functions.push(f);
	    }
	  }
	}
      }
    }

    parallel_functions.pop();
  }

  std::string filename = dir + "/" + StringRef(M.getModuleIdentifier()).rsplit('.').first.str();

  content = "";
  errs() << "Generating Parallel Instructions file...\n";
  for(auto Inst : parallel_instructions) {
    str = getInfoInstruction(Inst, "Parallel Instruction: ");
    if(!str.empty() && content.find(str) == std::string::npos) {
      content += str + "\n";
    }
  }
  writeFile("# Parallel Instructions", filename, PI_LINES, content);

  content = "";
  errs() << "Generating Sequential Instructions file...\n";
  for(auto Inst : sequential_instructions) {
    str = getInfoInstruction(Inst, "Sequential Instruction: ");
    if(!str.empty() && content.find(str) == std::string::npos) {
      content += str + "\n";
    }
  }
  writeFile("# Sequential Instructions", filename, SI_LINES, content);

  content = "";
  errs() << "Generating Parallel Functions file...\n";
  for(auto Func : visited_functions) {
    str = getInfoFunction(Func, "Parallel Function: ");
    if(!str.empty() && content.find(str) == std::string::npos) {
      content += str + "\n";
    }
  }
  writeFile("# Parallel Functions", filename, PF_LINES, content);

  return true;
}

// errs() << "Printing instruction\n";
// std::string str;
// llvm::raw_string_ostream rso(str);
// I->print(errs());
