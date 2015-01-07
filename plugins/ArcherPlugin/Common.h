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

#include <map>
#include <set>
#include <string>
#include <sstream>
#include <vector>

#define DD_LINES ".dd"
#define BL_LINES ".bl"

struct DDAInfo {
  std::string path;
  std::string filename;
  std::map<unsigned, bool> line_entries;
};

struct OMPStmt {
  unsigned num_of_stmt;
  unsigned lb_loc;
  unsigned ub_loc;

  OMPStmt(unsigned nos, unsigned lb, unsigned ub) {
    num_of_stmt = nos;
    lb_loc = lb;
    ub_loc = ub;
  }
};

inline bool operator<(const std::pair<unsigned, OMPStmt>& lhs, const std::pair<unsigned, OMPStmt>& rhs)
{
  return lhs.first < rhs.first;
}

struct OMPInfo {
  std::string path;
  std::string filename;
  // pragma LOC + range of CapturedStmt
  std::set<std::pair<unsigned, OMPStmt>> omp_range;
};

struct BLInfo {
  std::string path;
  std::string filename;
  std::set<unsigned> line_entries;
};

template <typename T>
std::string NumberToString ( T Number )
{
  std::ostringstream ss;
  ss << Number;
  return ss.str();
}

template <typename T>
T StringToNumber ( std::string str )
{
  std::istringstream ss(str);
  T val;
  ss >> val;
  return val;
}

struct CompareRange
{
CompareRange(unsigned val) : val_(val) {}
  bool operator()(const std::pair<unsigned, OMPStmt>& elem) const {
    return ((val_ >= elem.second.lb_loc) && (val_ <= elem.second.ub_loc));
  }
private:
  unsigned val_;
};
