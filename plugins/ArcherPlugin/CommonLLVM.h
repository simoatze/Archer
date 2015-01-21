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

#include "clang/AST/Stmt.h"

inline const char* ToString(clang::Stmt::StmtClass v) {
  switch (v) {
  case clang::Stmt::NullStmtClass: return "NullStmtClass";
  case clang::Stmt::CompoundStmtClass: return "CompoundStmtClass";
  case clang::Stmt::LabelStmtClass: return "LabelStmtClass";
  case clang::Stmt::AttributedStmtClass: return "AttributedStmtClass";
  case clang::Stmt::IfStmtClass: return "IfStmtClass";
  case clang::Stmt::SwitchStmtClass: return "SwitchStmtClass";
  case clang::Stmt::WhileStmtClass: return "WhileStmtClass";
  case clang::Stmt::DoStmtClass: return "DoStmtClass";
  case clang::Stmt::ForStmtClass: return "ForStmtClass";
  case clang::Stmt::GotoStmtClass: return "GotoStmtClass";
  case clang::Stmt::IndirectGotoStmtClass: return "IndirectGotoStmtClass";
  case clang::Stmt::ContinueStmtClass: return "ContinueStmtClass";
  case clang::Stmt::BreakStmtClass: return "BreakStmtClass";
  case clang::Stmt::ReturnStmtClass: return "ReturnStmtClass";
  case clang::Stmt::DeclStmtClass: return "DeclStmtClass";
  /* case clang::Stmt::SwitchCaseClass: return "SwitchCaseClass"; */
  case clang::Stmt::CaseStmtClass: return "CaseStmtClass";
  case clang::Stmt::DefaultStmtClass: return "DefaultStmtClass";
  case clang::Stmt::CapturedStmtClass: return "CapturedStmtClass";
  /* case clang::Stmt::AsmStmtClass: return "AsmStmtClass"; */
  case clang::Stmt::GCCAsmStmtClass: return "GCCAsmStmtClass";
  case clang::Stmt::MSAsmStmtClass: return "MSAsmStmtClass";
  case clang::Stmt::ObjCAtTryStmtClass: return "ObjCAtTryStmtClass";
  case clang::Stmt::ObjCAtCatchStmtClass: return "ObjCAtCatchStmtClass";
  case clang::Stmt::ObjCAtFinallyStmtClass: return "ObjCAtFinallyStmtClass";
  case clang::Stmt::ObjCAtThrowStmtClass: return "ObjCAtThrowStmtClass";
  case clang::Stmt::ObjCAtSynchronizedStmtClass: return "ObjCAtSynchronizedStmtClass";
  case clang::Stmt::ObjCForCollectionStmtClass: return "ObjCForCollectionStmtClass";
  case clang::Stmt::ObjCAutoreleasePoolStmtClass: return "ObjCAutoreleasePoolStmtClass";
  case clang::Stmt::CXXCatchStmtClass: return "CXXCatchStmtClass";
  case clang::Stmt::CXXTryStmtClass: return "CXXTryStmtClass";
  case clang::Stmt::CXXForRangeStmtClass: return "CXXForRangeStmtClass";
  /* case clang::Stmt::ExprClass: return "ExprClass"; */
  case clang::Stmt::PredefinedExprClass: return "PredefinedExprClass";
  case clang::Stmt::DeclRefExprClass: return "DeclRefExprClass";
  case clang::Stmt::IntegerLiteralClass: return "IntegerLiteralClass";
  case clang::Stmt::FloatingLiteralClass: return "FloatingLiteralClass";
  case clang::Stmt::ImaginaryLiteralClass: return "ImaginaryLiteralClass";
  case clang::Stmt::StringLiteralClass: return "StringLiteralClass";
  case clang::Stmt::CharacterLiteralClass: return "CharacterLiteralClass";
  case clang::Stmt::ParenExprClass: return "ParenExprClass";
  case clang::Stmt::UnaryOperatorClass: return "UnaryOperatorClass";
  case clang::Stmt::OffsetOfExprClass: return "OffsetOfExprClass";
  case clang::Stmt::UnaryExprOrTypeTraitExprClass: return "UnaryExprOrTypeTraitExprClass";
  case clang::Stmt::ArraySubscriptExprClass: return "ArraySubscriptExprClass";
  case clang::Stmt::CallExprClass: return "CallExprClass";
  case clang::Stmt::MemberExprClass: return "MemberExprClass";
  /* case clang::Stmt::CastExprClass: return "CastExprClass"; */
  case clang::Stmt::BinaryOperatorClass: return "BinaryOperatorClass";
  case clang::Stmt::CompoundAssignOperatorClass: return "CompoundAssignOperatorClass";
  /* case clang::Stmt::AbstractConditionalOperatorClass: return "AbstractConditionalOperatorClass"; */
  case clang::Stmt::ConditionalOperatorClass: return "ConditionalOperatorClass";
  case clang::Stmt::BinaryConditionalOperatorClass: return "BinaryConditionalOperatorClass";
  case clang::Stmt::ImplicitCastExprClass: return "ImplicitCastExprClass";
  /* case clang::Stmt::ExplicitCastExprClass: return "ExplicitCastExprClass"; */
  case clang::Stmt::CStyleCastExprClass: return "CStyleCastExprClass";
  case clang::Stmt::CompoundLiteralExprClass: return "CompoundLiteralExprClass";
  case clang::Stmt::ExtVectorElementExprClass: return "ExtVectorElementExprClass";
  case clang::Stmt::InitListExprClass: return "InitListExprClass";
  case clang::Stmt::DesignatedInitExprClass: return "DesignatedInitExprClass";
  case clang::Stmt::ImplicitValueInitExprClass: return "ImplicitValueInitExprClass";
  case clang::Stmt::ParenListExprClass: return "ParenListExprClass";
  case clang::Stmt::VAArgExprClass: return "VAArgExprClass";
  case clang::Stmt::GenericSelectionExprClass: return "GenericSelectionExprClass";
  case clang::Stmt::PseudoObjectExprClass: return "PseudoObjectExprClass";
  case clang::Stmt::AtomicExprClass: return "AtomicExprClass";
  case clang::Stmt::AddrLabelExprClass: return "AddrLabelExprClass";
  case clang::Stmt::StmtExprClass: return "StmtExprClass";
  case clang::Stmt::ChooseExprClass: return "ChooseExprClass";
  case clang::Stmt::GNUNullExprClass: return "GNUNullExprClass";
  case clang::Stmt::CXXOperatorCallExprClass: return "CXXOperatorCallExprClass";
  case clang::Stmt::CXXMemberCallExprClass: return "CXXMemberCallExprClass";
  /* case clang::Stmt::CXXNamedCastExprClass: return "CXXNamedCastExprClass"; */
  case clang::Stmt::CXXStaticCastExprClass: return "CXXStaticCastExprClass";
  case clang::Stmt::CXXDynamicCastExprClass: return "CXXDynamicCastExprClass";
  case clang::Stmt::CXXReinterpretCastExprClass: return "CXXReinterpretCastExprClass";
  case clang::Stmt::CXXConstCastExprClass: return "CXXConstCastExprClass";
  case clang::Stmt::CXXFunctionalCastExprClass: return "CXXFunctionalCastExprClass";
  case clang::Stmt::CXXTypeidExprClass: return "CXXTypeidExprClass";
  case clang::Stmt::UserDefinedLiteralClass: return "UserDefinedLiteralClass";
  case clang::Stmt::CXXBoolLiteralExprClass: return "CXXBoolLiteralExprClass";
  case clang::Stmt::CXXNullPtrLiteralExprClass: return "CXXNullPtrLiteralExprClass";
  case clang::Stmt::CXXThisExprClass: return "CXXThisExprClass";
  case clang::Stmt::CXXThrowExprClass: return "CXXThrowExprClass";
  case clang::Stmt::CXXDefaultArgExprClass: return "CXXDefaultArgExprClass";
  case clang::Stmt::CXXDefaultInitExprClass: return "CXXDefaultInitExprClass";
  case clang::Stmt::CXXScalarValueInitExprClass: return "CXXScalarValueInitExprClass";
  case clang::Stmt::CXXStdInitializerListExprClass: return "CXXStdInitializerListExprClass";
  case clang::Stmt::CXXNewExprClass: return "CXXNewExprClass";
  case clang::Stmt::CXXDeleteExprClass: return "CXXDeleteExprClass";
  case clang::Stmt::CXXPseudoDestructorExprClass: return "CXXPseudoDestructorExprClass";
  case clang::Stmt::TypeTraitExprClass: return "TypeTraitExprClass";
  case clang::Stmt::ArrayTypeTraitExprClass: return "ArrayTypeTraitExprClass";
  case clang::Stmt::ExpressionTraitExprClass: return "ExpressionTraitExprClass";
  case clang::Stmt::DependentScopeDeclRefExprClass: return "DependentScopeDeclRefExprClass";
  case clang::Stmt::CXXConstructExprClass: return "CXXConstructExprClass";
  case clang::Stmt::CXXBindTemporaryExprClass: return "CXXBindTemporaryExprClass";
  case clang::Stmt::ExprWithCleanupsClass: return "ExprWithCleanupsClass";
  case clang::Stmt::CXXTemporaryObjectExprClass: return "CXXTemporaryObjectExprClass";
  case clang::Stmt::CXXUnresolvedConstructExprClass: return "CXXUnresolvedConstructExprClass";
  case clang::Stmt::CXXDependentScopeMemberExprClass: return "CXXDependentScopeMemberExprClass";
  /* case clang::Stmt::OverloadExprClass: return "OverloadExprClass"; */
  case clang::Stmt::UnresolvedLookupExprClass: return "UnresolvedLookupExprClass";
  case clang::Stmt::UnresolvedMemberExprClass: return "UnresolvedMemberExprClass";
  case clang::Stmt::CXXNoexceptExprClass: return "CXXNoexceptExprClass";
  case clang::Stmt::PackExpansionExprClass: return "PackExpansionExprClass";
  case clang::Stmt::SizeOfPackExprClass: return "SizeOfPackExprClass";
  case clang::Stmt::SubstNonTypeTemplateParmExprClass: return "SubstNonTypeTemplateParmExprClass";
  case clang::Stmt::SubstNonTypeTemplateParmPackExprClass: return "SubstNonTypeTemplateParmPackExprClass";
  case clang::Stmt::FunctionParmPackExprClass: return "FunctionParmPackExprClass";
  case clang::Stmt::MaterializeTemporaryExprClass: return "MaterializeTemporaryExprClass";
  case clang::Stmt::LambdaExprClass: return "LambdaExprClass";
  case clang::Stmt::ObjCStringLiteralClass: return "ObjCStringLiteralClass";
  case clang::Stmt::ObjCBoxedExprClass: return "ObjCBoxedExprClass";
  case clang::Stmt::ObjCArrayLiteralClass: return "ObjCArrayLiteralClass";
  case clang::Stmt::ObjCDictionaryLiteralClass: return "ObjCDictionaryLiteralClass";
  case clang::Stmt::ObjCEncodeExprClass: return "ObjCEncodeExprClass";
  case clang::Stmt::ObjCMessageExprClass: return "ObjCMessageExprClass";
  case clang::Stmt::ObjCSelectorExprClass: return "ObjCSelectorExprClass";
  case clang::Stmt::ObjCProtocolExprClass: return "ObjCProtocolExprClass";
  case clang::Stmt::ObjCIvarRefExprClass: return "ObjCIvarRefExprClass";
  case clang::Stmt::ObjCPropertyRefExprClass: return "ObjCPropertyRefExprClass";
  case clang::Stmt::ObjCIsaExprClass: return "ObjCIsaExprClass";
  case clang::Stmt::ObjCIndirectCopyRestoreExprClass: return "ObjCIndirectCopyRestoreExprClass";
  case clang::Stmt::ObjCBoolLiteralExprClass: return "ObjCBoolLiteralExprClass";
  case clang::Stmt::ObjCSubscriptRefExprClass: return "ObjCSubscriptRefExprClass";
  case clang::Stmt::ObjCBridgedCastExprClass: return "ObjCBridgedCastExprClass";
  case clang::Stmt::CUDAKernelCallExprClass: return "CUDAKernelCallExprClass";
  case clang::Stmt::ShuffleVectorExprClass: return "ShuffleVectorExprClass";
  case clang::Stmt::ConvertVectorExprClass: return "ConvertVectorExprClass";
  case clang::Stmt::BlockExprClass: return "BlockExprClass";
  case clang::Stmt::OpaqueValueExprClass: return "OpaqueValueExprClass";
  case clang::Stmt::MSPropertyRefExprClass: return "MSPropertyRefExprClass";
  case clang::Stmt::CXXUuidofExprClass: return "CXXUuidofExprClass";
  case clang::Stmt::SEHTryStmtClass: return "SEHTryStmtClass";
  case clang::Stmt::SEHExceptStmtClass: return "SEHExceptStmtClass";
  case clang::Stmt::SEHFinallyStmtClass: return "SEHFinallyStmtClass";
  case clang::Stmt::SEHLeaveStmtClass: return "SEHLeaveStmtClass";
  case clang::Stmt::MSDependentExistsStmtClass: return "MSDependentExistsStmtClass";
  case clang::Stmt::AsTypeExprClass: return "AsTypeExprClass";
  case clang::Stmt::CEANIndexExprClass: return "CEANIndexExprClass";
  /* case clang::Stmt::OMPExecutableDirectiveClass: return "OMPExecutableDirectiveClass"; */
  case clang::Stmt::OMPParallelDirectiveClass: return "OMPParallelDirectiveClass";
  case clang::Stmt::OMPParallelForDirectiveClass: return "OMPParallelForDirectiveClass";
  case clang::Stmt::OMPParallelForSimdDirectiveClass: return "OMPParallelForSimdDirectiveClass";
  case clang::Stmt::OMPForDirectiveClass: return "OMPForDirectiveClass";
  case clang::Stmt::OMPSimdDirectiveClass: return "OMPSimdDirectiveClass";
  case clang::Stmt::OMPForSimdDirectiveClass: return "OMPForSimdDirectiveClass";
  case clang::Stmt::OMPDistributeSimdDirectiveClass: return "OMPDistributeSimdDirectiveClass";
  case clang::Stmt::OMPDistributeParallelForDirectiveClass: return "OMPDistributeParallelForDirectiveClass";
  case clang::Stmt::OMPDistributeParallelForSimdDirectiveClass: return "OMPDistributeParallelForSimdDirectiveClass";
  case clang::Stmt::OMPSectionsDirectiveClass: return "OMPSectionsDirectiveClass";
  case clang::Stmt::OMPParallelSectionsDirectiveClass: return "OMPParallelSectionsDirectiveClass";
  case clang::Stmt::OMPSectionDirectiveClass: return "OMPSectionDirectiveClass";
  case clang::Stmt::OMPSingleDirectiveClass: return "OMPSingleDirectiveClass";
  case clang::Stmt::OMPTaskDirectiveClass: return "OMPTaskDirectiveClass";
  case clang::Stmt::OMPTaskyieldDirectiveClass: return "OMPTaskyieldDirectiveClass";
  case clang::Stmt::OMPMasterDirectiveClass: return "OMPMasterDirectiveClass";
  case clang::Stmt::OMPCriticalDirectiveClass: return "OMPCriticalDirectiveClass";
  case clang::Stmt::OMPBarrierDirectiveClass: return "OMPBarrierDirectiveClass";
  case clang::Stmt::OMPTaskwaitDirectiveClass: return "OMPTaskwaitDirectiveClass";
  case clang::Stmt::OMPTaskgroupDirectiveClass: return "OMPTaskgroupDirectiveClass";
  case clang::Stmt::OMPAtomicDirectiveClass: return "OMPAtomicDirectiveClass";
  case clang::Stmt::OMPFlushDirectiveClass: return "OMPFlushDirectiveClass";
  case clang::Stmt::OMPOrderedDirectiveClass: return "OMPOrderedDirectiveClass";
  case clang::Stmt::OMPTeamsDirectiveClass: return "OMPTeamsDirectiveClass";
  case clang::Stmt::OMPDistributeDirectiveClass: return "OMPDistributeDirectiveClass";
  case clang::Stmt::OMPCancelDirectiveClass: return "OMPCancelDirectiveClass";
  case clang::Stmt::OMPCancellationPointDirectiveClass: return "OMPCancellationPointDirectiveClass";
  case clang::Stmt::OMPTargetDirectiveClass: return "OMPTargetDirectiveClass";
  case clang::Stmt::OMPTargetDataDirectiveClass: return "OMPTargetDataDirectiveClass";
  case clang::Stmt::OMPTargetUpdateDirectiveClass: return "OMPTargetUpdateDirectiveClass";
  case clang::Stmt::OMPTargetTeamsDirectiveClass: return "OMPTargetTeamsDirectiveClass";
  case clang::Stmt::OMPTeamsDistributeDirectiveClass: return "OMPTeamsDistributeDirectiveClass";
  case clang::Stmt::OMPTeamsDistributeSimdDirectiveClass: return "OMPTeamsDistributeSimdDirectiveClass";
  case clang::Stmt::OMPTargetTeamsDistributeDirectiveClass: return "OMPTargetTeamsDistributeDirectiveClass";
  case clang::Stmt::OMPTargetTeamsDistributeSimdDirectiveClass: return "OMPTargetTeamsDistributeSimdDirectiveClass";
  case clang::Stmt::OMPTeamsDistributeParallelForDirectiveClass: return "OMPTeamsDistributeParallelForDirectiveClass";
  case clang::Stmt::OMPTeamsDistributeParallelForSimdDirectiveClass: return "OMPTeamsDistributeParallelForSimdDirectiveClass";
  case clang::Stmt::OMPTargetTeamsDistributeParallelForDirectiveClass: return "OMPTargetTeamsDistributeParallelForDirectiveClass";
  case clang::Stmt::OMPTargetTeamsDistributeParallelForSimdDirectiveClass: return "OMPTargetTeamsDistributeParallelForSimdDirectiveClass";
  default: return "[Unknown OS_type]";
  }
}

struct DDAInfo {
  std::string path;
  std::string filename;
  std::map<unsigned, bool> line_entries;
};

struct OMPStmt {
  clang::Stmt::StmtClass stmtClass;
  unsigned num_of_stmt;
  unsigned lb_loc;
  unsigned ub_loc;

OMPStmt(clang::Stmt::StmtClass scl, unsigned nos, unsigned lb, unsigned ub) {
    stmtClass = scl;
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

struct LSInfo {
  std::string path;
  std::string filename;
  std::set<std::pair<unsigned, std::string>> line_entries;
};

struct FCInfo {
  std::string path;
  std::string filename;
  std::map<unsigned, std::string> line_entries;
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
