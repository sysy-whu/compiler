#include <iostream>
#include "util/Util.h"
#include "util/MyConstants.h"
#include "syTestFiles/LexTest.h"
#include "parser/Parse.h"

// Run->Edit Configurations->Program arguments
// -S -o <OutputFilepath> <InputFilepath> [-O1]
int main(int argc, char **argv) {
    if (Util::handleParams(argc, argv) != PARAMS_PASS) {
        return 0;
    } else {
        std::cout << "Hello, World!" << std::endl;
    }

//    Semantic semantic;
    Parse parse;
    AST ast;
    parse.parseAST(ast);

//    Lex lex;
//    test(lex);

    std::cout << "exit,,," << std::endl;
    return 0;
}
