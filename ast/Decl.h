#ifndef COMPILER_DECL_H
#define COMPILER_DECL_H

#include <string>
#include <vector>

#include "SourceLocation.h"

class Expr;

class Stmt;

/**
 * declaration statement
 */
class Decl {

};

/**
 * define one (constant) variable (globally or locally).
 * can be known the type by VarDecl
 * and subs is not nullptr if INT_ARRAY
 */
class VarDef : public Decl {
private:
    std::string ident;

    std::vector<Expr> subs;

    Expr *initVal;

    SourceLocation identLoc, assignLoc;

public:
    VarDef(std::string ident, std::vector<Expr> subs, Expr *initVal, SourceLocation identLoc, SourceLocation assignLoc)
            : ident(std::move(ident)), subs(subs), initVal(initVal), identLoc(identLoc), assignLoc(assignLoc) {};

    std::string getIdent() { return ident; }

    std::vector<Expr> getSubs() { return subs; }

    Expr *getInitVal() { return initVal; }

    SourceLocation getIdentLoc() { return identLoc; }

    SourceLocation getAssignLoc() { return assignLoc; }

};

/**
 * declare variable, varDefs's data type can be known by varType
 * 变量声明 VarDecl → varType VarDef { ',' VarDef } ';'
 */
class VarDecl : public Decl {
private:
    int varType;

    std::vector<VarDef> varDefs;

    SourceLocation typeLoc;

public:
    VarDecl(int varType, std::vector<VarDef> varDefs, SourceLocation typeLoc) :
            varType(varType), varDefs(varDefs), typeLoc(typeLoc) {};

    int getVarType() const { return varType; }

    std::vector<VarDef> getVarDefs() { return varDefs; }

    SourceLocation getTypeLoc() { return typeLoc; }

};

/**
 * 常量声明 ConstDecl → 'const' VarDecl
 */
class ConstantDecl : public Decl {
private:
    VarDecl varDecl;

    SourceLocation constantLoc;

public:
    ConstantDecl(VarDecl varDecl, SourceLocation constantLoc) :
            varDecl(varDecl), constantLoc(constantLoc) {};

    VarDecl getVarDecl() { return varDecl; }

    SourceLocation getConstantLoc() { return constantLoc; }

};

/**
 * FuncFParam → BType Ident ['[' ']' { '[' Exp ']' }]
 */
class FuncFParam {
private:
    int varType;

    std::string ident;

    std::vector<Expr> subs;

    SourceLocation typeLoc, identLoc, lBracketLoc, rBracketLoc;

public:
    FuncFParam(int varType, std::string ident, std::vector<Expr> subs, SourceLocation typeLoc, SourceLocation identLoc,
               SourceLocation lBracketLoc, SourceLocation rBracketLoc) :
            varType(varType), ident(std::move(ident)), subs(subs), typeLoc(typeLoc), identLoc(identLoc),
            lBracketLoc(lBracketLoc), rBracketLoc(rBracketLoc) {};

    int getVarType() const { return varType; }

    std::string getIdent() { return ident; }

    std::vector<Expr> getSubs() { return subs; }

    SourceLocation getTypeLoc() { return typeLoc; }

    SourceLocation getIdentLoc() { return identLoc; }

    SourceLocation getLBracketLoc() { return lBracketLoc; }

    SourceLocation getRBracketLoc() { return rBracketLoc; }

};

/**
 * FuncFParams → FuncFParam { ',' FuncFParam }
 */
class FuncFParams {
private:
    std::vector<FuncFParam> funcFParams;

    SourceLocation lParenthesisLoc, rParenthesisLoc;

public:
    FuncFParams(std::vector<FuncFParam> funcParams, SourceLocation lParenthesisLoc, SourceLocation rParenthesisLoc) :
            funcFParams(funcParams), lParenthesisLoc(lParenthesisLoc), rParenthesisLoc(rParenthesisLoc) {};

    std::vector<FuncFParam> getFuncParams() { return funcFParams; }

    SourceLocation getLParenthesisLoc() { return lParenthesisLoc; }

    SourceLocation getRParenthesisLoc() { return rParenthesisLoc; }

};

/**
 * define one function: FuncDef → retType ident '(' [funcFParams] ')' blockStmts
 */
class FuncDef : public Decl {
private:
    int retType;

    std::string ident;

    FuncFParams funcFParams;

    Stmt *blockStmts;

    SourceLocation retLoc;

    SourceLocation identLoc;

public:
    FuncDef(int retType, std::string ident, FuncFParams funcFParams, Stmt *blockStmts, SourceLocation retLoc,
            SourceLocation identLoc) : retType(retType), ident(std::move(ident)), funcFParams(funcFParams),
                                       blockStmts(blockStmts), retLoc(retLoc), identLoc(identLoc) {};

    int getRetType() const { return retType; }

    std::string getIdent() { return ident; }

    FuncFParams getFuncFParams() { return funcFParams; }

    Stmt *getBlockStmts() { return blockStmts; }

    SourceLocation getRetLoc() { return retLoc; }

    SourceLocation getIdentLoc() { return identLoc; }

};


#endif //COMPILER_DECL_H
