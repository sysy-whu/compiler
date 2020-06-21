/********************************************
* Parse.h                              *
* Created by gyj on 2020/6/5.
* Modified by lugf027 on 2020/6/21.            *
*********************************************/

#ifndef COMPILER_PARSE_H
#define COMPILER_PARSE_H

#include <iostream>
#include <vector>

#include "../ast/Decl.h"
#include "../lexer/Token.h"
#include "../lexer/Lex.h"
#include "../ast/Expr.h"
#include "../ast/Stmt.h"

class Parse {
private:
    int step;

    Lex lex;

    std::vector<Token> tokens;

    std::vector<Decl> decls;

    Decl parseFuncOrVar();

    FuncDef parseFunc();

    FuncDef parseFunc(const std::string &ident, int retType, SourceLocation retTypeLoc, SourceLocation identLoc);

    FuncFParams parseFunFParams();

    FuncFParam parseFuncFParam();

    Stmt parseBlockStmts();

    VarDecl parseVarDecl();

    ConstantDecl parseConstantDecl();

    VarDecl parseVarDecl(std::string ident, SourceLocation typeLoc, SourceLocation firstIdentLoc);

    std::vector<VarDef> parseVarDef(std::string firstIdent, SourceLocation firstIdentLoc);

    VarDef parseOneVarDef();

    VarDef parseOneVarDef(const std::string &ident, SourceLocation identLoc);

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

    ArrayInitListExpr parseArrayInitListExpr();

    CallExpr parseCallExpr();

    FuncRParams parseFuncRParams();

public:
    Parse();

    std::vector<Decl> getDeclsAST() {
        startParse();
        return decls;
    };

    void startParse();


};


#endif //COMPILER_PARSE_H
