#include <iostream>
#include "util/Util.h"
#include "util/MyConstants.h"
#include "lexer/Lex.h"

// Run->Edit Configurations->Program arguments
// -S -o <OutputFilepath> <InputFilepath> [-O2]
int main(int argc, char **argv) {
    if(Util::handleParams(argc, argv) != PARAMS_PASS){
        return 0;
    }else{
        std::cout << "Hello, World!" << std::endl;
    }

    Lex lex;

    std::cout<<Util::getInputUrl()<<std::endl;
    std::cout<<Util::getOutputUrl()<<std::endl;
    std::cout<<Util::getIfO2()<<std::endl;

    return 0;
}
