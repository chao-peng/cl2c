#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/Tooling.h"

#include "Utils.h"
#include "Constants.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;

std::vector<const VarDecl*> globalDataDecl;

bool isGlobalVariableDefinition(Rewriter& r, const VarDecl* VD) {
    std::string varDeclStr = r.getRewrittenText(VD->getSourceRange());
    std::string varQualifiedName = VD->getQualifiedNameAsString();
    std::string varTypeStr = varDeclStr.substr(0, varDeclStr.find_first_of("=({")); // remove possible initialisation after operators =, ( and {;
    varTypeStr = varTypeStr.substr(0, varTypeStr.find(varQualifiedName));
    if (varTypeStr.find("global") != std::string::npos) {
        return true;
    } else {
        return false;
    }
}

bool isGlobalParmVarDefinition(Rewriter& r, const ParmVarDecl* PD){
    std::string varDeclStr = r.getRewrittenText(PD->getSourceRange());
    std::string varQualifiedName = PD->getQualifiedNameAsString();
    std::string varTypeStr = varDeclStr.substr(0, varDeclStr.find_first_of("=({")); // remove possible initialisation after operators =, ( and {;
    varTypeStr = varTypeStr.substr(0, varTypeStr.find(varQualifiedName));
    if (varTypeStr.find("global") != std::string::npos) {
        return true;
    } else {
        return false;
    }
}

bool isKernelFunctionDefinition(Rewriter& r, const FunctionDecl* FD){
    std::string functionDeclStr = r.getRewrittenText(FD->getSourceRange());
    std::string functionName = FD->getNameAsString();
    std::string functionTypeStr = functionDeclStr.substr(0, functionDeclStr.find(functionName));
    if (functionTypeStr.find("kernel") != std::string::npos){
        return true;
    } else {
        return false;
    }

}

// Need to deal with commas after removing parameter definitions
void removeGlobalParameters(Rewriter& r, const FunctionDecl* FD){
    if (FD->getNumParams() == 0) return;
    std::string newParameterListStr;
    for (unsigned int i = 0; i < FD->getNumParams(); i++){
        const ParmVarDecl* parmDecl = FD->getParamDecl(i);
        if (!isGlobalParmVarDefinition(r, parmDecl)){
            if (newParameterListStr.empty()){
                newParameterListStr = r.getRewrittenText(parmDecl->getSourceRange());
            } else {
                newParameterListStr = newParameterListStr + ", " + r.getRewrittenText(parmDecl->getSourceRange());
            }
        }
    }
    SourceLocation replaceBegin = FD->getParamDecl(0)->getLocStart();
    SourceLocation replaceEnd = FD->getParamDecl(FD->getNumParams() - 1)->getLocEnd();
    SourceRange replaceRange;
    replaceRange.setBegin(replaceBegin);
    replaceRange.setEnd(replaceEnd);
    r.ReplaceText(replaceRange, newParameterListStr);
}

auto VarDeclMatcher = 
    varDecl().bind("variableDecl");

class VarDeclHandler : public MatchFinder::MatchCallback {
private:
    Rewriter& rewriter;

public:
    VarDeclHandler(Rewriter& r) : rewriter(r) {}

    virtual void run(const MatchFinder::MatchResult &result) {
        if (const VarDecl * VD = result.Nodes.getNodeAs<clang::VarDecl>("variableDecl")) {
            if (isGlobalVariableDefinition(rewriter, VD)) {
                globalDataDecl.push_back(VD);
            }
        }
    }
};

auto FunctionDeclMathcer = 
    functionDecl().bind("functionDecl");

class FunctionDeclHandler : public MatchFinder::MatchCallback{
private:
    Rewriter& rewriter;

public:
    FunctionDeclHandler(Rewriter& r) : rewriter(r) {}

    virtual void run(const MatchFinder::MatchResult &result){
        if (const FunctionDecl *FD = result.Nodes.getNodeAs<clang::FunctionDecl>("functionDecl")){
            if (isKernelFunctionDefinition(rewriter, FD)){
                removeGlobalParameters(rewriter, FD);
            }
        }
    }
};

auto FuntionCallMatcher = 
    callExpr().bind("callExpression");

class CallExprHandler : public MatchFinder::MatchCallback{
private:
    Rewriter& rewriter;

public:
    CallExprHandler(Rewriter& r) : rewriter(r) {}

    virtual void run(const MatchFinder::MatchResult &result) {
        if (const CallExpr *CE = result.Nodes.getNodeAs<clang::CallExpr>("callExpression")) {
            std::string calleeFunctionName = rewriter.getRewrittenText(CE->getCallee()->getSourceRange());
            if (calleeFunctionName == "get_global_id") {

            } else if (calleeFunctionName == "get_local_id") {

            } 
        }
    }
};

class KernelTranslatorComsumer : public ASTConsumer {
private:
    MatchFinder matchFinderGlobalDataMatcher;
    VarDeclHandler varDeclHandler;

    MatchFinder matchFinderFucntionDeclRewriter;
    FunctionDeclHandler functionDeclHandler;

    MatchFinder matchFinderCallExprMatcher;
    CallExprHandler callExprHandler;

public:
    KernelTranslatorComsumer(Rewriter& r) : functionDeclHandler(r), varDeclHandler(r), callExprHandler(r) {
        matchFinderGlobalDataMatcher.addMatcher(VarDeclMatcher, &varDeclHandler);
        matchFinderFucntionDeclRewriter.addMatcher(FunctionDeclMathcer ,&functionDeclHandler);
        matchFinderCallExprMatcher.addMatcher(FuntionCallMatcher, &callExprHandler);
    }

    void HandleTranslationUnit(ASTContext& context) override {
        matchFinderGlobalDataMatcher.matchAST(context);
        matchFinderFucntionDeclRewriter.matchAST(context);
        matchFinderCallExprMatcher.matchAST(context);
    }

};

class KernelTranslatorAction : public ASTFrontendAction{
private:
    Rewriter rewriter;

public:
    void EndSourceFileAction() override {
        auto filename = getCurrentFile().rsplit('/').second;
        auto &sourceMgr = rewriter.getSourceMgr();

        std::string rewriteBuffer;
        std::string source;
        auto buffer = rewriter.getRewriteBufferFor(sourceMgr.getMainFileID());
        if (buffer == nullptr) {
            rewriteBuffer = sourceMgr.getBufferData(sourceMgr.getMainFileID());
        } else {
            rewriteBuffer = std::string(buffer->begin(), buffer->end());
        }

        auto filenameStr = filename.str();

        std::stringstream bufferStream(rewriteBuffer);
        std::string line;
        while(getline(bufferStream, line)){
            /* TODO: output to file
            */
           CL2CUtils::alert(line, output_colour::KGRN);
        }
    }

    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &compilerInstance, StringRef inFile) override {
        rewriter.setSourceMgr(compilerInstance.getSourceManager(), compilerInstance.getLangOpts());
        return llvm::make_unique<KernelTranslatorComsumer>(rewriter);
    }

    
};

int parseCode(ClangTool* tool){
    return tool->run(newFrontendActionFactory<KernelTranslatorAction>().get());
}