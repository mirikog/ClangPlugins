//===--------------------------------- TracesPlugin.cpp ---------------------------------------------===//
//
// This plugin inserts trace prints in all points in the code that represent 
// a fork, i.e. a split of the flow to different branches. The plugin generates a new file
// that has the content of the input file with the trace prints interwoven in it.
//
// The plugin can accept an argument specifying a name for the output file. If this argument 
// is not passed, the plugin will use the name of the input file and will concatenate it with “_traces”.
//
//===------------------------------------------------------------------------------------------------===//

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Rewrite/Frontend/FrontendActions.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include <string>
#include <sstream>
#include <fstream>

using namespace clang;
using namespace ast_matchers;
using namespace std;

#define RESET           "\\e[0m"
#define BOLDBLACK       "\\e[1;30m"
#define BOLDRED         "\\e[1;31m"
#define BOLDGREEN       "\\e[1;32m"
#define BOLDYELLOW      "\\e[1;33m"
#define BOLDBLUE        "\\e[1;34m"
#define BOLDMAGENTA     "\\e[1;35m"
#define BOLDCYAN        "\\e[1;36m"
#define NEWLINE         "\n"

/* used for determining the location of the #define __TRACES__ directive and necessary #include-s */
static const FunctionDecl *topFunc;
/* used for determining the location of new variables defined to handle loops traces */
static const FunctionDecl *currFunc;

namespace {

    static const string getPrintStartStr(bool isCPP) {
        stringstream out;
        if (isCPP) {
            out << "std::cout << \"";
        }
        else {
            out << "printf(\"";
        }
        return out.str();
    }
    
    static const string getPrintEndStr(bool isCPP) {
        stringstream out;
        if (isCPP) {
            out << "\" << std::endl;" << NEWLINE;
        }
        else {
            out << "\\n\");" << NEWLINE;
        }
        return out.str();
    }
    
    typedef struct TraceInfo {
        const string DefaultIndentation = "    ";
        const string TraceCommentBefore = "/* traces - begin */";
        const string TraceCommentAfter =  "/*  traces - end  */";
        string IfDefDirective;
        string EndifDirective = "#endif";
        bool IsCPP;

        TraceInfo(string defineSymb, bool isCPP) : IfDefDirective("#ifdef " + defineSymb), IsCPP(isCPP) {}
        
        string GetTraceStr(int origLine) {
            stringstream trace;            
            if (IsCPP) {
                trace << "std::cout << \"\\n" << BOLDBLACK << ">>> program reached: \" << __FILE__ << \", " << origLine << " <<<" <<  RESET << "\" <<  std::endl;" << NEWLINE;
            }
            else {
                trace << "printf(\"\\n" << BOLDBLACK << ">>> program reached: %s, " << origLine << " <<<" <<  RESET << " \\n\" , __FILE__);" << NEWLINE;
            }
            
            return trace.str();
        }
        
        string GetTracePrologue() {
            return  IfDefDirective + "\t" + TraceCommentBefore;
        }
        
        string GetTraceEpilogue() {
            return EndifDirective + "\t\t\t\t" + TraceCommentAfter;
        }

    } TraceInfo_st;
    
    
    class ForRewriteCallback : public MatchFinder::MatchCallback {
    private:
        SourceManager &SM;
        Rewriter &TheRewriter;
        ASTContext &Context;
        int flagBeforeCount;
        TraceInfo_st *TraceInfo;
    public:
        ForRewriteCallback(SourceManager &sm, Rewriter &rewriter, ASTContext &context, TraceInfo_st *traceInfo) : SM(sm), TheRewriter(rewriter), Context(context), TraceInfo(traceInfo) {
            flagBeforeCount = 1;
        }
        virtual void run(const MatchFinder::MatchResult &Result) {
            const ForStmt *forStmt = Result.Nodes.getNodeAs<ForStmt>("forMatch");
            if (forStmt != nullptr) {
                /* get loop condition */
                string consdStr = "";
                const Expr *cond = forStmt->getCond();
                if (cond != nullptr) {
                    SourceLocation condEnd = Lexer::getLocForEndOfToken(cond->getEndLoc(), 0, SM, Context.getLangOpts());
                    CharSourceRange condCharRange = CharSourceRange::getCharRange(cond->getBeginLoc(), condEnd);
                    consdStr = Lexer::getSourceText(condCharRange, SM, Context.getLangOpts());
                }
                                
                /** define variable to flag that the loop was entered **/
                SourceLocation beforeForLoc;
                stringstream varDefineStr;
                string indent_funcBody = TraceInfo->DefaultIndentation;
                if (currFunc != nullptr) {
                    if (CompoundStmt *CS = dyn_cast<CompoundStmt>(currFunc->getBody())) {
                        /* use this location mark [and not CS->body_front()->getBeginLoc()]
                         in order to make sure that vriables are defined at the very top of
                         the function's definition, to avoid multiple prints in nested loops */
                        beforeForLoc = CS->getLBracLoc().getLocWithOffset(1);
                        indent_funcBody = Lexer::getIndentationForLine(CS->body_front()->getBeginLoc(), SM).str();
                    }
                    else {
                        /* not expected to reach this code, as all functions must be syntactically 
                         defined with braces (= CompoundStmt) */
                        beforeForLoc = forStmt->getForLoc();
                        indent_funcBody = TraceInfo->DefaultIndentation;
                    } 
                }
                else {
                    /* not expected to reach this code, as all for loops statements
                     must be included in a function */
                    beforeForLoc = forStmt->getForLoc();
                }
                string varName = "traces_inForLoop_" + to_string(flagBeforeCount++);
                varDefineStr << indent_funcBody << "bool " << varName << " = false;";
                string varDefinePrint = NEWLINE + TraceInfo->GetTracePrologue() + NEWLINE + varDefineStr.str() + NEWLINE +
                TraceInfo->GetTraceEpilogue();
                TheRewriter.InsertText(beforeForLoc, varDefinePrint, true, false);
                
                /** add trace in top of for loop body **/
                /* check flag variable to see if trace already printed */
                string ifStartStr = "if (" + varName + " == false)" + NEWLINE;
                string ifLBraceStr = "{\n";
                string ifBodyStr_varAssign =  TraceInfo->DefaultIndentation + varName + " = true;" + NEWLINE;
                /* prepare trace string 1 */
                string ifBodyStr_forInfo = TraceInfo->DefaultIndentation + getPrintStartStr(TraceInfo->IsCPP) + BOLDBLACK + ">>> for loop: " + BOLDBLUE + "(" + consdStr + ")" + BOLDBLACK + " <<< " + RESET + "\\n" + getPrintEndStr(TraceInfo->IsCPP);
                string ifRBraceStr = "}\n";
                
                /* get location of loop's body */
                SourceLocation forBodyLoc;
                if (const CompoundStmt *CS = dyn_cast<CompoundStmt>(forStmt->getBody())) {
                    /* loop's body has braces */
                    SourceLocation insertTraceLoc = CS->getLBracLoc().getLocWithOffset(1);
                    string indetBody = Lexer::getIndentationForLine(CS->child_begin()->getBeginLoc(), SM).str();
                    forBodyLoc = CS->child_begin()->getBeginLoc();
                    /* prepare trace string 2 */
                    string ifBodyStr_trace = TraceInfo->DefaultIndentation + TraceInfo->GetTraceStr(SM.getPresumedLineNumber(forBodyLoc));                    
                    string tracePrint = NEWLINE + TraceInfo->GetTracePrologue() + NEWLINE +
                    indetBody + ifStartStr +
                    indetBody + ifLBraceStr +
                    indetBody + ifBodyStr_varAssign + 
                    indetBody + ifBodyStr_trace + 
                    indetBody + ifBodyStr_forInfo + 
                    indetBody + ifRBraceStr +
                    TraceInfo->GetTraceEpilogue();                    
                    TheRewriter.InsertText(insertTraceLoc, tracePrint, true, false);
                }
                else {
                    /* loop's body doesn't have braces */
                    SourceLocation newScopeStart = forStmt->getRParenLoc().getLocWithOffset(1);
                    SourceLocation newScopeEnd = Lexer::findLocationAfterToken(forStmt->getBody()->getEndLoc(), tok::semi, SM, LangOptions(), true);
                    string indetInScope = Lexer::getIndentationForLine(forStmt->getBody()->getBeginLoc(), SM).str();
                    string indetScopeBraces = Lexer::getIndentationForLine(forStmt->getForLoc(), SM).str();
                    forBodyLoc = forStmt->getBody()->getBeginLoc();
                    /* prepare trace string 2 */
                    string ifBodyStr_trace = TraceInfo->DefaultIndentation + TraceInfo->GetTraceStr(SM.getPresumedLineNumber(forBodyLoc));                    
                    string tracePrint = NEWLINE + TraceInfo->GetTracePrologue() + NEWLINE +
                    indetScopeBraces + "{" + NEWLINE +
                    indetInScope + ifStartStr +
                    indetInScope + ifLBraceStr +
                    indetInScope + ifBodyStr_varAssign + 
                    indetInScope + ifBodyStr_trace + 
                    indetInScope + ifBodyStr_forInfo + 
                    indetInScope + ifRBraceStr +
                    TraceInfo->GetTraceEpilogue();
                    TheRewriter.InsertText(newScopeStart, tracePrint, true, false);
                    
                    string endScopePrint = TraceInfo->GetTracePrologue() + NEWLINE +
                    indetScopeBraces + "}" + NEWLINE +
                    TraceInfo->GetTraceEpilogue() + NEWLINE;
                    TheRewriter.InsertText(newScopeEnd, endScopePrint, true, false);
                }
            }
        }
    };
    
    class WhileRewriteCallback : public MatchFinder::MatchCallback {
    private:
        SourceManager &SM;
        Rewriter &TheRewriter;
        ASTContext &Context;
        int flagBeforeCount;
        TraceInfo_st *TraceInfo;
    public:
        WhileRewriteCallback(SourceManager &sm, Rewriter &rewriter, ASTContext &context, TraceInfo_st *traceInfo) : SM(sm), TheRewriter(rewriter), Context(context), TraceInfo(traceInfo) {
            flagBeforeCount = 1;
        }
        
        virtual void run(const MatchFinder::MatchResult &Result) {
            const WhileStmt *whileStmt = Result.Nodes.getNodeAs<WhileStmt>("whileMatch");
            if (whileStmt != nullptr) {
                /* get loop condition */
                const Expr *cond = whileStmt->getCond();
                SourceLocation condEnd = Lexer::getLocForEndOfToken(cond->getEndLoc(), 0, SM, Context.getLangOpts());
                CharSourceRange condCharRange = CharSourceRange::getCharRange(cond->getBeginLoc(), condEnd);
                string consdStr = Lexer::getSourceText(condCharRange, SM, Context.getLangOpts());
                
                /** define variable to flag that the loop was entered **/
                SourceLocation beforeWhileLoc;
                stringstream varDefineStr;
                string indent_funcBody = TraceInfo->DefaultIndentation;
                
                if (currFunc != nullptr) {
                    if (CompoundStmt *CS = dyn_cast<CompoundStmt>(currFunc->getBody())) {
                        /* use this location mark [and not CS->body_front()->getBeginLoc()]
                         in order to make sure that vriables are defined at the very top of
                         the function's definition, to avoid multiple prints in nested loops */
                        beforeWhileLoc = CS->getLBracLoc().getLocWithOffset(1);
                        indent_funcBody = Lexer::getIndentationForLine(CS->body_front()->getBeginLoc(), SM).str();
                    }
                    else {
                        /* not expected to reach this code, as all functions must be syntactically 
                         defined with braces (= CompoundStmt) */
                        beforeWhileLoc = whileStmt->getWhileLoc();
                    } 
                }
                else {
                    /* not expected to reach this code, as all for loops statements
                     must be included in a function */
                    beforeWhileLoc = whileStmt->getWhileLoc();
                }
                string varName = "traces_inWhileLoop_" + to_string(flagBeforeCount++);
                varDefineStr << indent_funcBody << "bool " << varName << " = false;";
                string varDefinePrint = NEWLINE + TraceInfo->GetTracePrologue() + NEWLINE + varDefineStr.str() + NEWLINE + TraceInfo->GetTraceEpilogue();
                TheRewriter.InsertText(beforeWhileLoc, varDefinePrint, true, true);
    
                /** add trace in top of for loop body **/
                /* check flag variable to see if trace already printed */
                string ifStartStr = "if (" + varName + " == false)" + NEWLINE;
                string ifLBraceStr = "{\n";
                string ifBodyStr_varAssign =  TraceInfo->DefaultIndentation + varName + " = true;" + NEWLINE;
                /* prepare trace string 1 */
                string ifBodyStr_whileInfo = TraceInfo->DefaultIndentation + getPrintStartStr(TraceInfo->IsCPP) + BOLDBLACK + ">>> while loop: " + BOLDGREEN + "(" + consdStr + ")" + BOLDBLACK + " <<<" + RESET + "\\n" + getPrintEndStr(TraceInfo->IsCPP);
                string ifRBraceStr = "}\n";
                
                /* get location of loop's body */
                SourceLocation whileBodyLoc;
                if (const CompoundStmt *CS = dyn_cast<CompoundStmt>(whileStmt->getBody())) {
                    /* loop's body has braces */
                    whileBodyLoc = CS->child_begin()->getBeginLoc();
                    /* prepare trace string 2 */
                    string ifBodyStr_trace = TraceInfo->DefaultIndentation + TraceInfo->GetTraceStr(SM.getPresumedLineNumber(whileBodyLoc));
                    
                    SourceLocation insertTrace = CS->getLBracLoc().getLocWithOffset(1);
                    string indetBody = Lexer::getIndentationForLine(CS->child_begin()->getBeginLoc(), SM).str();
                    
                    string tracePrint = NEWLINE + TraceInfo->GetTracePrologue() + NEWLINE +
                    indetBody + ifStartStr +
                    indetBody + ifLBraceStr +
                    indetBody + ifBodyStr_varAssign + 
                    indetBody + ifBodyStr_trace + 
                    indetBody + ifBodyStr_whileInfo + 
                    indetBody + ifRBraceStr +
                    TraceInfo->GetTraceEpilogue();
                    TheRewriter.InsertText(insertTrace, tracePrint, true, false);
                }
                else {
                    /* loop's body doesn't have braces */
                    SourceLocation newScopeStart = Lexer::findLocationAfterToken(whileStmt->getCond()->getEndLoc(), tok::r_paren, SM, LangOptions(), true);
                    SourceLocation newScopeEnd = Lexer::findLocationAfterToken(whileStmt->getBody()->getEndLoc(), tok::semi, SM, LangOptions(), true);
                    string indetInScope = Lexer::getIndentationForLine(whileStmt->getBody()->getBeginLoc(), SM).str();
                    string indetScopeBraces = Lexer::getIndentationForLine(whileStmt->getWhileLoc(), SM).str();
                    
                    whileBodyLoc = whileStmt->getBody()->getBeginLoc();
                    /* prepare trace string 2 */
                    string ifBodyStr_trace = TraceInfo->DefaultIndentation + TraceInfo->GetTraceStr(SM.getPresumedLineNumber(whileBodyLoc));
                    
                    string tracePrint = TraceInfo->GetTracePrologue() + NEWLINE +
                    indetScopeBraces + "{" + NEWLINE +
                    indetInScope + ifStartStr +
                    indetInScope + ifLBraceStr +
                    indetInScope + ifBodyStr_varAssign + 
                    indetInScope + ifBodyStr_trace + 
                    indetInScope + ifBodyStr_whileInfo + 
                    indetInScope + ifRBraceStr +
                    TraceInfo->GetTraceEpilogue() + NEWLINE;
                    TheRewriter.InsertText(newScopeStart, tracePrint, true, false);
                    
                    string endScopePrint = TraceInfo->GetTracePrologue() + NEWLINE +
                    indetScopeBraces + "}" + NEWLINE +
                    TraceInfo->GetTraceEpilogue() + NEWLINE;
                    TheRewriter.InsertText(newScopeEnd, endScopePrint, true, false);
                }
            }
        }
    };
    
    class IfRewriteCallback : public MatchFinder::MatchCallback {
    private:
        SourceManager &SM;
        Rewriter &TheRewriter;
        ASTContext &Context;
        TraceInfo_st *TraceInfo;
    public:
        IfRewriteCallback(SourceManager& sm, Rewriter &rewriter, ASTContext& context, TraceInfo_st *traceInfo) :  SM(sm), TheRewriter(rewriter), Context(context), TraceInfo(traceInfo) {
        }
        virtual void run(const MatchFinder::MatchResult &Result) {
            const IfStmt *ifStmt = Result.Nodes.getNodeAs<IfStmt>("ifMatch");
            if (ifStmt != nullptr) {
                /* get if condition */
                stringstream ifInfo;
                const Expr *cond = ifStmt->getCond();
                SourceLocation condEnd = Lexer::getLocForEndOfToken(cond->getEndLoc(), 0, SM, Context.getLangOpts());
                CharSourceRange condCharRange = CharSourceRange::getCharRange(cond->getBeginLoc(), condEnd);
                string consdStr = Lexer::getSourceText(condCharRange, SM, Context.getLangOpts());
                ifInfo << getPrintStartStr(TraceInfo->IsCPP) << BOLDBLACK << ">>> if statement: " << BOLDMAGENTA << "(" << consdStr << ")" << BOLDBLACK << ", ";
                
                /** handle 'then' clause **/
                SourceLocation thenLoc;
                if (const CompoundStmt *CS = dyn_cast<CompoundStmt>(ifStmt->getThen())) {
                    /* 'then' clause has braces */
                    SourceLocation insertTrace = CS->getLBracLoc().getLocWithOffset(1);
                    string indet_body = Lexer::getIndentationForLine(CS->child_begin()->getBeginLoc(), SM).str();
                    /* prepare trace string */
                    thenLoc = CS->child_begin()->getBeginLoc();
                    string traceStr = TraceInfo->GetTraceStr(SM.getPresumedLineNumber(thenLoc));
                    string ifStr = ifInfo.str() + "'then' clause <<<" + RESET + "\\n" + getPrintEndStr(TraceInfo->IsCPP);
                    string tracePrint = NEWLINE + TraceInfo->GetTracePrologue() + NEWLINE +
                    indet_body + traceStr +
                    indet_body + ifStr +
                    TraceInfo->GetTraceEpilogue();
                    TheRewriter.InsertText(insertTrace, tracePrint, true, false);
                }
                else {
                    /* 'then' clause doesn't have braces */
                    SourceLocation newScopeStart = Lexer::findLocationAfterToken(cond->getEndLoc(), tok::r_paren, SM, LangOptions(), true);
                    SourceLocation newScopeEnd = Lexer::findLocationAfterToken(ifStmt->getThen()->getEndLoc(), tok::semi, SM, LangOptions(), true);
                    string indet_inScope = Lexer::getIndentationForLine(ifStmt->getThen()->getBeginLoc(), SM).str();
                    string indet_scopeBraces = Lexer::getIndentationForLine(ifStmt->getIfLoc(), SM).str();
                    /* prepare trace string */
                    thenLoc = ifStmt->getThen()->getBeginLoc();
                    string traceStr = TraceInfo->GetTraceStr(SM.getPresumedLineNumber(thenLoc));
                    string ifStr = ifInfo.str() + "'then' clause <<<" + RESET + "\\n" + getPrintEndStr(TraceInfo->IsCPP);
                    string tracePrint = TraceInfo->GetTracePrologue() + NEWLINE +
                    indet_scopeBraces + "{" + NEWLINE +
                    indet_inScope + traceStr +
                    indet_inScope + ifStr + 
                    TraceInfo->GetTraceEpilogue() + NEWLINE;
                    TheRewriter.InsertText(newScopeStart, tracePrint, true, false);
                    
                    string endScopePrint = TraceInfo->GetTracePrologue() + NEWLINE +
                    indet_scopeBraces + "}" + NEWLINE +
                    TraceInfo->GetTraceEpilogue() + NEWLINE;
                    TheRewriter.InsertText(newScopeEnd, endScopePrint, true, false);
                }
                
                /** handle 'else' clause **/
                const Stmt *elseStmt = ifStmt->getElse();
                if (elseStmt != nullptr && !isa<IfStmt>(elseStmt)) {
                    /* 'else' clause has braces */
                    SourceLocation elseLoc;
                    if (const CompoundStmt *CS = dyn_cast<CompoundStmt>(ifStmt->getElse())) {
                        SourceLocation insertTrace = CS->getLBracLoc().getLocWithOffset(1);
                        string indet_body = Lexer::getIndentationForLine(CS->child_begin()->getBeginLoc(), SM).str();
                        /* prepare trace string */
                        elseLoc = CS->child_begin()->getBeginLoc();
                        string traceStr = TraceInfo->GetTraceStr(SM.getPresumedLineNumber(elseLoc));
                        string ifStr = ifInfo.str() + "'else' clause <<<" + RESET + "\\n" + getPrintEndStr(TraceInfo->IsCPP);
                        string tracePrint = NEWLINE + TraceInfo->GetTracePrologue() + NEWLINE +
                        indet_body + traceStr +
                        indet_body + ifStr +
                        TraceInfo->GetTraceEpilogue();
                        
                        TheRewriter.InsertText(insertTrace, tracePrint, true, false);
                    }
                    else {
                        /* 'else' clause doesn't have braces */
                        SourceLocation newScopeStart = ifStmt->getElseLoc().getLocWithOffset(string("else").length());
                        SourceLocation newScopeEnd = Lexer::findLocationAfterToken(ifStmt->getElse()->getEndLoc(), tok::semi, SM, LangOptions(), true);
                        string indet_inScope = Lexer::getIndentationForLine(ifStmt->getElse()->getBeginLoc(), SM).str();
                        string indet_scopeBraces = Lexer::getIndentationForLine(ifStmt->getElseLoc(), SM).str();
                        /* prepare trace string */
                        elseLoc = ifStmt->getElse()->getBeginLoc();
                        string traceStr = TraceInfo->GetTraceStr(SM.getPresumedLineNumber(elseLoc));
                        string ifStr = ifInfo.str() +  "'else' clause <<<" + RESET + "\\n" + getPrintEndStr(TraceInfo->IsCPP);
                        string tracePrint = NEWLINE + TraceInfo->GetTracePrologue() + NEWLINE +
                        indet_scopeBraces + "{" + NEWLINE +
                        indet_inScope + traceStr +
                        indet_inScope + ifStr + 
                        TraceInfo->GetTraceEpilogue();
                        TheRewriter.InsertText(newScopeStart, tracePrint, true, false);
                        
                        string endScopePrint = TraceInfo->GetTracePrologue() + NEWLINE +
                        indet_scopeBraces + "}" + NEWLINE +
                        TraceInfo->GetTraceEpilogue() + NEWLINE;
                        TheRewriter.InsertText(newScopeEnd, endScopePrint, true, false);
                    }
                }
            }
        }
    };
    
    class SwitchCaseRewriteCallback : public MatchFinder::MatchCallback {
    private:
        SourceManager &SM;
        Rewriter &TheRewriter;
        ASTContext &Context;
        TraceInfo_st *TraceInfo;
    public:
        SwitchCaseRewriteCallback(SourceManager& sm, Rewriter &rewriter, ASTContext& context, TraceInfo_st *traceInfo) :  SM(sm), TheRewriter(rewriter), Context(context), TraceInfo(traceInfo) {
        }
        virtual void run(const MatchFinder::MatchResult &Result) {
            const SwitchStmt *switchStmt = Result.Nodes.getNodeAs<SwitchStmt>("switchCaseMatch");
            if (switchStmt != nullptr) {
                /* get switch condition */
                stringstream switchInfo;
                const Expr *cond = switchStmt->getCond();
                SourceLocation condEnd = Lexer::getLocForEndOfToken(cond->getEndLoc(), 0, SM, Context.getLangOpts());
                CharSourceRange condCharRange = CharSourceRange::getCharRange(cond->getBeginLoc(), condEnd);
                string consdStr = Lexer::getSourceText(condCharRange, SM, Context.getLangOpts());
                const SwitchCase *switchCase = switchStmt->getSwitchCaseList();
                /* prepare trace string 1 */
                switchInfo << getPrintStartStr(TraceInfo->IsCPP) << BOLDBLACK << ">>> switch statement: (" << consdStr << "), ";
                
                while (switchCase != nullptr) {
                    SourceLocation insertTrace = switchCase->getColonLoc().getLocWithOffset(1);
                    string indetBody = Lexer::getIndentationForLine(switchCase->getSubStmt()->getBeginLoc(), SM).str();
                    /* get switch case */
                    SourceLocation caseEnd = Lexer::getLocForEndOfToken(switchCase->getColonLoc().getLocWithOffset(-1), 0, SM, Context.getLangOpts());
                    CharSourceRange caseCharRange = CharSourceRange::getCharRange(switchCase->getKeywordLoc(), caseEnd);
                    string caseStr = Lexer::getSourceText(caseCharRange, SM, Context.getLangOpts());
                    SourceLocation inCase = switchCase->getSubStmt()->getBeginLoc();
                    /* prepare trace string 2 */
                    string traceStr = TraceInfo->GetTraceStr(SM.getPresumedLineNumber(inCase));
                    string caseInfoStr = switchInfo.str() + BOLDCYAN + caseStr + BOLDBLACK + " <<<" + RESET + "\\n" + getPrintEndStr(TraceInfo->IsCPP);
                    string tracePrint = NEWLINE + TraceInfo->GetTracePrologue() + NEWLINE +
                    indetBody + traceStr +
                    indetBody + caseInfoStr +
                    TraceInfo->GetTraceEpilogue();
                    
                    TheRewriter.InsertText(insertTrace, tracePrint, true, false);
                    switchCase = switchCase->getNextSwitchCase();
                }
            }
        }
    };
    
    class FuncDefRewriteCallback : public MatchFinder::MatchCallback {
    private:
        SourceManager &SM;
        Rewriter &TheRewriter;
        TraceInfo_st *TraceInfo;
    public:
        FuncDefRewriteCallback(SourceManager &sm, Rewriter &rewriter, TraceInfo_st *traceInfo) : SM(sm), TheRewriter(rewriter), TraceInfo(traceInfo) {
        }
        virtual void run(const MatchFinder::MatchResult &Result) {
            const FunctionDecl *funcDecl = Result.Nodes.getNodeAs<FunctionDecl>("funcDefMatch");
            if (funcDecl != nullptr) {
                SourceLocation funcStart;
                string text = "";
                
                /* used for determining the location of new variables defined to handle loops traces */
                currFunc = funcDecl;
                
                /* get the function that is at the very top of the file -
                 used for determining the location of the #define __TRACES__ directive */
                if (topFunc == nullptr || funcDecl->getLocation() < topFunc->getLocation()) {
                    topFunc = funcDecl;
                }
                
                /* prepare trace string 1 */
                stringstream funcInfo;
                funcInfo << getPrintStartStr(TraceInfo->IsCPP) << BOLDBLACK << ">>> function: " << BOLDRED << funcDecl->getNameInfo().getAsString() << BOLDBLACK <<  " <<<" << RESET << "\\n" << getPrintEndStr(TraceInfo->IsCPP);
                if (funcDecl->hasBody()) {
                    if (const CompoundStmt *CS = dyn_cast<CompoundStmt>(funcDecl->getBody())) {
                        if (const Stmt *firstStmt = CS->body_front()) {
                            SourceLocation insertTrace = CS->getLBracLoc().getLocWithOffset(1);
                            string indet_body = Lexer::getIndentationForLine(CS->child_begin()->getBeginLoc(), SM).str();
                            
                            funcStart = firstStmt->getBeginLoc();
                            /* prepare trace string 2 */
                            string traceStr = TraceInfo->GetTraceStr(SM.getPresumedLineNumber(funcStart));
                            string funcStr = funcInfo.str();
                            string tracePrint = NEWLINE + TraceInfo->GetTracePrologue() + NEWLINE +
                            indet_body + traceStr +
                            indet_body + funcStr +
                            TraceInfo->GetTraceEpilogue();
                            TheRewriter.InsertText(insertTrace, tracePrint, true, false);
                        }
                        else {
                            /* support empty constructor */
                            /* prepare trace string 2 */
                            funcStart = CS->getLBracLoc().getLocWithOffset(1);
                            string traceStr = TraceInfo->GetTraceStr(SM.getPresumedLineNumber(funcStart));
                            text = NEWLINE + TraceInfo->GetTracePrologue() + NEWLINE +
                            TraceInfo->DefaultIndentation + traceStr +
                            TraceInfo->DefaultIndentation + funcInfo.str() + 
                            TraceInfo->GetTraceEpilogue() + NEWLINE;
                            TheRewriter.InsertText(funcStart, text, true, false);
                        }
                    }
                    else {
                        /* not expected to reach this code, as all functions must be syntactically 
                         defined with braces (= CompoundStmt) */
                        return;
                    }
                }
            }
        }
    };
    
    class GetTracesConsumer : public clang::ASTConsumer {
    private:
        TraceInfo_st TraceInfo;
        MatchFinder Matcher;
        ForRewriteCallback ForAction;
        WhileRewriteCallback WhileAction;
        IfRewriteCallback IfActions;
        SwitchCaseRewriteCallback SwitchAction;
        FuncDefRewriteCallback FuncDefAction;
    public:
        GetTracesConsumer(CompilerInstance &instance, Rewriter &rewriter, const string defineSymb, bool isCPP): 
        TraceInfo(defineSymb, isCPP),
        ForAction(instance.getSourceManager(), rewriter, instance.getASTContext(), &TraceInfo), 
        WhileAction(instance.getSourceManager(), rewriter, instance.getASTContext(), &TraceInfo), 
        IfActions(instance.getSourceManager(), rewriter, instance.getASTContext(), &TraceInfo),
        SwitchAction(instance.getSourceManager(), rewriter, instance.getASTContext(), &TraceInfo),
        FuncDefAction(instance.getSourceManager(), rewriter, &TraceInfo) {
            Matcher.addMatcher(forStmt(isExpansionInMainFile()).bind("forMatch"), &ForAction);
            Matcher.addMatcher(whileStmt(isExpansionInMainFile()).bind("whileMatch"), &WhileAction);
            Matcher.addMatcher(ifStmt(isExpansionInMainFile()).bind("ifMatch"), &IfActions);
            Matcher.addMatcher(switchStmt(isExpansionInMainFile()).bind("switchCaseMatch"), &SwitchAction);
            Matcher.addMatcher(functionDecl(isExpansionInMainFile(), isDefinition()).bind("funcDefMatch"), &FuncDefAction);
        }
        
        virtual void HandleTranslationUnit(clang::ASTContext &Context) {
            Matcher.matchAST(Context);
        }
    };
    
    class TracesAction : public PluginASTAction {
    private:
        Rewriter TheRewriter;
        SourceManager *SM;
        string RewriteFile = "";
        const string DefineSymb = "__TRACE__";
        bool isCPP = true;
    public:        
        std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, llvm::StringRef) override {
            SM = &CI.getSourceManager();
            
            string compiledFileName = SM->getFileEntryForID(SM->getMainFileID())->getName();
            size_t lastDotPos = compiledFileName.find_last_of(".");
            string fileExtension = compiledFileName.substr(lastDotPos + 1, string::npos);
            if (RewriteFile.empty()) {
                RewriteFile = compiledFileName.substr(0, lastDotPos) + "_traces." + fileExtension;
            } 
            
            isCPP = fileExtension == "cpp";
            TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
            return llvm::make_unique<GetTracesConsumer>(CI, TheRewriter, DefineSymb, isCPP);
        }
        
        bool ParseArgs(const CompilerInstance &CI, const std::vector<std::string> &args) override {
            for (unsigned i = 0, e = args.size(); i != e; ++i) {
                if (args[i] == "-rewrittern-out" &&  i + 1 < args.size()) {
                    RewriteFile = args[i + 1];
                }
            }
            
            return true;
        }
        
        void EndSourceFileAction() override {
            if (topFunc != nullptr) {
                /* add necessary #include-s at the top of the file */
                if (!isCPP) {
                    TheRewriter.InsertText(topFunc->clang::Decl::getBeginLoc(), string("#include <stdbool.h>\n#include <stdio.h>") + "\n\n", true, true);
                }
                else {
                   TheRewriter.InsertText(topFunc->clang::Decl::getBeginLoc(), string("#include <iostream>") + "\n\n", true, true); 
                }
                /* add necessary #define at the top of the file */
                TheRewriter.InsertText(topFunc->clang::Decl::getBeginLoc(), "#define " + DefineSymb + "\n\n", true, true);
            }
            /* write rewritten source to a new file */
            error_code error_code;
            llvm::raw_fd_ostream rewritten(RewriteFile, error_code, llvm::sys::fs::F_None);
            TheRewriter.getEditBuffer(SM->getMainFileID()).write(rewritten);
            rewritten.close();
        }
    };
    
}

static FrontendPluginRegistry::Add<TracesAction>
X("TracesPlugin", "auto generate traces prints in file");
