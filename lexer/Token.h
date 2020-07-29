#ifndef COMPILER_TOKEN_H
#define COMPILER_TOKEN_H


#include <string>
#include <utility>
#include <cstring>

class Token {

private:
    int type;
    // 不是多个文件就不用
//    char *filename{};

    unsigned int row;
    unsigned int endColumn;
    unsigned int startColumn;

    std::string IdentifierStr;
    int NumVal;

public:
    Token(int type, char *filename, unsigned int row, unsigned int startColumn, unsigned int endColumn,
          std::string *IdentifierStr, int NumVal) :
            type(type), /*filename(filename),*/ row(row), startColumn(startColumn), endColumn(endColumn),
            IdentifierStr(*IdentifierStr), NumVal(NumVal) {
    };

    Token(Token *token) {
//        if ((*token).filename != nullptr) {
//            filename = (char *) malloc((strlen((*token).filename) + 1) * sizeof(char));
//            strcpy_s(filename, strlen((*token).filename) + 1, (*token).filename);
//        }
        IdentifierStr = (*token).IdentifierStr;
        type = (*token).type;
        row = (*token).row;
        endColumn = (*token).endColumn;
        startColumn = (*token).startColumn;
        NumVal = (*token).NumVal;
    };

    [[nodiscard]] int getType() const {
        return type;
    }

//    [[nodiscard]] char *getFilename() const {
//        return filename;
//    }

    [[nodiscard]] unsigned int getRow() const {
        return row;
    }

    [[nodiscard]] unsigned int getEndColumn() const {
        return endColumn;
    }

    [[nodiscard]] unsigned int getStartColumn() const {
        return startColumn;
    }

    [[nodiscard]] const std::string &getIdentifierStr() const {
        return IdentifierStr;
    }

    [[nodiscard]] int getNumVal() const {
        return NumVal;
    }
};

#endif //COMPILER_TOKEN_H
