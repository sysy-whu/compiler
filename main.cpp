#include <iostream>

#include "util/Util.h"
#include "util/MyConstants.h"
#include "semantic/Semantic.h"
#include "arm/Arm7Gen.h"



// Run->Edit Configurations->Program arguments
// -S -o <OutputFilepath> <InputFilepath> [-O1]
int main(int argc, char **argv) {
    if (Util::handleParams(argc, argv) != PARAMS_PASS) {
        std::cout << "Error Program arguments" << std::endl;
        return -1;
    }
    Semantic semantic;
    semantic.startSemantic();

    Arm7Gen arm7Gen;
    arm7Gen.startGen(semantic.getAST(), semantic.getSymbolTable());

    std::cout << arm7Gen.genArmTree()->genString() << std::endl;
    // 移步 semantic 构造方法
    //    Parse parse;
    //    parse.parseAST();

    // 移步 parse 构造方法
    //    Lex lex;
    //    test(lex);

    std::cout << "quit" << std::endl;
    return 0;
}
