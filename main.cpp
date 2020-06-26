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
    parse.parseAST();

    std::cout << parse.getAST().getDecls()->size() << std::endl;
    Decl *decl = parse.getAST().getDecls()->at(0);

    std::cout << "exit,,," << std::endl;

    //    Lex lex;
    //    test(lex);
    return 0;
}
