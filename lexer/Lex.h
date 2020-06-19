/********************************************
* Lex.h                              *
* Created by zj                             *
* modified by lugf027 on 2020/6/19.            *
* Github: https://github.com/lugf027        *
*********************************************/

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
    int NumVal;                // Filled when meet tok_int_num

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
public:
    Lex();

    Token getToken();
};


#endif //COMPILER_LEX_H
