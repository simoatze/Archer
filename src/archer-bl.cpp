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

using namespace boost::algorithm;

int main(int argc, char **argv)
{

  std::string content = "";
  std::string line;
  std::vector<OMPStmt> omp_stmt;
  std::map<int, std::string> ndd_info;

  if(argc < 3) {
    printf("Usage: %s FILENAME OPTION[0=Blacklist all,1=Only Sequential instructions,2=Only Parallel Instruction from DDA]\n", argv[0]);
    exit(-1);
  }

  std::string filename = ".blacklists/" + std::string(argv[1]);
  unsigned option = StringToNumber<unsigned>(argv[2]);

  // Sequential Instructions
  if(option == 0 || option == 1) {
    std::ifstream sifile(std::string(filename + SI_LINES).c_str(), std::ios_base::in);
    if(sifile.is_open()) {
      boost::iostreams::filtering_istream insi;
      insi.push(sifile);
      for(; std::getline(insi, line); ) {
	if (!boost::starts_with(line, "#")) {
	  std::vector<std::string> tokens;
	  split(tokens, line, is_any_of(","));
	  content += "line:" + tokens[0] + "," + tokens[2] + "," + tokens[3] + "\n";
	}
      }
    }
  }
  
  // Parallel Instructions from DDA
  if(option == 0 || option == 2) {
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
      for (std::map<int, std::string>::iterator it = ndd_info.begin(); it != ndd_info.end(); ++it) {
    	if((it->first >= omp_stmt[i].lb_loc) && (it->first <= omp_stmt[i].ub_loc)) {
    	  int pos = it->second.find(",");
    	  std::string pragma = "line:" + NumberToString<unsigned>(omp_stmt[i].pragma_loc) + "," + it->second.substr(pos + 1);
    	  if(content.find(pragma) == std::string::npos)
    	    content += pragma + "\n";
    	  content += "line:" + it->second + "\n";
    	  ndd_info.erase(it);
    	  continue;
    	}
      }
    }
  }

  // Writing final blacklist
  std::string blfilename = filename + BL_LINES;
  std::ofstream blfile(blfilename.c_str(), std::ofstream::out); // | std::ofstream::app
  if(!blfile.is_open()) {
    std::cerr << "Couldn't open " << blfilename << std::endl;
    exit(-1);
  }
  if(!content.empty())
    blfile << content << "\n";
  blfile.close();
}
