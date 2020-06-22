#ifndef COMPILER_LEX_H
#define COMPILER_LEX_H

#include "Token.h"
#include <string>

class Lex {
private:
    static FILE *fp;

    int endRow;
    int startRow;
    int endColumn;
    int startColumn;
    char *filename;

    std::string IdentifierStr; // Filled when meet tok_identifier
//    int NumVal;                // Filled when meet tok_int_num

    int LastChar;

    void nextLine();

    void nextSpaceChar() {
        LastChar = fgetc(fp);
        endColumn++;
        startColumn++;
    };

    void nextChar(){
        LastChar = fgetc(fp);
        endColumn++;
    }

    std::string ch2str(int ch){
        IdentifierStr.push_back(ch);
        return IdentifierStr;
    }

    std::string ch2str(int ch1, int ch2){
        IdentifierStr.push_back(ch1);
        IdentifierStr.push_back(ch2);
        return IdentifierStr;
    }

public:
    Lex();

    Token getToken();
};


#endif //COMPILER_LEX_H
