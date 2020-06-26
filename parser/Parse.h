#ifndef COMPILER_PARSE_H
#define COMPILER_PARSE_H

#include "../ast/AST.h"
#include "../lexer/Token.h"
#include "../lexer/Lex.h"

class Parse {
private:
    ///===--------------------------------------------------------------===///
    /// 成员变量
    ///===--------------------------------------------------------------===///
    /// 游标
    int step{};
    /// 词法分析器
//    Lex lex;
    /// token流
    std::vector<Token> tokens;
    /// AST
    AST *ast;

    ///===--------------------------------------------------------------===///
    /// 变量定义
    ///===--------------------------------------------------------------===///
    ConstDecl *parseConstDecl();

    ConstDef *parseConstDef();

    ConstInitVal *parseConstInitVal();

    VarDecl *parseVarDecl();

    VarDef *parseVarDef();

    InitVal *parseInitVal();

    ///===--------------------------------------------------------------===///
    /// 函数定义
    ///===--------------------------------------------------------------===///
    FuncDef *parseFuncDef();

    FuncFParams *parseFuncFParams();

    FuncFParam *parseFuncFParam();

    ///===--------------------------------------------------------------===///
    /// 语句
    ///===--------------------------------------------------------------===///
    Block *parseBlock();

    BlockItem *parseBlockItem();

    Stmt *parseStmt();

    ///===--------------------------------------------------------------===///
    /// 表达式
    ///===--------------------------------------------------------------===///
    Exp *parseExp();

    Cond *parseCond();

    LVal *parseLVal();

    PrimaryExp *parsePrimaryExp();

    UnaryExp *parseUnaryExp();

    MulExp *parseMulExp();

    AddExp *parseAddExp();

    RelExp *parseRelExp();

    EqExp *parseEqExp();

    LAndExp *parseLAndExp();

    LOrExp *parseLOrExp();

    ConstExp *parseConstExp();

    FuncRParams *parseFuncRParams();

public:
    /// 构造
    Parse();

    ///===--------------------------------------------------------------===///
    /// 根节点
    ///===--------------------------------------------------------------===///
    void parseAST();

    AST getAST() { return *ast; }
};

#endif //COMPILER_PARSE_H
