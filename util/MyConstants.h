/********************************************
* MyConstants.h                              *
* Created by lugf027 on 2020/6/19.            *
* Github: https://github.com/lugf027        *
*********************************************/

#ifndef COMPILER_MYCONSTANTS_H
#define COMPILER_MYCONSTANTS_H

// on/off
static int ifGenIR = 1;

// handle params from console
static const int PARAMS_PASS = 0;
static const int PARAMS_FILE_WRONG = 1;
static const int PARAMS_NUMBER_WRONG = 2;
static const int PARAMS_WORD_WRONG = 3;

static const char *PARAMS_S = "-S";
static const char *PARAMS_O = "-o";
static const char *PARAMS_O2 = "-O2";

static const int O2_TRUE = 1;
static const int O2_FALSE = 0;

// token
static const int TOKEN_EOF = -1;

static const int TOKEN_INT = -2;
static const int TOKEN_VOID = -3;
static const int TOKEN_CONST = -4;

static const int TOKEN_IF = -5;
static const int TOKEN_ELSE = -6;
static const int TOKEN_WHILE = -7;
static const int TOKEN_BREAK = -8;
static const int TOKEN_CONTINUE = -9;
static const int TOKEN_RETURN = -10;

static const int TOKEN_IDENTIFIER = -11;
static const int TOKEN_NUMBER = -12;

// keyword
static const std::string KEYWORD_INT = "int";
static const std::string KEYWORD_VOID = "void";
static const std::string KEYWORD_CONST = "const";

static const std::string KEYWORD_IF = "if";
static const std::string KEYWORD_ELSE = "else";
static const std::string KEYWORD_WHILE = "while";
static const std::string KEYWORD_BREAK = "break";
static const std::string KEYWORD_CONTINUE = "continue";
static const std::string KEYWORD_RETURN = "return";

// some important char, for we'll be easy to find where such char has been used
static const char CHAR_UNDERLINE = '_';
static const char CHAR_SLASH = '/';
static const char CHAR_STAR = '*';
static const char CHAR_N_BACKEND_SLASH = '\n';
static const char CHAR_R_BACKEND_SLASH = '\r';

#endif //COMPILER_MYCONSTANTS_H
