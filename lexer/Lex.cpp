#include <iostream>
#include "Lex.h"
#include "../util/Util.h"
#include "../util/Error.h"

FILE *Lex::fp = nullptr;

Lex::Lex() {
    filename = (char *) malloc(sizeof(char) * strlen(Util::getInputUrl()));
    strcpy(filename, Util::getInputUrl());

    std::cout << "source filename:" << filename << std::endl;
    fp = fopen(filename, "r");
//    fopen_s(&fp, filename, "r");
    if (!fp) {
        Error::errorOpenFile(filename);
    }

    row = 1;
    endColumn = 0;
    startColumn = 0;
    str = "";
    LastChar = ' ';
}

Token Lex::getToken() {
    startColumn = endColumn;
    str = "";

    while (isspace(LastChar)) {
        if (LastChar == CHAR_N_BACKEND_SLASH || LastChar == CHAR_R_BACKEND_SLASH) {
            nextLine();
        }
        nextSpaceChar();
    }

    if (isalpha(LastChar) || LastChar == CHAR_UNDERLINE) {
        str.push_back(LastChar);
        LastChar = fgetc(fp);
        endColumn++;
        while (isalnum(LastChar) || LastChar == CHAR_UNDERLINE) {
            str.push_back(LastChar);
            nextChar();
        }

        /// keyword or identifier
        if (str == KEYWORD_INT) {
            return Token(TOKEN_INT, filename, row, startColumn, endColumn, &str, 0);
        } else if (str == KEYWORD_VOID)
            return Token(TOKEN_VOID, filename, row, startColumn, endColumn, &str, 0);
        else if (str == KEYWORD_CONST)
            return Token(TOKEN_CONST, filename, row, startColumn, endColumn, &str, 0);
        else if (str == KEYWORD_IF)
            return Token(TOKEN_IF, filename, row, startColumn, endColumn, &str, 0);
        else if (str == KEYWORD_ELSE)
            return Token(TOKEN_ELSE, filename, row, startColumn, endColumn, &str, 0);
        else if (str == KEYWORD_WHILE)
            return Token(TOKEN_WHILE, filename, row, startColumn, endColumn, &str, 0);
        else if (str == KEYWORD_BREAK)
            return Token(TOKEN_BREAK, filename, row, startColumn, endColumn, &str, 0);
        else if (str == KEYWORD_CONTINUE)
            return Token(TOKEN_CONTINUE, filename, row, startColumn, endColumn, &str, 0);
        else if (str == KEYWORD_RETURN)
            return Token(TOKEN_RETURN, filename, row, startColumn, endColumn, &str, 0);
        else
            return Token(TOKEN_IDENTIFIER, filename, row, startColumn, endColumn, &str, 0);

    } // end if keyword or identifier

    /// integer
    if (isdigit(LastChar)) {
        do {
            str.push_back(LastChar);
            nextChar();
        } while (isdigit(LastChar));
        return Token(TOKEN_NUMBER, filename, row, startColumn, endColumn, &str, stoi(str));
    }

    int ThisChar = LastChar;
    nextChar();

    /// comment-> "||", "/*"
    if (ThisChar == CHAR_SLASH) {
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
            return Token(OP_BO_DIV, filename, row, startColumn, endColumn, ch2str(ThisChar), 0);
        }
    } // end comment

    switch (ThisChar) {
        case CHAR_EQ:
            if (LastChar == CHAR_EQ) {  /// "==" -> OP_BO_EQ
                return Token(OP_BO_EQ, filename, row, startColumn, endColumn, ch2str(ThisChar, LastChar), 0);
            } else {                    /// "=" -> ASSIGN
                return Token(OP_ASSIGN, filename, row, startColumn, endColumn, ch2str(ThisChar), 0);
            }
        case CHAR_ADD:  /// "+" -> OP_BO_ADD
            return Token(OP_BO_ADD, filename, row, startColumn, endColumn, ch2str(ThisChar), 0);
        case CHAR_SUB:  /// "-" -> OP_BO_SUB
            return Token(OP_BO_SUB, filename, row, startColumn, endColumn, ch2str(ThisChar), 0);
        case CHAR_MUL:  /// "*" -> OP_BO_MUL
            return Token(OP_BO_MUL, filename, row, startColumn, endColumn, ch2str(ThisChar), 0);
        case CHAR_REM:  /// "%" -> OP_BO_REM
            return Token(OP_BO_REM, filename, row, startColumn, endColumn, ch2str(ThisChar), 0);
        case CHAR_GREATER:
            if (LastChar == CHAR_EQ) {  /// ">=" -> OP_BO_GTE
                return Token(OP_BO_GTE, filename, row, startColumn, endColumn, ch2str(ThisChar, LastChar), 0);
            } else {                    /// ">" -> OP_BO_GT
                return Token(OP_BO_GT, filename, row, startColumn, endColumn, ch2str(ThisChar), 0);
            }
        case CHAR_LOWER:
            if (LastChar == CHAR_EQ) {  /// "<=" -> OP_BO_LTE
                return Token(OP_BO_LTE, filename, row, startColumn, endColumn, ch2str(ThisChar, LastChar), 0);
            } else {                    /// "<" -> OP_BO_LT
                return Token(OP_BO_LT, filename, row, startColumn, endColumn, ch2str(ThisChar), 0);
            }
        case CHAR_AND:
            if (LastChar == CHAR_AND) {  /// "&&" -> OP_BO_AND
                return Token(OP_BO_AND, filename, row, startColumn, endColumn, ch2str(ThisChar, LastChar), 0);
            } else {                     /// SysY2020不支持位运算"&"。考虑在此报错，或者当作Otherwise情况交给Parse处理。此处后者
                Token token(ThisChar, filename, row, startColumn, endColumn, ch2str(ThisChar), 0);
            }
        case CHAR_OR:
            if (LastChar == CHAR_OR) {  /// "||" -> OP_BO_OR
                return Token(OP_BO_OR, filename, row, startColumn, endColumn, ch2str(ThisChar, LastChar), 0);
            } else {                    /// SysY2020不支持位运算"|"。考虑在此报错，或者当作Otherwise情况交给Parse处理。此处后者
                return Token(ThisChar, filename, row, startColumn, endColumn, ch2str(ThisChar), 0);
            }
        case CHAR_NOT:
            if (LastChar == CHAR_EQ) {  /// "!=" -> OP_BO_NEQ
                return Token(OP_BO_NEQ, filename, row, startColumn, endColumn, ch2str(ThisChar, LastChar), 0);
            } else {                    /// "!" -> OP_UO_NEG
                return Token(OP_UO_NOT, filename, row, startColumn, endColumn, ch2str(ThisChar), 0);
            }
        case EOF:                       /// end file
            return Token(TOKEN_EOF, filename, row, startColumn, endColumn, ch2str(ThisChar), 0);
        case CHAR_QUOTE_DOUBLE: {      /// "<格式串>"
            while (LastChar != CHAR_QUOTE_DOUBLE && LastChar != EOF) {
                str.push_back(LastChar);
                nextChar();
            }
            nextChar();
            return Token(TOKEN_STR, filename, row, startColumn, endColumn, &str, 0);
        }
        default:
            /// otherwise, just return the character as its ascii value like (, ), [, ], {, }, ,, ; ...
            /// pay attention that there are some returns like this in /// "&&" and /// "||"
            return Token(ThisChar, filename, row, startColumn, endColumn, ch2str(ThisChar), 0);
    }
}

void Lex::nextLine() {
    row++;
    endColumn = 0;
    startColumn = 0;
}
