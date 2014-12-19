//===- ArcherPlugin.h ---------------------------------------------===//
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

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/CapturedStmt.h"
#include "clang/Frontend/CompilerInstance.h"

#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include "Common.h"

#include <iostream>

using namespace std;
using namespace clang;
using namespace llvm;

namespace archer {

  class ArcherDDAClassVisitor : public RecursiveASTVisitor<ArcherDDAClassVisitor>
  {
  private:
    ASTContext *context;
    SourceManager *sourceMgr;
    std::string blfilename;
    std::string ddfilename;
    DDAInfo ddaloc;
    OMPInfo omploc;
    BLInfo blacklist;

    void parse(std::string pathname, bool value, std::string path, std::string filename);
    bool writeBlacklistFile();
    
  public:
    ArcherDDAClassVisitor(StringRef InFile);
    void setContext(ASTContext &context);
    void setSourceManager(SourceManager &sourceMgr);
    bool VisitStmt(clang::Stmt* stmt);
    bool createBlacklist();
  };

  class ArcherDDAConsumer : public ASTConsumer
  {
  private:
    ArcherDDAClassVisitor *visitor;

  public:
    ArcherDDAConsumer(StringRef InFile) { visitor = new ArcherDDAClassVisitor(InFile); }
    virtual void HandleTranslationUnit(ASTContext &context);
  };

  class ArcherDDAAction : public PluginASTAction
  {
  public:
    virtual clang::ASTConsumer *CreateASTConsumer(CompilerInstance &Compiler, llvm::StringRef InFile);

    bool ParseArgs(const CompilerInstance &CI, const std::vector<std::string>& args);
    void PrintHelp(llvm::raw_ostream& ros);
  };

}
