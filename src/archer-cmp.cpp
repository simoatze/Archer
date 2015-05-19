#include <stdio.h>
#include <iostream>
#include <vector>
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
  std::string filename;
  std::string line;
  std::set<std::string> fcloc;

  if(argc < 2) {
    printf("Usage: %s FILENAME\n", argv[0]);
    exit(-1);
  }

  filename.append(argv[1]);

  std::string lsfilename = "blacklists/" + filename + LS_LINES;
  std::string fcfilename = "blacklists/function_calls.fc";
  std::string blfilename = "blacklists/" + filename + BL_LINES;

  std::ifstream fcfile(fcfilename.c_str(), std::ios_base::in);
  std::ifstream lsfile(lsfilename.c_str(), std::ios_base::in);
  
  if(!fcfile.is_open()) {
    std::cerr << "Couldn't open " << fcfilename << std::endl;
    exit(-1);
  }

  boost::iostreams::filtering_istream infc;
  infc.push(fcfile);
  for(; std::getline(infc, line); ) {
    unsigned index;

    if (!boost::starts_with(line, "#")) {

      std::vector<std::string> tokens;
      split(tokens, line, is_any_of(","));
     
      fcloc.insert(tokens[1]);
    }
  }

  boost::iostreams::filtering_istream inls;
  inls.push(lsfile);
  for(; std::getline(inls, line); ) {
    unsigned index;

    if (!boost::starts_with(line, "#")) {
      std::vector<std::string> tokens;
      split(tokens, line, is_any_of(","));

      if(fcloc.find(tokens[1]) == fcloc.end())
	 content += tokens[0] + "\n";
    }
  }
  
  // Writing final blacklist
  std::ofstream blfile(blfilename.c_str(), std::ofstream::out | std::ofstream::app);
  if(!blfile.is_open()) {
    std::cerr << "Couldn't open " << blfilename << std::endl;
    exit(-1);
  }
  if(!content.empty())
    blfile << content << "\n";
  blfile.close();
}
