#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream> 
#include <set>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

#include "Common.h"
#include "Util.h"

using namespace boost::algorithm;

int main(int argc, char **argv)
{
  std::string content = "";
  std::string line;
  std::vector<OMPStmt> omp_stmt;
  std::map<int, std::string> si_info;
  std::map<int, std::string> ndd_info;

  if(argc < 3) {
    printf("Usage: %s FILENAME OPTION[0=Blacklist all,1=Only Sequential instructions,2=Only Parallel Instruction from DDA]\n", argv[0]);
    exit(-1);
  }

  std::string file = std::string(argv[1]);
  
  if(file.substr(0,2).compare("./") == 0)
    file = file.substr(2);

  std::vector<std::string> tokens;
  split(tokens, file, is_any_of("/"));
  std::string filename = file.substr(0, file.size() - tokens.back().size()) + ".archer/blacklists/" + tokens.back();
  unsigned option = StringToNumber<unsigned>(argv[2]);

  // Sequential Instructions
  std::ifstream sifile(std::string(filename + SI_LINES).c_str(), std::ios_base::in);
  if(sifile.is_open()) {
    boost::iostreams::filtering_istream insi;
    insi.push(sifile);
    for(; std::getline(insi, line); ) {
      if (!boost::starts_with(line, "#")) {
	std::vector<std::string> tokens;
	split(tokens, line, is_any_of(","));
	si_info.insert(std::pair<int,std::string>(StringToNumber<unsigned>(tokens[0]), line));
      }
      
    }
  }
  
  // Parallel Instructions from DDA
  // Load OpenMP pragmas info
  std::ifstream oifile(std::string(filename + OI_LINES).c_str(), std::ios_base::in);
  if(oifile.is_open()) {
    boost::iostreams::filtering_istream inoi;
    inoi.push(oifile);
    for(; std::getline(inoi, line); ) {
      if (!boost::starts_with(line, "#")) {
	std::vector<std::string> tokens;
	split(tokens, line, is_any_of(","));
	OMPStmt ompStmt(StringToNumber<unsigned>(tokens[0]), StringToNumber<unsigned>(tokens[1]), StringToNumber<unsigned>(tokens[2]), tokens[3]);
	omp_stmt.push_back(ompStmt);
      }
    }
  }
  
  // Loading DDA in a map data structure <line number, string>
  std::ifstream ndfile(std::string(filename + ND_LINES).c_str(), std::ios_base::in);
  if(ndfile.is_open()) {
    boost::iostreams::filtering_istream innd;
    innd.push(ndfile);
    for(; std::getline(innd, line); ) {
      if (!boost::starts_with(line, "#")) {
	std::vector<std::string> tokens;
	split(tokens, line, is_any_of(","));
	ndd_info.insert(std::pair<int,std::string>(StringToNumber<unsigned>(tokens[0]), line));
      }
    }
  }
  
  // Compare DDA info with OpenMP pragma info to find the correct pragma line number
  for(int i = 0; i < omp_stmt.size(); i++) {
    unsigned pragma_num = omp_stmt[i].pragma_loc;
    for (std::map<int, std::string>::iterator it = ndd_info.begin(); it != ndd_info.end(); ++it) {
      if((it->first >= omp_stmt[i].lb_loc) && (it->first <= omp_stmt[i].ub_loc)) {
	int pos = it->second.find(",");
	std::string pragma = "line:" + NumberToString<unsigned>(pragma_num) + "," + it->second.substr(pos + 1);
	if(content.find(pragma) == std::string::npos)
	  content += pragma + "\n";
	content += "line:" + it->second + "\n";
	ndd_info.erase(it);
	continue;
      }
    }
    // if a line is already in the parallel list needs to be removed from the
    // sequential instruction list to avoid blacklist wrong lines (i.e. pragmas)
    std::map<int, std::string>::const_iterator it2 = si_info.find(pragma_num);
    if(it2 != si_info.end()) {
      si_info.erase(pragma_num);
    }
  }
  
  for (std::map<int, std::string>::iterator it = si_info.begin(); it != si_info.end(); ++it)
    content += "line:" + NumberToString<unsigned>(it->first) + "," + it->second;
  
  // Writing final blacklist
  std::string blfilename = filename + BL_LINES;
  std::ofstream blfile(blfilename.c_str(), std::ofstream::out); // | std::ofstream::app
  if(!blfile.is_open()) {
    std::cerr << "Couldn't open " << blfilename << std::endl;
    exit(-1);
  }
  blfile << "# Blacklist file\n";
  if(!content.empty())
    blfile << content << "\n";
  blfile.close();
}
