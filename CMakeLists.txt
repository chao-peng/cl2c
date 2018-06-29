set(LLVM_LINK_COMPONENTS
    Support
)
        
add_clang_executable(cl2c
    src/Utils.cpp
    src/Utils.h
    src/Constants.h
    src/OpenCLKernelTranslator.cpp
    src/OpenCLKernelTranslator.h
    src/UserConfig.cpp
    src/UserConfig.h
    src/Main.cpp)
        
target_link_libraries(cl2c
    clangAST
    clangASTMatchers
    clangBasic
    clangFrontend
    clangTooling)
