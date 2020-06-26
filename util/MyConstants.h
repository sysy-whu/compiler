#ifndef COMPILER_MYCONSTANTS_H
#define COMPILER_MYCONSTANTS_H

/// on/off
static int ifGenIR = 1;

/// handle params from console
static const int PARAMS_PASS = 0;
static const int PARAMS_FILE_WRONG = 1;
static const int PARAMS_NUMBER_WRONG = 2;
static const int PARAMS_WORD_WRONG = 3;

static const char *PARAMS_S = "-S";
static const char *PARAMS_O = "-o";
static const char *PARAMS_O1 = "-O1";

static const int O1_TRUE = 1;
static const int O1_FALSE = 0;

/// token (token types)
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
static const int TOKEN_STR = -13;

/// keyword
static const std::string KEYWORD_INT = "int";
static const std::string KEYWORD_VOID = "void";
static const std::string KEYWORD_CONST = "const";

static const std::string KEYWORD_IF = "if";
static const std::string KEYWORD_ELSE = "else";
static const std::string KEYWORD_WHILE = "while";
static const std::string KEYWORD_BREAK = "break";
static const std::string KEYWORD_CONTINUE = "continue";
static const std::string KEYWORD_RETURN = "return";

/// some important char, it's mostly used in lexer for now.
/// and we'll be easy to find where such char has been used
static const char CHAR_UNDERLINE = '_';
static const char CHAR_SLASH = '/';
static const char CHAR_STAR = '*';
static const char CHAR_N_BACKEND_SLASH = '\n';
static const char CHAR_R_BACKEND_SLASH = '\r';
static const char CHAR_QUOTE_DOUBLE = '\"';
static const char CHAR_QUOTE_SINGLE = '\'';

static const char CHAR_GREATER = '>';
static const char CHAR_LOWER = '<';
static const char CHAR_NOT = '!';
static const char CHAR_AND = '&';
static const char CHAR_OR = '|';
static const char CHAR_EQ = '=';

static const char CHAR_ADD = '+';
static const char CHAR_SUB = '-';
static const char CHAR_MUL = '*';
//static const char CHAR_DIV = '/'; // CHAR_SLASH
static const char CHAR_REM = '%';

// (token types)
static const char CHAR_COMMA = ',';
static const char CHAR_SEPARATOR = ';';
static const char CHAR_L_BRACE = '{';
static const char CHAR_R_BRACE = '}';
static const char CHAR_L_BRACKET = '[';
static const char CHAR_R_BRACKET = ']';
static const char CHAR_L_PARENTHESIS = '(';
static const char CHAR_R_PARENTHESIS = ')';


/// some symbols for operation  (token types)
static const int OP_ASSIGN = -100;

static const int OP_BO_ADD = -101;
static const int OP_BO_SUB = -102;
static const int OP_BO_MUL = -103;
static const int OP_BO_DIV = -104;
static const int OP_BO_REM = -105;

static const int OP_BO_GT = -106;
static const int OP_BO_LT = -107;
static const int OP_BO_OR = -108;
static const int OP_BO_EQ = -109;
static const int OP_BO_AND = -110;
static const int OP_BO_NEQ = -111;
static const int OP_BO_GTE = -112;
static const int OP_BO_LTE = -113;

static const int OP_UO_NOT = -114;
//static const int OP_UO_POS = -115;  // OP_BO_ADD
//static const int OP_UO_NEG = -116;  // OP_BO_SUB

// data type
static const std::string TYPE_INT_STR = "int";
static const std::string TYPE_VOID_STR = "void";

static const int TYPE_INT = -1;
static const int TYPE_INT_STAR = -2;
static const int TYPE_VOID = -3;
static const int TYPE_STR = -4;

static const int DATA_IS_CONSTANT = 1;
static const int DATA_NOT_CONSTANT = 0;

// symbolTableType
static const int SYMBOL_TABLE_EXTERN = -20;
static const int SYMBOL_TABLE_GLOBAL = -21;
static const int SYMBOL_TABLE_LOCAL = -22;

// Stmt type
static const int STMT_LVAL_ASSIGN = -31;
static const int STMT_EXP = -31;
static const int STMT_EXP_BLANK = -31;
static const int STMT_BLOCK = -31;
static const int STMT_IF = -31;
static const int STMT_WHILE = -31;
static const int STMT_BREAK = -31;
static const int STMT_CONTINUE = -31;
static const int STMT_RETURN = -31;


#endif //COMPILER_MYCONSTANTS_H
