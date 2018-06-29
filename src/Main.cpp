#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <list>
#include <map>

#include "Utils.h"
#include "Constants.h"
#include "OpenCLKernelTranslator.h"

#include "llvm/Support/CommandLine.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

static llvm::cl::OptionCategory ToolCategory("OpenCL kernel translator command line options");

static llvm::cl::extrahelp CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);

static llvm::cl::extrahelp MoreHelp("\nMore help text...\n");


/*
static llvm::cl::opt<std::string> executableFile(
    "run",
    llvm::cl::desc("Specify the executable file"),
    llvm::cl::value_desc("executable"),
    llvm::cl::Optional
);

static llvm::cl::opt<std::string> expectedOutput(
    "compare",
    llvm::cl::desc("Specify the expected output file path"),
    llvm::cl::value_desc("filename"),
    llvm::cl::Optional
);

static llvm::cl::opt<std::string> keepMutants(
    "keep",
    llvm::cl::desc("Keep which kind (killed, survived, all, none) of generated mutants after execution."),
    llvm::cl::value_desc("type string"),
    llvm::cl::Optional
);

static llvm::cl::opt<std::string> matchLines(
    "lines",
    llvm::cl::desc("Compare certain lines of output with the expected output file"),
    llvm::cl::value_desc("number of lines"),
    llvm::cl::Optional
);

static llvm::cl::opt<std::string> userConfigFileName(
    "config",
    llvm::cl::desc("Specify the user config file name"),
    llvm::cl::value_desc("filename"),
    llvm::cl::Optional // Will be empty string if not specified
);

static llvm::cl::opt<std::string> userSpecifiedTimeout(
    "timeout",
    llvm::cl::desc("Specify the timeout of execution"),
    llvm::cl::value_desc("seconds"),
    llvm::cl::Optional // Will be empty string if not specified
);
*/

int main(int argc, const char** argv){
    clang::tooling::CommonOptionsParser optionsParser(argc, argv, ToolCategory);

    clang::tooling::ClangTool tool(optionsParser.getCompilations(), optionsParser.getSourcePathList());

    parseCode(&tool);
    
    return error_code::STATUS_OK;
}