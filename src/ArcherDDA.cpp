/*
 * Copyright (c) 2015, Lawrence Livermore National Security, LLC.
 * 
 * Produced at the Lawrence Livermore National Laboratory
 * 
 * Written by Simone Atzeni (simone@cs.utah.edu), Ganesh Gopalakrishnan,
 * Zvonimir Rakamari\'c Dong H. Ahn, Ignacio Laguna, Martin Schulz, and
 * Gregory L. Lee
 * 
 * LLNL-CODE-676696
 * 
 * All rights reserved.
 * 
 * This file is part of Archer. For details, see
 * https://github.com/soarlab/Archer. Please also read Archer/LICENSE.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 * 
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the disclaimer (as noted below) in
 * the documentation and/or other materials provided with the
 * distribution.
 * 
 * Neither the name of the LLNS/LLNL nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE
 * LIVERMORE NATIONAL SECURITY, LLC, THE U.S. DEPARTMENT OF ENERGY OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * ArcherDDA.cpp
 *
 * Created on: Oct 22, 2014
 *     Author: Simone Atzeni
 *    Contact: simone@cs.utah.edu, atzeni1@llnl.gov
 *
 * ArcherDDA - This is a module pass that applies
 * data dependency analysis with Polly.
 * The ouput of this pass is a file that contains blacklisting
 * information for ThreadSanitizer.
 *
 */

#include "ArcherDDA.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/ToolOutputFile.h"

#include "polly/LinkAllPasses.h"
#include "polly/Options.h"
#include "polly/ScopInfo.h"
  
#include "isl/aff.h"
#include "isl/list.h"
#include "isl/map.h"
#include "isl/set.h"
#include "isl/union_map.h"

using namespace llvm;
using namespace polly;

#define DEBUG_TYPE "archer-dda"

/*
static int getSingleMap(__isl_take isl_map *map, void *user) {
  std::string content;
  
  isl_set *domain = isl_map_domain(map);
  isl_id *id = isl_set_get_tuple_id(domain);
  // isl_set *range = isl_map_range(map);
  // isl_id *id = isl_set_get_tuple_id(range);
  ScopStmt *scopStmt = (ScopStmt *) isl_id_get_user(id);

  return 0;
}
*/

void ArcherDDA::createDir(std::string dir) {
  if(llvm::sys::fs::create_directory(Twine(dir))) {
    llvm::errs() << "Unable to create \"" << dir << "\" directory.\n";
    // exit(-1);
  }
}

void ArcherDDA::getAnalysisUsage(AnalysisUsage &AU) const
{
  ScopPass::getAnalysisUsage(AU);
  AU.addRequired<Dependences>();
}

void ArcherDDA::printScop(raw_ostream &OS) const {
  Function *F = S->getRegion().getEntry()->getParent();

  OS << F->getName() << "():\n";
  S->print(OS);
}

bool ArcherDDA::getLOCInfo(polly::Scop &Scop, bool isNotDependency) {
  polly::Scop *S;
  std::string content;
  std::string FileName;

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
	FileName = File.str();
  	StringRef Dir = Loc.getDirectory();

	// llvm::dbgs() << "Line: " << Line << " - File: " << File << " - Dir: " << Dir << " - Value: " << isNotDependency << "\n";

	// std::string ModuleName = S->getRegion().getEntry()->getParent()->getParent()->getModuleIdentifier();
	// std::pair<StringRef, StringRef> filename = StringRef(ModuleName).rsplit('.');

	// llvm::dbgs() << File << " - " << filename.first << "\n";
	// if (File.compare(filename.first) == 0) {
	// std::string str;
	// llvm::raw_string_ostream rso(str);
	// BI->print(rso);
	
	// llvm::dbgs() << "line:" + std::to_string(Line) + "," + File.str() + "," + Dir.str() + "\n";
	std::string string = std::to_string(Line) + "," + File.str() + "," + Dir.str() + "\n";
	// std::string string = std::to_string(Line) + "\n";
	if(content.find(string) == std::string::npos) {
	  content += string;
	  DEBUG(llvm::dbgs() << content << "\n");
	}
	DEBUG(llvm::dbgs() << File << " - Ignoring Line[" << Line << "]\n");
	//}
      }
    }
  }

  std::string path = dir + "/";
  // std::string ModuleName = S->getRegion().getEntry()->getParent()->getParent()->getModuleIdentifier();
  // std::pair<StringRef, StringRef> filename = StringRef(ModuleName).rsplit('.');
  // int pos = filename.first.str().find(".nomp.bc");
  // std::string FileName;
  // if(pos != std::string::npos) {
  //   FileName = filename.first.str().substr(0,pos);
  // } else {
  //   FileName = filename.first.str();
  // }
  if(FileName.substr(0,2).compare("./") == 0)
    FileName = FileName.substr(2);
  std::string filepath;

  if(StringRef(FileName).rsplit('/').second.str().empty()) {
    filepath = path + StringRef(FileName).rsplit('/').first.str();
    createDir(path);
  } else {
    filepath = StringRef(FileName).rsplit('/').first.str() + "/" + path + StringRef(FileName).rsplit('/').second.str();
    createDir(StringRef(FileName).rsplit('/').first.str() + "/" + path);
  }

  if(isNotDependency)
    FileName = filepath + ND_LINES;
  else
    FileName = filepath + DD_LINES;
  std::string ErrInfo;
  tool_output_file F(FileName.c_str(), ErrInfo, llvm::sys::fs::F_Append);

  if (ErrInfo.empty()) {
    F.os() << content;
    F.os().close();
    if (!F.os().has_error()) {
      F.keep();
    }
  } else {    
    errs() << "error opening file " << FileName << " for writing!\n";
    F.os().clear_error();
    return false;
  }

  return true;
}

void ArcherDDA::releaseMemory() {
      
}

bool ArcherDDA::runOnScop(Scop &Scop)
{
  bool ret = true;

  Dependences *D = &getAnalysis<Dependences>();
  //D->printScop(dbgs());
  // llvm::dbgs() << "Printing Scop...\n";
  // Scop.print(dbgs());

  /*
  isl_union_map *Deps = D->getDependences(Dependences::TYPE_RAW | Dependences::TYPE_WAW | Dependences::TYPE_WAR);
  isl_map *StmtSchedule;
  isl_union_map_foreach_map(Deps, getSingleMap, &StmtSchedule); 
  isl_union_map_free(Deps);
  if (D->hasValidDependences()) {
     errs() << "Exiting...\n";
  }
  */

  isl_union_map *Deps = D->getDependences(Dependences::TYPE_RAW | Dependences::TYPE_WAW | Dependences::TYPE_WAR);
  // isl_union_map *RAW = D->getDependences(Dependences::TYPE_RAW);
  // isl_union_map *WAW = D->getDependences(Dependences::TYPE_WAW);
  // isl_union_map *WAR = D->getDependences(Dependences::TYPE_WAR);

  // if all sets are empty there are not dependencies and the line of the scop will be blacklisted
  // if((isl_union_map_is_empty(RAW)) && (isl_union_map_is_empty(WAW)) && (isl_union_map_is_empty(WAR))) {
  ret = getLOCInfo(Scop, isl_union_map_is_empty(Deps));

  isl_union_map_free(Deps);
  // isl_union_map_free(RAW);
  // isl_union_map_free(WAW);
  // isl_union_map_free(WAR);

  return ret;
}

char ArcherDDA::ID = 0;
static RegisterPass<ArcherDDA> X("archer-dda", "Archer Data Dependency Analysis for ThreadSanitizer blacklist generation.", false, false);

/*
bool ArcherDDA::initializeFiles(polly::Scop &S) {
  std::string ModuleName = S.getRegion().getEntry()->getParent()->getParent()->getModuleIdentifier();
  std::pair<StringRef, StringRef> filename = StringRef(ModuleName).rsplit('.');
  std::string FileName;

  FileName = dir + "/" + filename.first.str() + DD_LINES;

  std::string ErrInfo1;
  tool_output_file F(FileName.c_str(), ErrInfo1, llvm::sys::fs::F_Append);
  
  if (ErrInfo1.empty()) {
    F.os() << "# LOCs that have data dependency";
    F.os().close();
    if (!F.os().has_error()) {
      F.keep();
    }
  } else {    
    errs() << "error opening file " << FileName << " for writing!\n";
    F.os().clear_error();
    return false;
  }

  FileName = dir + "/" + filename.first.str() + ND_LINES;
  
  std::string ErrInfo2;
  tool_output_file G(FileName.c_str(), ErrInfo2, llvm::sys::fs::F_Append);
  
  if (ErrInfo2.empty()) {
    G.os() << "# LOCs that have not data dependency";
    G.os().close();
    if (!G.os().has_error()) {
      G.keep();
    }
  } else {    
    errs() << "error opening file " << FileName << " for writing!\n";
    G.os().clear_error();
    return false;
  }

  fileInitialized = true;
  return true;
}
*/
