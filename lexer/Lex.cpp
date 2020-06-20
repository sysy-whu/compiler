/********************************************
* Lex.cpp                              *
* Created by zj                             *
* Created by lugf027 on 2020/6/19.            *
* Github: https://github.com/lugf027        *
*********************************************/

#include <iostream>
#include "Lex.h"
#include "../util/Util.h"
#include "../util/Error.h"
#include "../util/MyConstants.h"

FILE *Lex::fp = nullptr;

Lex::Lex() {
    filename = (char *) malloc(sizeof(char) * strlen(Util::getInputUrl()));
    strcpy(filename, Util::getInputUrl());

    std::cout << "filename:" << filename << std::endl;
    fopen_s(&fp, filename, "r");
    if (!fp) {
        Error::errorOpenFile(filename);
    }

    endRow = 0;
    startRow = 0;
    endColumn = 0;
    startColumn = 0;
    NumVal = 0;
    IdentifierStr = "";
    LastChar = ' ';

//    fclose(fp);
}

Token Lex::getToken() {
    startColumn = endColumn;
    startRow = endRow;

    while (isspace(LastChar)) {
        if (LastChar == CHAR_N_BACKEND_SLASH || LastChar == CHAR_R_BACKEND_SLASH) {
            nextLine();
        }
        nextSpaceChar();
    }

    if (isalpha(LastChar)) {
        IdentifierStr = std::to_string(LastChar);
        LastChar = fgetc(fp);
        endColumn++;
        while (isalnum(LastChar) || LastChar == CHAR_UNDERLINE) {
            IdentifierStr += std::to_string(LastChar);
            nextChar();
        }

        /// keyword or identifier
        if (IdentifierStr == KEYWORD_INT)
            return Token(TOKEN_INT, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        else if (IdentifierStr == KEYWORD_VOID)
            return Token(TOKEN_VOID, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        else if (IdentifierStr == KEYWORD_CONST)
            return Token(TOKEN_CONST, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        else if (IdentifierStr == KEYWORD_IF)
            return Token(TOKEN_IF, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        else if (IdentifierStr == KEYWORD_ELSE)
            return Token(TOKEN_ELSE, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        else if (IdentifierStr == KEYWORD_WHILE)
            return Token(TOKEN_WHILE, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        else if (IdentifierStr == KEYWORD_BREAK)
            return Token(TOKEN_BREAK, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        else if (IdentifierStr == KEYWORD_CONTINUE)
            return Token(TOKEN_CONTINUE, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        else if (IdentifierStr == KEYWORD_RETURN)
            return Token(TOKEN_RETURN, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        else return Token(TOKEN_IDENTIFIER, filename, startRow, startColumn, endRow, endColumn, IdentifierStr, 0);

    } // end if keyword or identifier

    /// integer
    if (isdigit(LastChar)) {
        std::string NumStr;
        do {
            NumStr += std::to_string(LastChar);
            nextChar();
        } while (isdigit(LastChar));

        // 也许可以考虑去掉NumVal
        NumVal = stoi(NumStr);
        return Token(TOKEN_NUMBER, filename, startRow, startColumn, endRow, endColumn, nullptr, NumVal);
    }

    /// comment-> "||", "/*"
    if (LastChar == CHAR_SLASH) {
        int ThisChar = LastChar;
        nextChar();
        if (LastChar == CHAR_SLASH) {  /// "//" -> end of line.
            do {
                nextChar();
            } while (LastChar != EOF && LastChar != CHAR_N_BACKEND_SLASH && LastChar != CHAR_R_BACKEND_SLASH);
            if (LastChar != EOF) {
                nextLine();
                return getToken();
            }
        } else if (LastChar == CHAR_STAR) { /// "/*" -> until "*/"
            nextSpaceChar();
            do {
                ThisChar = LastChar;
                nextSpaceChar();
                if (LastChar == CHAR_N_BACKEND_SLASH || LastChar == CHAR_R_BACKEND_SLASH) {
                    nextLine();
                }
            } while (!(ThisChar == CHAR_STAR && LastChar == CHAR_SLASH) && LastChar != EOF);

            if (LastChar != EOF) {
                nextSpaceChar();
                return getToken();
            }
        } else {  /// "/" -> OP_BO_DIV
            return Token(OP_BO_DIV, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        }
    } // end comment

    /// ">="
    if (LastChar == CHAR_GREATER) {
        nextChar();
        if (LastChar == CHAR_EQ) {  /// ">=" -> OP_BO_GTE
            return Token(OP_BO_GTE, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        } else {  /// ">" -> OP_BO_GT
            return Token(OP_BO_GT, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        }
    }

    /// "<="
    if (LastChar == CHAR_LOWER) {
        nextChar();
        if (LastChar == CHAR_EQ) {  /// "<=" -> OP_BO_LTE
            return Token(OP_BO_LTE, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        } else {                   /// "<" -> OP_BO_LT
            return Token(OP_BO_LT, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        }
    }

    /// "=="
    if (LastChar == CHAR_EQ) {
        nextChar();
        if (LastChar == CHAR_EQ) {  /// "==" -> OP_BO_EQ
            return Token(OP_BO_EQ, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        } else {                    /// "=" -> ASSIGN
            return Token(OP_ASSIGN, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        }
    }

    /// "&&"
    if (LastChar == CHAR_AND) {
        int ThisChar = LastChar;
        nextChar();
        if (LastChar == CHAR_AND) {  /// "&&" -> OP_BO_AND
            return Token(OP_BO_AND, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        } else {                     /// SysY2020不支持位运算"&"。考虑在此报错，或者当作Otherwise情况交给Parse处理。此处后者
            Token token(ThisChar, filename, startRow, startColumn, endRow, endColumn, std::to_string(ThisChar), 0);
        }
    }

    /// "||"
    if (LastChar == CHAR_OR) {
        int ThisChar = LastChar;
        nextChar();
        if (LastChar == CHAR_OR) {  /// "||" -> OP_BO_OR
            return Token(OP_BO_OR, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        } else {                    /// SysY2020不支持位运算"|"。考虑在此报错，或者当作Otherwise情况交给Parse处理。此处后者
            return Token(ThisChar, filename, startRow, startColumn, endRow, endColumn, std::to_string(ThisChar), 0);
        }
    }

    /// "!="
    if (LastChar == CHAR_NOT) {
        nextChar();
        if (LastChar == CHAR_EQ) {  /// "!=" -> OP_BO_NEQ
            return Token(OP_BO_NEQ, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        } else {                    /// "!" -> OP_UO_NEG
            return Token(OP_UO_NEG, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        }
    }

    switch (LastChar) {
        case EOF:       /// end file
            return Token(TOKEN_EOF, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        case CHAR_ADD:  /// "+" -> OP_BO_ADD
            return Token(OP_BO_ADD, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        case CHAR_SUB:  /// "-" -> OP_BO_SUB
            return Token(OP_BO_SUB, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        case CHAR_MUL:  /// "*" -> OP_BO_MUL
            return Token(OP_BO_MUL, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
        case CHAR_REM:  /// "%" -> OP_BO_REM
            return Token(OP_BO_REM, filename, startRow, startColumn, endRow, endColumn, nullptr, 0);
    }

    /// otherwise, just return the character as its ascii value like (, ), [, ], {, }, #...
    /// pay attention that there are some returns like this in /// "&&" and /// "||"
    int ThisChar = LastChar;

    Token token(ThisChar, filename, startRow, startColumn, endRow, endColumn, std::to_string(ThisChar), 0);
    nextChar();

    return Token(token);
}

void Lex::nextLine() {
    endRow++;
    startRow++;
    endColumn = 0;
    startColumn = 0;
}
