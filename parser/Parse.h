#ifndef COMPILER_PARSE_H
#define COMPILER_PARSE_H

#include <iostream>
#include <vector>

#include "../ast/Decl.h"
#include "../lexer/Token.h"
#include "../lexer/Lex.h"
#include "../ast/Expr.h"
#include "../ast/Stmt.h"

/**
 * 语法分析器 -> 未测试！
 */
class Parse {
private:
    /// 游标
    int step;
    /// 词法分析器
    Lex lex;
    /// token流
    std::vector<Token> tokens;
    /// 要返回的AST
    std::vector<Decl> decls;

    ///===-----------------------------------------------------------------------------------===///
    /// 内部启动入口
    ///===-----------------------------------------------------------------------------------===///

    void startParse();

    Decl parseFuncOrVar();

    ///===-----------------------------------------------------------------------------------===///
    /// Function
    ///===-----------------------------------------------------------------------------------===///

    FuncDef parseFunc();

    FuncDef parseFunc(const std::string &ident, int retType, SourceLocation retTypeLoc, SourceLocation identLoc);

    FuncFParams parseFunFParams();

    FuncFParam parseFuncFParam();

    ///===-----------------------------------------------------------------------------------===///
    /// Variable
    ///===-----------------------------------------------------------------------------------===///

    VarDecl parseVarDecl();

    ConstantDecl parseConstantDecl();

    VarDecl parseVarDecl(const std::string &ident, SourceLocation typeLoc, SourceLocation firstIdentLoc);

    std::vector<VarDef> parseVarDef(const std::string &firstIdent, SourceLocation firstIdentLoc);

    VarDef parseOneVarDef();

    VarDef parseOneVarDef(const std::string &ident, SourceLocation identLoc);

    ///===-----------------------------------------------------------------------------------===///
    /// Expression
    ///===-----------------------------------------------------------------------------------===///

    Expr parseExpr();

    AddExpr parseAddExpr();

    MulExpr parseMulExpr();

    UnaryExpr parseUnaryExpr();

    LValExpr parseLValExpr();

    NumberExpr parseNumberExpr();

    LOrExpr parseLOrExpr();

    LAndExpr parseLAndExpr();

    EqExpr parseEqExpr();

    RelExpr parseRelExpr();

    ArrayInitListExpr parseArrayInitListExpr(int needSep);

    CallExpr parseCallExpr();

    FuncRParams parseFuncRParams();

    ///===---------------------------------------------------------------------------------===///
    /// Stmts
    ///===---------------------------------------------------------------------------------===///
    Stmt parseBlockStmts();

    std::vector<Stmt> parseBlockItems();

    Stmt parseOneStmt();

public:
    Parse();

    /// 调用接口
    std::vector<Decl> getDeclsAST() {
        startParse();
        return decls;
    };

};


#endif //COMPILER_PARSE_H
