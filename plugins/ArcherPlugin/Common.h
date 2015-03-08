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

#define PI_LINES ".pi" // Parallel instructions
#define SI_LINES ".si" // Sequential instructions
#define PF_LINES ".pf" // Parallel functions
#define DD_LINES ".dd" // Data dependency
#define ND_LINES ".nd" // No data dependency
#define FC_LINES ".fc" // Functions call within an OpenMP construct
#define LS_LINES ".ls" // Lines of code that are loads/stores
#define TB_LINES ".tb" // Temporary blacklist
#define BL_LINES ".bl" // Blacklists
