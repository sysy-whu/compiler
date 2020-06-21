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
 * 语句块 Block -> '{' { BlockItem } '}'
 * 同时存在 语句块项 BlockItem -> 语句 Stmt -> 语句块 Block
 */
class BlockStmt : public Stmt {
private:
    std::vector<Stmt> stmts{};

    SourceLocation lBraceLoc, rBraceLoc;

public:
    BlockStmt(std::vector<Stmt> stmts, SourceLocation lBraceLoc, SourceLocation rBraceLoc) :
            stmts(stmts), lBraceLoc(lBraceLoc), rBraceLoc(rBraceLoc) {};

    SourceLocation getLBraceLoc() { return lBraceLoc; }

    SourceLocation getRBraceLoc() { return rBraceLoc; }

    std::vector<Stmt> getStmts() { return stmts; }

};

/**
 * 语句块项 BlockItem → ConstantDecl | VarDecl
 */
class DeclStmt : public Stmt {
public:
    Decl *decl;

    SourceLocation startLoc, endLoc;

public:
    DeclStmt(Decl *decl, SourceLocation startLoc, SourceLocation endLoc) :
            decl(decl), startLoc(startLoc), endLoc(endLoc) {};

    Decl *getDecl() const { return decl; }

    SourceLocation getStartLoc() const { return startLoc; }

    SourceLocation getEndLoc() const { return endLoc; }

};

/**
 * 运算语句 Stmt -> LVal '=' Expr ';'
 */
class OpStmt : public Stmt {
private:
    Expr *lValExpr, *rExpr;

    SourceLocation assignLoc;

public:
    OpStmt(Expr *lValExpr, Expr *rExpr, SourceLocation assignLoc) :
            lValExpr(lValExpr), rExpr(rExpr), assignLoc(assignLoc) {};

    Expr *getLValExpr() { return lValExpr; }

    Expr *getRExpr() { return rExpr; }

    SourceLocation getAssignLoc() { return assignLoc; }

};

/**
 * 调用语句或没啥意义的语句 Stmt -> [Expr] ';'
 */
class ExprStmt : public Stmt {
private:
    Expr *expr;

public:
    ExprStmt(Expr *expr) : expr(expr) {};

    Expr *getExpr() { return expr; }
};

/**
 * 控制语句 Stmt -> 'if' '( Cond ')' Stmt [ 'else' Stmt ]
 */
class IfStmt : public Stmt {
private:
    Expr *conditionExpr;

    Stmt thenStmt, elseStmt;

    /// Cond's position
    SourceLocation lParenthesisLoc, rParenthesisLoc;

public:
    IfStmt(Expr *condition, Stmt thenStmt, Stmt elseStmt, SourceLocation lParenthesisLoc,
           SourceLocation rParenthesisLoc) :
            conditionExpr(condition), thenStmt(thenStmt), elseStmt(elseStmt),
            lParenthesisLoc(lParenthesisLoc), rParenthesisLoc(rParenthesisLoc) {};

    Expr *getCondition() { return conditionExpr; }

    Stmt getThenStmt() { return thenStmt; }

    Stmt getElseStmt() { return elseStmt; }

    SourceLocation getLParenthesisLoc() { return lParenthesisLoc; }

    SourceLocation getRParenthesisLoc() { return rParenthesisLoc; }

};

/**
 * 循环语句 Stmt -> 'while' '(' Cond ')' Stmt
 */
class WhileStmt : public Stmt {
private:
    Expr *conditionExpr;

    Stmt bodyStmt;

    /// Cond's position
    SourceLocation lParenthesisLoc, rParenthesisLoc;

public:
    WhileStmt(Expr *conditionExpr, Stmt bodyStmt, SourceLocation lParenthesisLoc,
              SourceLocation rParenthesisLoc) :
            conditionExpr(conditionExpr), bodyStmt(bodyStmt), lParenthesisLoc(lParenthesisLoc),
            rParenthesisLoc(rParenthesisLoc) {};

    Expr *getConditionExpr() { return conditionExpr; }

    Stmt getBodyStmt() { return bodyStmt; }

    SourceLocation getLParenthesisLoc() { return lParenthesisLoc; }

    SourceLocation getRParenthesisLoc() { return rParenthesisLoc; }

};

/**
 * 跳转语句 Stmt -> 'break' ';'
 */
class BreakStmt : public Stmt {
private:
    SourceLocation breakLoc;

public:
    BreakStmt(SourceLocation breakLoc) : breakLoc(breakLoc) {};

    SourceLocation getBreakLoc() { return breakLoc; }

};

/**
 * 跳转语句 Stmt -> 'continue' ';'
 */
class ContinueStmt : public Stmt {
private:
    SourceLocation continueLoc;

public:
    ContinueStmt(SourceLocation continueLoc) : continueLoc(continueLoc) {};

    SourceLocation getContinueLoc() { return continueLoc; }

};

/**
 * 跳转语句 Stmt -> 'return' [Exp] ';'
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
