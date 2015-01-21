//===- ArcherPlugin.cpp ---------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Description
//
//===----------------------------------------------------------------------===//

#include "ArcherPlugin.h"
#include "llvm/ADT/StringExtras.h"
#include <system_error>
#include <sys/types.h>
#include <sys/stat.h>
#include "llvm/Support/ToolOutputFile.h"

using namespace archer;

ArcherDDAClassVisitor::ArcherDDAClassVisitor(StringRef InFile)
{
  std::string path;
  std::string filename;

  ddfilename = ".blacklists/" + InFile.str() + DD_LINES;
  blfilename = ".blacklists/" + InFile.str() + ND_LINES;
  lsfilename = ".blacklists/" + InFile.str() + LS_LINES;
  fcfilename = ".blacklists/" + InFile.str() + FC_LINES;
  
  std::pair<StringRef, StringRef> pathname = InFile.rsplit('/');
  
  if(pathname.second.empty()) {
    path = "";
    filename = pathname.first.str();
  } else {
    path = pathname.first.str();
    filename = pathname.second.str();
  }

  // Read file "ddfilename" and fill "ddaloc" struct
  parse(ddfilename, false, path, filename);

  // Read file "blfilename" and fill "ddaloc" struct
  parse(blfilename, true, path, filename);

  // Read file "lsfilename" and fill "lsloc" struct
  parse(lsfilename, path, filename, lsloc);

  // Read file "fcfilename" and fill "fcloc" struct
  parseFCInfo(fcfilename, path, filename);

  omploc.path = path;
  omploc.filename = filename;

  fcloc.path = path;
  fcloc.filename = filename;

  blacklist.path = path;
  blacklist.filename = filename;
}

void ArcherDDAClassVisitor::parse(std::string pathname, bool value, std::string path, std::string filename)
{
  if (pathname.empty()) {
    llvm::errs() << "File is empty or does not exist!\n";
    exit(-1);
  }
  
  ErrorOr<std::unique_ptr<MemoryBuffer>> FileOrErr = MemoryBuffer::getFile(pathname);
  if (std::error_code EC = FileOrErr.getError()) {
    // llvm::errs() << "File '" << pathname + "' does not exist: " + EC.message() + "\n";
    // llvm::errs() << "Can't open file '" << pathname + "': " + EC.message() + "\n";
    // exit(-1);
  } else {
    int LineNo = 1;
    SmallVector<StringRef, 16> Lines;
    SplitString(FileOrErr.get().get()->getBuffer(), Lines, "\n\r");
    ddaloc.path = path;
    ddaloc.filename = filename;
    for (SmallVectorImpl<StringRef>::iterator I = Lines.begin(), E = Lines.end(); I != E; ++I, ++LineNo) {
      if (I->empty() || I->startswith("#"))
	continue;
      std::pair<StringRef, StringRef> values = I->split(',');
      std::pair<StringRef, StringRef> names = values.second.split(',');
      std::string index = values.first.str();
      std::string File = names.first;
      std::string Dir = names.second;
      // llvm::dbgs() << "Info: " << index <<  " - " << File << " - " << Dir << "\n";
      // llvm::dbgs() << StringToNumber<unsigned>(I->str()) << " - " << ddaloc.line_entries[StringToNumber<unsigned>(I->str())] << "\n";
      if(filename.compare(File) == 0)
	ddaloc.line_entries[StringToNumber<unsigned>(index)] = value;
    }
  }
}

void ArcherDDAClassVisitor::parse(std::string pathname, std::string path, std::string filename, LSInfo &vec)
{
  if (pathname.empty()) {
    llvm::errs() << "File is empty or does not exist!\n";
    exit(-1);
  }
  
  ErrorOr<std::unique_ptr<MemoryBuffer>> FileOrErr = MemoryBuffer::getFile(pathname);
  if (std::error_code EC = FileOrErr.getError()) {
    llvm::errs() << "File '" << pathname + "' does not exist: " + EC.message() + "\n";
    llvm::errs() << "Can't open file '" << pathname + "': " + EC.message() + "\n";
    exit(-1);
  } else {
    int LineNo = 1;
    SmallVector<StringRef, 16> Lines;
    SplitString(FileOrErr.get().get()->getBuffer(), Lines, "\n\r");
    vec.path = path;
    vec.filename = filename;
    for (SmallVectorImpl<StringRef>::iterator I = Lines.begin(), E = Lines.end(); I != E; ++I, ++LineNo) {
      if (I->empty() || I->startswith("#"))
	continue;
      std::pair<StringRef, StringRef> values = I->split(',');
      std::pair<StringRef, StringRef> names1 = values.second.split(',');
      std::pair<StringRef, StringRef> names2 = names1.second.split(',');
      std::string index = values.first.str();
      std::string Function = names1.first;
      std::string File = names2.first;
      std::string Dir = names2.second;
      // llvm::dbgs() << "Info: " << index <<  " - " << Function << " - " << File << " - " << Dir << "\n";
      vec.line_entries.insert(std::make_pair(StringToNumber<unsigned>(index), Function));
    }
  }
}

void ArcherDDAClassVisitor::parseFCInfo(std::string pathname, std::string path, std::string filename)
{
  if (pathname.empty()) {
    llvm::errs() << "File is empty or does not exist!\n";
    exit(-1);
  }
  
  ErrorOr<std::unique_ptr<MemoryBuffer>> FileOrErr = MemoryBuffer::getFile(pathname);
  if (std::error_code EC = FileOrErr.getError()) {
    llvm::errs() << "File '" << pathname + "' does not exist: " + EC.message() + "\n";
    llvm::errs() << "Can't open file '" << pathname + "': " + EC.message() + "\n";
    exit(-1);
  } else {
    int LineNo = 1;
    SmallVector<StringRef, 16> Lines;
    SplitString(FileOrErr.get().get()->getBuffer(), Lines, "\n\r");
    fc.path = path;
    fc.filename = filename;
    for (SmallVectorImpl<StringRef>::iterator I = Lines.begin(), E = Lines.end(); I != E; ++I, ++LineNo) {
      if (I->empty() || I->startswith("#"))
	continue;
      std::pair<StringRef, StringRef> values = I->split(',');
      std::pair<StringRef, StringRef> names1 = values.second.split(',');
      std::pair<StringRef, StringRef> names2 = names1.second.split(',');
      std::string index = values.first.str();
      std::string Function = names1.first;
      std::string File = names2.first;
      std::string Dir = names2.second;
      // llvm::dbgs() << "Info: " << index <<  " - " << Function << " - " << File << " - " << Dir << "\n";
      fc.line_entries[StringToNumber<unsigned>(index)] = Function;
    }
  }
}

void ArcherDDAClassVisitor::setContext(ASTContext &context)
{
  this->context = &context;
}

void ArcherDDAClassVisitor::setSourceManager(SourceManager &sourceMgr)
{
  this->sourceMgr = &sourceMgr;
}

bool ArcherDDAClassVisitor::VisitStmt(clang::Stmt* stmt) 
{
  static unsigned start_current_omp_loc = 0;
  static unsigned end_current_omp_loc = 0;
  // llvm::dbgs() << "Visiting Statements...";
  // llvm::dbgs() << stmt->getStmtClass() << " - " <<  Stmt::ForStmtClass << " - " << sourceMgr->getSpellingLineNumber(stmt->getLocStart()) << "\n";
  //llvm::dbgs() << ToString(stmt->getStmtClass()) << " - Line:" << sourceMgr->getSpellingLineNumber(stmt->getLocStart()) << "\n";

  switch (stmt->getStmtClass()) {
  case Stmt::OMPParallelDirectiveClass:
  case Stmt::OMPForDirectiveClass:
  case Stmt::OMPParallelForDirectiveClass: {
    //llvm::dbgs() << "OMPConstruct - " << sourceMgr->getFilename(stmt->getLocStart()) << " - LOC:" << sourceMgr->getSpellingLineNumber(stmt->getLocStart()) << "\n";
    
    clang::Stmt::child_iterator child = stmt->child_begin();
    
    Stmt *S = cast<Stmt>(*child);
    CapturedStmt *CS = cast<CapturedStmt>(S);

    OMPStmt ompStmt(stmt->getStmtClass(), CS->capture_size(), sourceMgr->getSpellingLineNumber(CS->getLocStart()), sourceMgr->getSpellingLineNumber(CS->getLocEnd()));
    omploc.omp_range.insert(std::make_pair(sourceMgr->getSpellingLineNumber(stmt->getLocStart()), ompStmt));

    // llvm::dbgs() << "Class[" << ToString(stmt->getStmtClass()) << "] - CapturedStmt[" << CS->capture_size() << "] - " << sourceMgr->getFilename(CS->getLocStart()) << " - LOC_START:" << sourceMgr->getSpellingLineNumber(CS->getLocStart()) << " - LOC_END:" << sourceMgr->getSpellingLineNumber(CS->getLocEnd()) << "\n";
    unsigned start_curr_omp_loc = sourceMgr->getSpellingLineNumber(CS->getLocStart());
    unsigned end_curr_omp_loc = sourceMgr->getSpellingLineNumber(CS->getLocEnd());
    if(start_curr_omp_loc > end_current_omp_loc) {
      start_current_omp_loc = start_curr_omp_loc;
      end_current_omp_loc = end_curr_omp_loc;
    }
  }
    break;
    // Remove all the line relative to a For-Loop statement (in this way we can avoid to miss race on For-Loop index)
  case Stmt::ForStmtClass: {
    // llvm::dbgs() << "For-Loop Location: " << sourceMgr->getSpellingLineNumber(stmt->getLocStart()) << "\n"; 
    
    unsigned line = sourceMgr->getSpellingLineNumber(stmt->getLocStart());
    ddaloc.line_entries.erase(line);
  }
    break;
  case Stmt::CallExprClass: {
    unsigned fc_loc = sourceMgr->getSpellingLineNumber(stmt->getLocStart());
    // llvm::dbgs() << "Function Call LOC: " << fc_loc << "\n";
    // llvm::dbgs() << "Start OMP: " << start_current_omp_loc << " - End OMP: " << end_current_omp_loc << "\n";
    if((fc_loc >= start_current_omp_loc && fc_loc <= end_current_omp_loc) && (fc.line_entries.find(fc_loc) != fc.line_entries.end()))
      fcloc.line_entries.insert(std::make_pair(fc_loc, fc.line_entries[fc_loc]));
  }
    break;
  default:
    break;
  }     
      
  return(true);
}

bool ArcherDDAClassVisitor::createBlacklist()
{
  // llvm::dbgs() << "Generating Blacklist...\n";
  // llvm::dbgs() << "Size: " << omploc.omp_range.size() << "\n";

  // for (std::map<unsigned, bool>::iterator it = ddaloc.line_entries.begin(); it != ddaloc.line_entries.end(); ++it) {
  //   std::set<std::pair<unsigned, OMPStmt>>::iterator range = std::find_if(omploc.omp_range.begin(), omploc.omp_range.end(), CompareRange(it->first));
    
  //   if(range->first != 0) {
  //     // llvm::dbgs() << it->first << " - " << it->second << "\n";
  //     if(it->second) {
      
  // 	blacklist.line_entries.insert(range->first);
  // 	blacklist.line_entries.insert(it->first);
      
  // 	llvm::dbgs() << "Value: " << it->first << " - Pragma: " << ToString(range->second.stmtClass) << " - PragmaLine: "<< range->first << " - NumOfStmt: " << range->second.num_of_stmt << " - Range:[" << range->second.lb_loc << "," << range->second.ub_loc << "]\n";
  //     }
  //   }
  // }

  std::set<unsigned> local_blacklist;
  std::set<unsigned> total_blacklist;
  for (std::set<std::pair<unsigned, OMPStmt>>::iterator it1 = omploc.omp_range.begin(); it1 != omploc.omp_range.end(); ++it1) {
    for (std::map<unsigned, bool>::iterator it2 = ddaloc.line_entries.begin(); it2 != ddaloc.line_entries.end(); ++it2) {
      // llvm::dbgs() << it1->first << " - " << it1->second.ub_loc << " - " << it2->first << "\n";
      if(it2->first >= it1->first && it2->first <= it1->second.ub_loc) {
	if(!it2->second) {
	  local_blacklist.insert(it1->first);
	  for (std::set<std::pair<unsigned, std::string>>::iterator it3 = lsloc.line_entries.begin(); it3 != lsloc.line_entries.end(); ++it3) {
	    // llvm::dbgs() << "Lines: " << it3->first << " - " << it2->first << "\n";
	    if((it3->first == it2->first) || (it3->second.find("omp_microtask") != string::npos)) {
	      lsloc.line_entries.erase(it3);
	    }
	  }
	} else {
	  total_blacklist.insert(it1->first);
	  total_blacklist.insert(it2->first);
	}
      }
    }
  }

  // for(std::set<unsigned>::iterator it = total_blacklist.begin(); it != total_blacklist.end(); it++)
  //   llvm::dbgs() << "Line: " << (*it) << "\n";
  
  std::set_difference(total_blacklist.begin(), total_blacklist.end(), local_blacklist.begin(), local_blacklist.end(), inserter(blacklist.line_entries, blacklist.line_entries.begin()));
  
  return writeBlacklistFile();
}
  
  bool ArcherDDAClassVisitor::writeBlacklistFile() 
  {
    std::string dir;
    std::string fileContent = "";

    // Loads/Stores list
    if(lsloc.path.empty())
      dir = ".blacklists";
    else
      dir = lsloc.path + "/.blacklists";
  
    if(llvm::sys::fs::create_directory(Twine(dir))) {
      llvm::errs() << "Unable to create \"" << dir << "\" directory.\n";
      return false;
    }
  
    for(std::set<pair<unsigned, std::string>>::iterator it = lsloc.line_entries.begin(); it != lsloc.line_entries.end(); ++it)
      fileContent = fileContent + "line:" + NumberToString<unsigned>(it->first) + "," + it->second + "\n";
    fileContent = "# Loads/Stores in in the sourcefile \"" + lsloc.filename + "\"\n" + fileContent;

    //llvm::dbgs() << fileContent << "\n";
  
    std::string FileNameLS = dir + "/" + fcloc.filename + LS_LINES;
  
    std::string ErrInfoLS;
    tool_output_file LS(FileNameLS.c_str(), ErrInfoLS, llvm::sys::fs::F_Text);
  
    if (ErrInfoLS.empty()) {
      LS.os() << fileContent;
      LS.os().close();
    }
    if (!LS.os().has_error()) {
      LS.keep();
    } else {    
      errs() << "Error opening file for writing!\n";
      LS.os().clear_error();
      return false;
    }

    // Function calls list
    fileContent = "";

    if(fcloc.path.empty())
      dir = ".blacklists";
    else
      dir = fcloc.path + "/.blacklists";
  
    if(llvm::sys::fs::create_directory(Twine(dir))) {
      llvm::errs() << "Unable to create \"" << dir << "\" directory.\n";
      return false;
    }
  
    for(std::set<std::pair<unsigned, std::string>>::iterator it = fcloc.line_entries.begin(); it != fcloc.line_entries.end(); ++it)
      fileContent = fileContent + "line:" + NumberToString<unsigned>(it->first) + "," + it->second + "," + fcloc.filename + "," + fcloc.path + "\n";

    fileContent = "# Function Calls in between OpenMP constructs information, sourcefile \"" + fcloc.filename + "\"\n" + fileContent;

    //llvm::dbgs() << fileContent << "\n";
  
    std::string FileNameFC = dir + "/function_calls" + FC_LINES + ".tmp";
  
    std::string ErrInfoFC;
    tool_output_file FC(FileNameFC.c_str(), ErrInfoFC, llvm::sys::fs::F_Append);
  
    if (ErrInfoFC.empty()) {
      FC.os() << fileContent;
      FC.os().close();
    }
    if (!FC.os().has_error()) {
      FC.keep();
    } else {    
      errs() << "Error opening file for writing!\n";
      FC.os().clear_error();
      return false;
    }

    // Blacklist
    fileContent = "";
  
    if(blacklist.path.empty())
      dir = ".blacklists";
    else
      dir = blacklist.path + "/.blacklists";
  
    if(llvm::sys::fs::create_directory(Twine(dir))) {
      llvm::errs() << "Unable to create \"" << dir << "\" directory.\n";
      return false;
    }
  
    for (std::set<unsigned>::iterator it = blacklist.line_entries.begin(); it != blacklist.line_entries.end(); ++it)
      fileContent = fileContent + "line:" + NumberToString<unsigned>(*it) + "\n";
    fileContent = "# Blacklisting information sourcefile \"" + blacklist.filename + "\"\n" + fileContent;

    //llvm::dbgs() << fileContent << "\n";
  
    std::string FileName = dir + "/" + blacklist.filename + BL_LINES;
  
    std::string ErrInfo;
    tool_output_file F(FileName.c_str(), ErrInfo, llvm::sys::fs::F_Text);
  
    if (ErrInfo.empty()) {
      F.os() << fileContent;
      F.os().close();
    }
    if (!F.os().has_error()) {
      F.keep();
    } else {    
      errs() << "Error opening file for writing!\n";
      F.os().clear_error();
      return false;
    }
  
    return true; 
  }

  void ArcherDDAConsumer::HandleTranslationUnit(ASTContext &context) 
  {
    visitor->setContext(context);
    visitor->setSourceManager(context.getSourceManager());
    visitor->TraverseDecl(context.getTranslationUnitDecl());
    visitor->createBlacklist();
  }

  clang::ASTConsumer *ArcherDDAAction::CreateASTConsumer(CompilerInstance &Compiler, llvm::StringRef InFile) 
  {
    return(new ArcherDDAConsumer(InFile));
  }

  bool ArcherDDAAction::ParseArgs(const CompilerInstance &CI, const std::vector<std::string>& args) 
  {
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
      llvm::errs() << "ArcherDDAParser arg = " << args[i] << "\n";
    
      // Example error handling.
      if (args[i] == "-an-error") {
	DiagnosticsEngine &D = CI.getDiagnostics();
	unsigned DiagID = D.getCustomDiagID(DiagnosticsEngine::Error, "invalid argument '%0'");
	D.Report(DiagID) << args[i];
	return false;
      }
    }
    if (args.size() && args[0] == "help")
      PrintHelp(llvm::errs());
  
    return true;
  }

  void ArcherDDAAction::PrintHelp(llvm::raw_ostream& ros)
  {
    ros << "Help for ArcherDDAParser plugin goes here\n";
  }

  static FrontendPluginRegistry::Add<ArcherDDAAction>
    X("archer", "Generate blacklisting information exploiting results from different analysis (Polly Data Dependency Analysis, etc.)");

  // virtual bool HandleTopLevelDecl(DeclGroupRef DG) {
  //   for (DeclGroupRef::iterator i = DG.begin(), e = DG.end(); i != e; ++i) {
  // 	const Decl *D = *i;
  // 	if (const NamedDecl *ND = dyn_cast<NamedDecl>(D))
  // 	  llvm::errs() << "top-level-decl: \"" << ND->getNameAsString() << "\"\n";
  //   }

  //   return true;
  // }

  /*
    bool VisitCallExpr(CallExpr *E) {
    NamedDecl *callee = dyn_cast<NamedDecl>(E->getCalleeDecl());
    if(callee == NULL) {
    llvm::errs() << "WTF";
    return false;
    }
  
    llvm::dbgs() << "\tCALL " << callee->getNameAsString() << "\n";
  
    return true;
    }

    bool VisitParmVarDecl(clang::ParmVarDecl* decl)
    {
    //
    // Note: Don't believe this will work as I believe children will be
    //       called before this method is invoked
    //

    bool ret = true;

    std::string paramName = decl->getNameAsString();

    llvm::dbgs() << "VisitParmVarDecl: name is " << paramName.c_str() << ".\n";

    return(ret);
    }
  
    bool VisitVarDecl(clang::VarDecl* decl) {
    //
    // Note: Don't believe this will work as I believe children will be
    //       called before this method is invoked
    //

    bool ret = true;

    std::string varName = decl->getNameAsString();

    llvm::dbgs() << "VisitVarDecl: name is " << varName.c_str() << ".\n";

    return(ret);
    }

    bool VisitNamedDecl(clang::NamedDecl* decl) {
    //
    // Note: Don't believe this will work as I believe children will be
    //       called before this method is invoked
    //

    bool ret = true;

    std::string declName = decl->getNameAsString();

    llvm::dbgs() << "VisitNamedDecl: name is " << declName.c_str() << ".\n"; 

    return(ret);
    }
  */
