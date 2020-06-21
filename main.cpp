#include <iostream>
#include "util/Util.h"
#include "util/MyConstants.h"
#include "parser/Parse.h"
#include "lexer/Lex.h"
#include "syTestFiles/LexTest.h"

// Run->Edit Configurations->Program arguments
// -S -o <OutputFilepath> <InputFilepath> [-O2]
int main(int argc, char **argv) {
    if (Util::handleParams(argc, argv) != PARAMS_PASS) {
        return 0;
    } else {
        std::cout << "Hello, World!" << std::endl;
    }

    Parse parse;

//    Lex lex;
//    test(lex);

    return 0;
}
