/********************************************
* Decl.h                              *
* Created by zt on 2020/6/4
* Modified by gyj on 2020/6/14
* modified by lugf027 on 2020/6/20.            *
*********************************************/

#ifndef COMPILER_DECL_H
#define COMPILER_DECL_H

#include <string>
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

    Expr **subs;

    Expr *initVal;

    int isConstant;

    SourceLocation identLoc, assignLoc;

public:
    VarDef(std::string ident, Expr **subs, Expr *initVal, int isConstant, int isDefined, SourceLocation identLoc,
           SourceLocation assignLoc) : ident(std::move(ident)), subs(subs), initVal(initVal), isConstant(isConstant),
                                       identLoc(identLoc), assignLoc(assignLoc) {};

    std::string getIdent() { return ident; }

    Expr **getSubs() { return subs; }

    Expr *getInitVal() { return initVal; }

    int getIsConstant() const { return isConstant; }

    SourceLocation getIdentLoc() { return identLoc; }

    SourceLocation getAssignLoc() { return assignLoc; }

};

/**
 * declare variable, varDefs's data type can be known by varType
 */
class VarDecl : public Decl {
private:
    int varType;

    VarDef **varDefs;

    SourceLocation typeLoc;

public:
    VarDecl(int varType, VarDef **varDefs, SourceLocation typeLoc) :
            varType(varType), varDefs(varDefs), typeLoc(typeLoc) {};

    int getVarType() const { return varType; }

    VarDef **getVarDefs() { return varDefs; }

    SourceLocation getTypeLoc() { return typeLoc; }

};

/**
 * FuncFParam → BType Ident ['[' ']' { '[' Exp ']' }]
 */
class FuncFParam {
private:
    int varType;

    std::string ident;

    Expr **subs;

    SourceLocation typeLoc, identLoc, lBracketLoc, rBracketLoc;

public:
    FuncFParam(int varType, std::string ident, Expr **subs, SourceLocation typeLoc, SourceLocation identLoc,
               SourceLocation lBracketLoc, SourceLocation rBracketLoc) :
            varType(varType), ident(std::move(ident)), subs(subs), typeLoc(typeLoc), identLoc(identLoc),
            lBracketLoc(lBracketLoc), rBracketLoc(rBracketLoc) {};

    int getVarType() const { return varType; }

    std::string getIdent() { return ident; }

    Expr **getSubs() { return subs; }

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
    FuncFParam **funcFParams;

    SourceLocation lParenthesisLoc, rParenthesisLoc;

public:
    FuncFParams(FuncFParam **funcParams, SourceLocation lParenthesisLoc, SourceLocation rParenthesisLoc) :
            funcFParams(funcParams), lParenthesisLoc(lParenthesisLoc), rParenthesisLoc(rParenthesisLoc) {};

    FuncFParam **getFuncParams() { return funcFParams; }

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
