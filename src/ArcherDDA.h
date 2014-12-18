/**
 * ArcherDDA.h
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

#ifndef ARCHER_DDA_H
#define ARCHER_DDA_H

#include "llvm/Pass.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"

#include "isl/schedule.h"

#include "polly/Config/config.h"
#include "polly/Dependences.h"
#include "polly/ScopPass.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <system_error>

using namespace llvm;

struct clast_name;
namespace llvm {
  class raw_ostream;
}

template <typename T>
std::string NumberToString ( T Number )
{
  std::ostringstream ss;
  ss << Number;
  return ss.str();
}

namespace polly {
  class Scop;
  class ScopStmt;

  class ArcherDDA : public ScopPass {

  private:
    Scop *S;
    bool getLOCInfo(Scop &Scop, bool isDependency);

  public:
    static char ID;
    std::string dir;

  ArcherDDA() : ScopPass(ID) {
      dir = ".polly";
      if(llvm::sys::fs::create_directory(Twine(dir))) {
	llvm::errs() << "Unable to create \"" << dir << "\" directory.\n";
	exit(-1);
      }
    }
    
    bool runOnScop(Scop &S);
    void printScop(llvm::raw_ostream &OS) const;
    virtual void getAnalysisUsage(AnalysisUsage &AU) const;
    virtual void releaseMemory();
  };
}

namespace llvm {
  class PassRegistry;
  void initializeArcherDDAPass(llvm::PassRegistry &);
}

#endif /* ARCHER_DDA_H */
