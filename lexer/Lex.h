#ifndef COMPILER_LEX_H
#define COMPILER_LEX_H

#include "Token.h"
#include <string>

class Lex {
private:
    static FILE *fp;

    int row;
    int endColumn;
    int startColumn;
    char *filename;

    std::string str; // Filled when meet TOKEN_IDENTIFIER or TOKEN_NUMBER or TOKEN_STR

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

    std::string *ch2str(int ch){
        str.push_back(ch);
        return &str;
    }

    std::string *ch2str(int ch1, int ch2){
        str.push_back(ch1);
        str.push_back(ch2);
        nextChar();
        return &str;
    }

public:
    Lex();

    Token getToken();
};


#endif //COMPILER_LEX_H
