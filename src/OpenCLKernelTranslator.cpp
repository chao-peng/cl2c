#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Tooling.h"

#include "Utils.h"
#include "Constants.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;


auto FunctionWithBodyMathcer = 
    functionDecl().bind("functionDecl");

class FunctionDeclRewriter : public MatchFinder::MatchCallback{
public:
    virtual void run(const MatchFinder::MatchResult &Result){
        if (const FunctionDecl *FD = Result.Nodes.getNodeAs<clang::FunctionDecl>("functionDecl")){
            std::string functionQualifiedName = FD->getQualifiedNameAsString();
            CL2CUtils::alert(functionQualifiedName, output_colour::KBLU, message_type::STD);
        }
    }
};

int parseCode(ClangTool* tool){
    FunctionDeclRewriter functionDeclRewriter;
    MatchFinder matchFinder;
    matchFinder.addMatcher(FunctionWithBodyMathcer, &functionDeclRewriter);

    return tool->run(newFrontendActionFactory(&matchFinder).get());
}