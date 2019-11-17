//===-------- TypeInconsistencyPlugin.cpp ---------------------------------------------===//
//
// This plugin extends the compiler’s warning messages, to point the user’s attention to 
// places in the code that deal with inconsistent types.
//
// cases where inconsistency os checked
// ------------------------------------
// 1. assignments: type lhs vs. type rhs              
// 2. return statements: type return val vs. type in signature
// 3. function arguments: type of passed argument vs. type of parameter in signature
// 4. binary operations: type lhs vs. type rhs 
// 
// reasons for inconsistency 
// -------------------------
// 1. typedef
// 2. bool
// 3. using (alias)
// 4. enum (and int)
// 
//===---------------------------------------------------------------------------------===//

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerHelpers.h"
#include "clang/AST/Type.h"
#include "clang/AST/ASTTypeTraits.h"
#include "clang/AST/OperationKinds.h"

#include <string>
#include <map>

using namespace clang;
using namespace std;

namespace {
    
    class TypeInconsistencyVisitor : public RecursiveASTVisitor<TypeInconsistencyVisitor> {
    private:
        SourceManager& SM;
        DiagnosticsEngine& DE;
        const FunctionDecl *currFuncDecl = nullptr;
        map<string, string> enumConstToEnumType;
                
        void emitMismatchMsgWarning(string msg, string toType, string fromType, SourceLocation loc) {
            unsigned diagID = DE.getCustomDiagID(DiagnosticsEngine::Warning, "types mismatch in %0 between %1 and %2");
            DiagnosticBuilder DB = DE.Report(loc, diagID);
            DB.AddString(msg);
            DB.AddString(fromType);
            DB.AddString(toType);
        }

        string isExprEnumConstant(const Expr *expr) {
            string enumConstant = "";
            if (const DeclRefExpr *DRE = dyn_cast<DeclRefExpr>(expr)) {
                if (const EnumConstantDecl * ECD = dyn_cast<EnumConstantDecl>(DRE->getDecl())) {
                    enumConstant = ECD->getName();
                }
            }
            return enumConstant;
        }
        
        bool checkEnum(const Expr *fromExpr, QualType toType, string msg, SourceLocation loc) {            
            string enumConstant = isExprEnumConstant(fromExpr);
            
            /* exactly one of the 2 sides (to\from) is an enum type */
            if (isa<EnumType>(toType.getCanonicalType()) != (!enumConstant.empty() || isa<EnumType>(fromExpr->getType().getCanonicalType()))) {
                string fromStr = enumConstant == "" ? fromExpr->getType().getCanonicalType().getAsString() : (enumConstToEnumType[enumConstant] + "::" + enumConstant);
                emitMismatchMsgWarning(msg, toType.getAsString(), fromStr, loc);
                return true;
            }            
            return false;
        }
        
        /* converting constructor - a constructor declared without the function-specifier explicit that can be called with a single parameter specifies a conversion from the type of its first parameter to the type of its class */
        bool checkCtorCast(const Expr *fromExpr, QualType toType, string msg, SourceLocation loc) {            
            if (const CXXConstructExpr *ctorExpr = dyn_cast<CXXConstructExpr>(fromExpr)) {
              if (ctorExpr->getNumArgs() == 1) {
                  fromExpr = (*(ctorExpr->arg_begin()))->IgnoreImplicit();
                  QualType fromType = fromExpr->getType().getNonReferenceType();
                  fromType.removeLocalFastQualifiers();

                  toType = toType.getNonReferenceType();
                  toType.removeLocalFastQualifiers();
                  
                  CXXConstructorDecl *ctorDecl = ctorExpr->getConstructor();
                  if (fromType.getAsString() != toType.getAsString() && 
                      ctorExpr->getType().getAsString() == toType.getAsString() &&
                      ctorDecl->isConvertingConstructor(false) /* AllowExplicit = false */) {
                      emitMismatchMsgWarning(msg, toType.getAsString(), fromType.getAsString(), loc);
                      return true; 
                  }
              }
            }
            
            return false;
        }
        
        bool checkBool(const Expr *fromExpr, QualType toType, string msg, SourceLocation loc) {
            /* get all castings that are of the kind: <SomeType> to Boolean */
            if (const ImplicitCastExpr *ICE = dyn_cast<ImplicitCastExpr>(fromExpr)) {
                if (ICE->getCastKind() == CK_MemberPointerToBoolean ||
                    ICE->getCastKind() == CK_PointerToBoolean ||
                    ICE->getCastKind() == CK_IntegralToBoolean ||
                    ICE->getCastKind() == CK_FixedPointToBoolean ||
                    ICE->getCastKind() == CK_FloatingToBoolean ||
                    ICE->getCastKind() == CK_FloatingComplexToBoolean ||
                    ICE->getCastKind() == CK_IntegralComplexToBoolean ||
                    ICE->getCastKind() == CK_BooleanToSignedIntegral) {
                    emitMismatchMsgWarning(msg, toType.getAsString(), fromExpr->IgnoreImpCasts()-> getType().getAsString(), loc);
                    return true;
                }
            }
            
            return false;
        }
        
        bool checkOKConversions(const Expr *fromExpr, QualType toType, string msg, SourceLocation loc) {
            if (const ImplicitCastExpr *ICE = dyn_cast<ImplicitCastExpr>(fromExpr)) {
                /* while casts TO bool are marked with explicit cast kinds, casts from 
                 FROM bool are considered as IntegralCast */
                if (ICE->getSubExpr()->getType().getTypePtr()->isBooleanType()) {
                    return false;
                }
                
                if (/* explicit casts are OK */
                    ICE->isPartOfExplicitCast() ||
                    /* implicit cast of derived class to base class is OK */
                    ICE->getCastKind() == CK_DerivedToBase ||   
                    /* implicit cast of NULL to pointer is OK */
                    ((ICE->getCastKind() == CK_NullToPointer || ICE->getCastKind() == CK_NullToMemberPointer) && isa<PointerType>(toType)) ||             
                    /* implicit cast from small integral\floating type to larger integral\floating type is OK */
                    /* the compiler will warn on its own in the case of implicit conversion that loses precision*/
                    (ICE->getCastKind() == CK_IntegralCast || ICE->getCastKind() == CK_IntegralToFloating || ICE->getCastKind() == CK_FloatingToIntegral) ||
                    /* all conversions between integral\floating and complex are OK */
                    ICE->getCastKind() == CK_FloatingRealToComplex ||
                    ICE->getCastKind() == CK_FloatingComplexToReal ||
                    ICE->getCastKind() == CK_FloatingComplexCast ||
                    ICE->getCastKind() == CK_FloatingComplexToIntegralComplex ||
                    ICE->getCastKind() == CK_IntegralRealToComplex ||
                    ICE->getCastKind() == CK_IntegralComplexToReal ||
                    ICE->getCastKind() == CK_IntegralComplexCast ||
                    ICE->getCastKind() == CK_IntegralComplexToFloatingComplex
                    ) {
                    return true;
                }
            }
            
            return false;
        }
        
        bool checkConversions_binOp(const Expr *lhs, const Expr *rhs, string msg, SourceLocation loc) {
            if (checkConversions(lhs, rhs->getType(), msg, loc, true)) {
                return true;
            }
            else if (checkConversions(rhs, lhs->getType(), msg, loc, true)) {
                return true;
            }
            
            return false;
        }
        
        /* returns true if a conversion was detected, returns false otherwise */
        bool checkConversions(const Expr *fromExpr, QualType toType, string msg, SourceLocation loc, bool checkCtor) {  
            
            /* the order here matters: checkEnum should be before checkOKConversions */
            if (checkEnum(fromExpr->IgnoreImplicit(), toType, msg, loc)) {
                return true;
            }
            
            /* call checkOKConversions without removing implicit, as this function
             checks different kidns of implicit casting */
            if (checkOKConversions(fromExpr, toType, msg, loc)) {
                return false;
            }
            
            fromExpr = fromExpr->IgnoreImplicit();
            if (checkBool(fromExpr, toType, msg, loc)) {
                return true;
            }
            
            /* performing this check in case of a construcor call gives wrong results */
            if (checkCtor && checkCtorCast(fromExpr, toType, msg, loc)) {
                return true;
            }
            
            QualType fromType = fromExpr->getType().getNonReferenceType();
            fromType.removeLocalCVRQualifiers(Qualifiers::TQ::CVRMask);
            
            toType = toType.getNonReferenceType();
            toType.removeLocalCVRQualifiers(Qualifiers::TQ::CVRMask);
            if (fromType.getAsString() == toType.getAsString()) {
                return false;
            }
                        
            emitMismatchMsgWarning(msg, toType.getAsString(), fromExpr->IgnoreImplicit()->IgnoreImpCasts()->getType().getAsString(), loc);
            
            return true;
        }

    public:
        explicit TypeInconsistencyVisitor(CompilerInstance& ci)
        : SM(ci.getSourceManager()), DE(ci.getDiagnostics()) {}
        
        bool VisitDecl(Decl *D) {  
            SourceLocation SL = D->getBeginLoc();
            if (SL.isValid() && !SrcMgr::isSystem(SM.getFileCharacteristic(SL))) {
                /* save for identifying return statement conversions */
                if (const FunctionDecl *FD = dyn_cast<FunctionDecl>(D)) {
                    currFuncDecl = FD;
                }
                
                /* save for giving an elaborated output in enum conversions */
                if (const EnumDecl *ED = dyn_cast<EnumDecl>(D)) {
                    string enumName = "enum " + string(ED->getName());
                    for (auto it = ED->enumerator_begin(); it != ED->enumerator_end(); ++it) {
                        enumConstToEnumType[(*it)->getName()] = enumName;
                    }
                }                
                /*************** Assignment Statement ***************/
                if (const VarDecl *VD = dyn_cast<VarDecl>(D)) {
                    if (VD->hasInit() && !VD->checkInitIsICE()) {
                        bool isBinOpMismatch = false;
                        QualType toType = VD->getType();
                        if (const BinaryOperator *RHS_BO = dyn_cast<BinaryOperator>(VD->getInit())) {
                            if (checkConversions_binOp(RHS_BO->getLHS(), RHS_BO->getRHS(), "binary operation mismatch",  RHS_BO->getOperatorLoc())) {
                                isBinOpMismatch = true;
                            }
                            
                        } 
                        
                        if (!isBinOpMismatch) {
                            checkConversions(VD->getInit(), toType, "assignment", VD->getInit()->getExprLoc(), true);
                        }
                    }
                }
                    
            }    
            return true;
        }
                    
        bool VisitStmt(Stmt *S) {
            SourceLocation SL = S->getBeginLoc();
            if (SL.isValid() && !SrcMgr::isSystem(SM.getFileCharacteristic(SL))) {
                /*************** Assignment Statement ***************/
                if (const BinaryOperator *BO = dyn_cast<BinaryOperator>(S)) {
                    if (BO->isAssignmentOp()) {
                        pair<const clang::VarDecl *, const clang::Expr *> assignStmt = ento::parseAssignment(S);
                        if (assignStmt.first != nullptr && assignStmt.second != nullptr) {
                            bool isBinOpMismatch = false;
                            QualType toType = assignStmt.first->getType();
                        
                            if (const BinaryOperator *RHS_BO = dyn_cast<BinaryOperator>(assignStmt.second)) {
                                if (checkConversions_binOp(RHS_BO->getLHS(), RHS_BO->getRHS(), "binary operation mismatch", RHS_BO->getOperatorLoc())) {
                                    isBinOpMismatch = true;
                                }
                            } 
                            
                            if (!isBinOpMismatch) {
                                checkConversions(assignStmt.second, toType, "assignment", BO->getOperatorLoc(), true);
                            }
                        }
                    }
                }
                else if (const CXXOperatorCallExpr* oprtr = dyn_cast<CXXOperatorCallExpr>(S)) {
                    if (oprtr->isAssignmentOp()) {
                        /* first argument is the lvalue being assigned to */
                        QualType toType = (*(oprtr->arg_begin()))->getType();
                        
                        /* second argument is the rvalue being assigned */
                        checkConversions(*(++(oprtr->arg_begin())), toType, "assignment", oprtr->getOperatorLoc(), true);
                    }
                }
                /*************** Function Call Statement ***************/
                else if (const CallExpr *CE = dyn_cast<CallExpr>(S)) {
                    if (!CE->getBuiltinCallee()) {
                        if (const FunctionDecl *FD = CE->getDirectCallee()) {
                            /* arguments for template instantiation not considered as type-converted */                                                                
                            if (!FD->isTemplateInstantiation()) {
                                auto it_params = FD->param_begin();
                                if (FD->getNumParams() == CE->getNumArgs()) {
                                    for (auto it_args = CE->arg_begin(); it_args != CE->arg_end(); ++it_args, ++it_params) {
                                        QualType toType = (*it_params)->getOriginalType(); 
                                        checkConversions(*it_args, toType, "function argument", (*it_args)->getExprLoc(), true);
                                    }
                                }
                            }
                        }
                    }
                }
                /*************** Constructor Expression ***************/
                /* a constructor call is NOT a function call */
                else if (const CXXConstructExpr *ctorExpr = dyn_cast<CXXConstructExpr>(S)) {
                    CXXConstructorDecl *ctorDecl = ctorExpr->getConstructor();                    
                    if (!ctorDecl->isImplicitlyInstantiable()) {                       
                        auto it_params = ctorDecl->param_begin();
                        if (ctorDecl->getNumParams() == ctorExpr->getNumArgs()) {
                            for (auto it_args = ctorExpr->arg_begin(); it_args != ctorExpr->arg_end(); ++it_args, ++it_params) {
                                QualType toType = (*it_params)->getOriginalType();
                                checkConversions(*it_args, toType, "constructor argument", (*it_args)->getExprLoc(), false);
                            }
                        }
                    }
                }
                /*************** Return Statement ***************/
                else if (const ReturnStmt *RS = dyn_cast<ReturnStmt>(S)) {
                    if (const Expr *retExpr = RS->getRetValue()) {
                        if (currFuncDecl != nullptr) {
                            QualType toType = currFuncDecl->getReturnType();
                            checkConversions(retExpr, toType, "return statement", retExpr->getExprLoc(), true);
                        }
                    }
                }
            }
        return true;
    }
};
    
    class TypeInconsistencyConsumer : public clang::ASTConsumer {
    private:
        TypeInconsistencyVisitor Visitor;

    public:
        TypeInconsistencyConsumer(CompilerInstance &Instance) : Visitor(Instance) {
        }
        
        virtual void HandleTranslationUnit(clang::ASTContext &Context) {
            Visitor.TraverseDecl(Context.getTranslationUnitDecl());
        }
    };
    
    class TypeInconsistencyAction : public PluginASTAction {
    private:
        
    public:
        std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                       llvm::StringRef) override {
            return llvm::make_unique<TypeInconsistencyConsumer>(CI);
        }
        
        bool ParseArgs(const CompilerInstance &CI, const std::vector<std::string> &args) override {
            return true;
        }
    };
}

static FrontendPluginRegistry::Add<TypeInconsistencyAction>
X("TypeInconsistencyPlugin", "compare declared type to actual type");

