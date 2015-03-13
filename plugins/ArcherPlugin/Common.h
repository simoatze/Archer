//===- Common.h ---------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Common definition and data structures for Archer.
//
//===----------------------------------------------------------------------===//

#include <string.h>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <vector>

#define PI_LINES ".pi" // Parallel instructions
#define SI_LINES ".si" // Sequential instructions
#define PF_LINES ".pf" // Parallel functions
#define OI_LINES ".oi" // OpenMP Pragma Information
#define FI_LINES ".fi" // Functions Information

#define DD_LINES ".dd" // Data dependency
#define ND_LINES ".nd" // No data dependency
#define FC_LINES ".fc" // Functions call within an OpenMP construct
#define LS_LINES ".ls" // Lines of code that are loads/stores
#define TB_LINES ".tb" // Temporary blacklist
#define BL_LINES ".bl" // Blacklists

struct OMPStmt {
  unsigned pragma_loc;
  unsigned lb_loc;
  unsigned ub_loc;
  std::string stmt_class;

  OMPStmt(unsigned pr, unsigned lb, unsigned ub, std::string scl) {
    pragma_loc = pr;
    lb_loc = lb;
    ub_loc = ub;
    stmt_class = scl;
  }
};
