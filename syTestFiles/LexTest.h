#ifndef COMPILER_LEXTEST_H
#define COMPILER_LEXTEST_H


#include <iostream>
#include <vector>

#include "../util/MyConstants.h"
#include "../lexer/Token.h"
#include "../lexer/Lex.h"

static void test(Lex lex){
    std::cout << "type()" << "\t" << "sRow" << "\t" << "eRow" << "\t";
    std::cout << "sCol " << "\t" << "eCol" << "\t" << "numVal" << "\t" << "identifierStr" << std::endl;

    std::vector<Token> tokens;

    Token token(lex.getToken());
    while (token.getType() != TOKEN_EOF) {
        tokens.emplace_back(token);  /// no need to create new class
        token = Token(lex.getToken());
    }

    for (Token token1: tokens) {
        std::cout << token1.getType() << "\t" << token1.getStartRow() << "\t" << token1.getEndRow() << "\t";
        std::cout << token1.getStartColumn() << "\t" << token1.getEndColumn() << "\t" << token1.getNumVal() << "\t";

        if (!token1.getIdentifierStr().empty()) {
            std::cout << token1.getIdentifierStr() << std::endl;
        } else {
            std::cout << "null" << std::endl;
        }
    }

}

#endif //COMPILER_LEXTEST_H
