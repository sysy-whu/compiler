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

    ///===--------------------------------------------------------------===///
    /// 变量定义
    ///===--------------------------------------------------------------===///
    ConstDecl *parseConstDecl();
//    void parseConstDecl(ConstDecl constDecl);
//    void parseConstDecl(Decl &decl);

    ConstDef *parseConstDef();

    ConstInitVal *parseConstInitVal();

    VarDecl *parseVarDecl();

//    VarDecl *parseVarDecl(int varType, const char *ident);

    VarDef *parseVarDef();

    InitVal *parseInitVal();

    ///===--------------------------------------------------------------===///
    /// 函数定义
    ///===--------------------------------------------------------------===///
    FuncDef *parseFuncDef();

//    FuncDef *parseFuncDef(int funcType, const char *ident);

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
    Parse();

    ///===--------------------------------------------------------------===///
    /// 根节点
    ///===--------------------------------------------------------------===///
    void parseAST(AST &ast);
};

#endif //COMPILER_PARSE_H
