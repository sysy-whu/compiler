#ifndef COMPILER_ERROR_H
#define COMPILER_ERROR_H

#include <iostream>
#include <vector>
#include "MyConstants.h"
#include "../lexer/Token.h"

class Error {
private:
    static void printPos(Token token) {
        std::cout << "Position -> from " << token.getStartColumn() << " to " << token.getEndColumn() << " in line:"
                  << token.getStartRow() << " with \"" << token.getIdentifierStr() << "\"" << std::endl;
        exit(-1);
    }

public:
    static void errorParam() { std::cout << "invalid params input" << std::endl; }

    static void errorInputFile() { std::cout << "can't access target input file" << std::endl; }

    static void errorOpenFile() { std::cout << "Error when open target file" << std::endl; }

    static void errorOpenFile(char *filename) { std::cout << "Error when open:" << filename << std::endl; }

    static void errorChar(char ch, int row, int column) {
        std::cout << "Unreadable char:" << ch << " at line: " << row << " column: " << column << std::endl;
    }

    static void errorParse(std::vector<std::string> expects, Token token) {
        std::cout << "Error: Keyword ";
        for (std::string expectStr: expects) {
            std::cout << expectStr << ", ";
        }
        std::cout << " expected!" << std::endl;
        printPos(token);
    }

    static void errorParse(std::vector<int> expects, Token token) {
        std::cout << "Error: Keyword ";
        for (char expectCh: expects) {
            std::cout << expectCh << ", ";
        }
        std::cout << " expected!" << std::endl;
        printPos(token);
    }

    static void errorParse(std::string msg, Token token) {
        std::cout << msg << std::endl;
        printPos(token);
    }
};

#endif //COMPILER_ERROR_H
