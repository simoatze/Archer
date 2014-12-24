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

  ddfilename = ".polly/" + InFile.str() + ".dd";
  blfilename = ".polly/" + InFile.str() + ".bl";
  
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

  omploc.path = path;
  omploc.filename = filename;

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
    llvm::errs() << "Can't open file '" << pathname + "': " + EC.message() + "\n";
    exit(-1);
  }
  int LineNo = 1;
  SmallVector<StringRef, 16> Lines;
  SplitString(FileOrErr.get().get()->getBuffer(), Lines, "\n\r");
  ddaloc.path = path;
  ddaloc.filename = filename;
  for (SmallVectorImpl<StringRef>::iterator I = Lines.begin(), E = Lines.end(); I != E; ++I, ++LineNo) {
    if (I->empty() || I->startswith("#"))
      continue;
    ddaloc.line_entries[StringToNumber<unsigned>(I->str())] = value;
    //llvm::dbgs() << StringToNumber<unsigned>(I->str()) << " - " << ddaloc.line_entries[StringToNumber<unsigned>(I->str())] << "\n";
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
  // llvm::dbgs() << "Visiting Statements...\n";
  switch (stmt->getStmtClass()) {
  case Stmt::OMPParallelForDirectiveClass: {
    //llvm::dbgs() << "OMPConstruct - " << sourceMgr->getFilename(stmt->getLocStart()) << " - LOC:" << sourceMgr->getSpellingLineNumber(stmt->getLocStart()) << "\n";

    clang::Stmt::child_iterator child = stmt->child_begin();
    
    Stmt *S = cast<Stmt>(*child);
    CapturedStmt *CS = cast<CapturedStmt>(S);

    OMPStmt ompStmt(CS->capture_size(), sourceMgr->getSpellingLineNumber(CS->getLocStart()), sourceMgr->getSpellingLineNumber(CS->getLocEnd()));;
    omploc.omp_range.insert(std::make_pair(sourceMgr->getSpellingLineNumber(stmt->getLocStart()), ompStmt));

    //llvm::dbgs() << "CapturedStmt[" << CS->capture_size() << "] - " << sourceMgr->getFilename(CS->getLocStart()) << " - LOC_START:" << sourceMgr->getSpellingLineNumber(CS->getLocStart()) << " - LOC_END:" << sourceMgr->getSpellingLineNumber(CS->getLocEnd()) << "\n";
  }
    break;
  default:
    break;
  }     
      
  return(true);
}

bool ArcherDDAClassVisitor::createBlacklist()
{
  llvm::dbgs() << "Generating Blacklist...\n";

  // llvm::dbgs() << "Size: " << omploc.omp_range.size() << "\n";

  for (std::map<unsigned, bool>::iterator it = ddaloc.line_entries.begin(); it != ddaloc.line_entries.end(); ++it) {
    std::set<std::pair<unsigned, OMPStmt>>::iterator range = std::find_if(omploc.omp_range.begin(), omploc.omp_range.end(), CompareRange(it->first));
    

    // llvm::dbgs() << it->first << " - " << it->second << "\n";
    if(it->second) {
      
      blacklist.line_entries.insert(range->first);
      blacklist.line_entries.insert(it->first);
      
      // llvm::dbgs() << "Value: " << it->first << " - Pragma: " << range->first << " - NumOfStmt: " << range->second.num_of_stmt << " - Range:[" << range->second.lb_loc << "," << range->second.ub_loc << "]\n";
    }
  }
  
  return writeBlacklistFile();
}
  
bool ArcherDDAClassVisitor::writeBlacklistFile() 
{
  std::string fileContent;
  std::string dir;
  
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
  
  std::string FileName = dir + "/" + blacklist.filename + ".bl";
  
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
