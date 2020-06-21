/********************************************
* Stmt.h                              *
* Created by zt on 2020/6/4
* Modified by gyj on 2020/6/14
* modified by lugf027 on 2020/6/20.            *
*********************************************/

#ifndef COMPILER_STMT_H
#define COMPILER_STMT_H

#include "SourceLocation.h"

class Decl;

class Expr;

/**
 * statement/sentence
 */
class Stmt {

};


/**
 * { stmt [stmt]* }
 */
class BlockStmt final : public Stmt {
private:
    Stmt **stmts{};

    SourceLocation lBraceLoc, rBraceLoc;

public:
//    BlockStmt()= default;

    BlockStmt(Stmt **stmts, SourceLocation lBraceLoc, SourceLocation rBraceLoc) :
            stmts(stmts), lBraceLoc(lBraceLoc), rBraceLoc(rBraceLoc) {};

    SourceLocation getLBraceLoc() { return lBraceLoc; }

    SourceLocation getRBraceLoc() { return rBraceLoc; }

    Stmt **getStmts() { return stmts; }

};

/**
 * Declare
 */
class DeclStmt : public Stmt {
public:
    Decl *decl;

    SourceLocation startLoc, endLoc;

public:
//    DeclStmt()= default;

    DeclStmt(Decl *decl, SourceLocation startLoc, SourceLocation endLoc) :
            decl(decl), startLoc(startLoc), endLoc(endLoc) {};

    Decl *getDecl() const { return decl; }

    SourceLocation getStartLoc() const { return startLoc; }

    SourceLocation getEndLoc() const { return endLoc; }

};

/**
 * if ( conditionExpr ) then { BlockStmt } [else { BlockStmt }]
 */
class IfStmt : public Stmt {
private:
    Expr *conditionExpr;

    Stmt thenStmt, elseStmt;

    int hasVar;

    SourceLocation lParenthesisLoc, rParenthesisLoc;

public:
//    IfStmt() = default;

    IfStmt(Expr *condition, Stmt thenStmt, Stmt elseStmt, int hasVar, SourceLocation lParenthesisLoc,
           SourceLocation rParenthesisLoc) :
            conditionExpr(condition), thenStmt(thenStmt), elseStmt(elseStmt), hasVar(hasVar),
            lParenthesisLoc(lParenthesisLoc), rParenthesisLoc(rParenthesisLoc) {};

    Expr *getCondition() { return conditionExpr; }

    Stmt getThenStmt() { return thenStmt; }

    Stmt getElseStmt() { return elseStmt; }

    int getHasVar() const { return hasVar; }

    void setHasVar(int ifHasVar) { this->hasVar = ifHasVar; }

    SourceLocation getLParenthesisLoc() { return lParenthesisLoc; }

    SourceLocation getRParenthesisLoc() { return rParenthesisLoc; }

};

/**
 * while( conditionExpr ) { bodyStmt }
 */
class WhileStmt : public Stmt {
private:
    Expr *conditionExpr;

    Stmt bodyStmt;

    int hasVar;
public:
//    WhileStmt() = default;

    WhileStmt(Expr *conditionExpr, Stmt bodyStmt, int hasVar) :
            conditionExpr(conditionExpr), bodyStmt(bodyStmt), hasVar(hasVar) {};

    Expr *getConditionExpr() { return conditionExpr; }

    Stmt getBodyStmt() { return bodyStmt; }

    int getHasVar() const { return hasVar; }

    void setHasVar(int ifHasVar) { this->hasVar = ifHasVar; }

};

/**
 * break;
 */
class BreakStmt : public Stmt {
private:
    SourceLocation breakLoc;

public:
//    BreakStmt() = default;

    BreakStmt(SourceLocation breakLoc) : breakLoc(breakLoc) {};

    SourceLocation getBreakLoc() { return breakLoc; }

};

/**
 * return retExpr;
 */
class ReturnExpr : public Stmt {
private:
    Stmt retExpr;

    SourceLocation retLoc;

public:
    ReturnExpr() = default;

    ReturnExpr(Stmt retExpr, SourceLocation retLoc) :
            retExpr(retExpr), retLoc(retLoc) {};

    Stmt getRetExpr() { return retExpr; }

    SourceLocation gerRetLoc() { return retLoc; }

};

#endif //COMPILER_STMT_H
