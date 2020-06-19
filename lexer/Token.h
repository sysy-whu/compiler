/********************************************
* Token.h                              *
* Created by lugf027 on 2020/6/19.            *
* Github: https://github.com/lugf027        *
*********************************************/

#ifndef COMPILER_TOKEN_H
#define COMPILER_TOKEN_H


#include <string>
#include <utility>
#include <cstring>

class Token {

private:
    int type;
    // 不是多个文件就不用
    char *filename{};

    int endRow;
    int startRow;
    int endColumn;
    int startColumn;

    std::string IdentifierStr;
    int NumVal;

public:
    Token(int type, char *filename, int startRow, int startColumn, int endRow, int endColumn,
          std::string IdentifierStr, int NumVal) :
            type(type), filename(filename), startRow(startRow), startColumn(startColumn), endRow(endRow),
            endColumn(endColumn), IdentifierStr(std::move(IdentifierStr)), NumVal(NumVal) {};

    Token(const Token &token) {
        memcpy(filename, token.filename, strlen(token.filename));
        IdentifierStr = token.IdentifierStr;
        type = token.type;
        endRow = token.endRow;
        endColumn = token.endColumn;
        startRow = token.startRow;
        startColumn = token.startColumn;
        NumVal = token.NumVal;
    };

    int getType() { return type; }

    char *getFilename() { return filename; }

    int getEndRow() { return endRow; }

    int getStartRow() { return startRow; }

    int getEndColumn() { return endColumn; }

    int getStartColumn() { return startColumn; }

    std::string getIdentifierStr() { return IdentifierStr; }

    int getNumVal() { return NumVal; }
};

#endif //COMPILER_TOKEN_H
