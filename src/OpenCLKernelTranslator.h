#ifndef CL2C_OPENCL_KERNEL_TRANSLATOR_HEADER_
#define CL2C_OPENCL_KERNEL_TRANSLATOR_HEADER_

#include "clang/Tooling/Tooling.h"
#include "UserConfig.h"

int parseCode(clang::tooling::ClangTool* tool);

#endif