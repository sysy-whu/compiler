#include <iostream>

#include "util/Util.h"
#include "util/MyConstants.h"
#include "parser/Parse.h"
#include "semantic/Semantic.h"
#include "ir/IRGen.h"
#include "ir/IRLocalDAGGen.h"
#include "ir/ArmDagBuilder.h"
#include "arm/Arm.h"


// Run->Edit Configurations->Program arguments
// -S -o <OutputFilepath> <InputFilepath> [-O1]
int main(int argc, char **argv) {
    if (Util::handleParams(argc, argv) != PARAMS_PASS) {
        std::cout << "Error Program arguments" << std::endl;
        return -1;
    }
    Semantic semantic;
    semantic.startSemantic();

    IRGen irGenIR;
    irGenIR.startIrGen();

    auto *irLocalDagGen = new IRLocalDAGGen(irGenIR.getIrTree());
    irLocalDagGen->startGen();

    // 开始生成arm
    ArmGen armGen(irGenIR.getIrTree());
    armGen.startGen();
    // 移步 semantic 构造方法
    //    Parse parse;
    //    parse.parseAST();

    // 移步 parse 构造方法
    //    Lex lex;
    //    test(lex);
    std::cout << "quit" << std::endl;
    return 0;
}
