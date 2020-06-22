#ifndef COMPILER_EXPR_H
#define COMPILER_EXPR_H

#include <string>
#include <utility>
#include "Stmt.h"
#include "SourceLocation.h"

/**
 * represents expression
 */
class Expr : public Stmt {

};

/**
 * a integer type Number, like 1
 */
class NumberExpr : public Expr {
private:
    int value;

    SourceLocation numLoc;

public:
    NumberExpr(int value, SourceLocation numLoc) :
            value(value), numLoc(numLoc) {};

    int getValue() const { return value; }

    SourceLocation getNumLoc() { return numLoc; }

};

/**
 * 变量初值 InitVal → Exp | '{' [ InitVal { ',' InitVal } ] '}'
 */
class ArrayInitListExpr : public Expr {
private:
    std::vector<Expr> items;

    SourceLocation lBraceLoc, rBraceLoc;

public:
    ArrayInitListExpr(std::vector<Expr> items, SourceLocation lBraceLoc, SourceLocation rBraceLoc) :
            items(items), lBraceLoc(lBraceLoc), rBraceLoc(rBraceLoc) {};

    std::vector<Expr> getItems() { return items; }

    SourceLocation getLBraceLoc() { return lBraceLoc; }

    SourceLocation getRBraceLoc() { return rBraceLoc; }
};

/**
 * LVal ident('['sub']')* like a, b[2][3]
 */
class LValExpr : public Expr {
private:
    std::string ident;

    std::vector<Expr> subs;

    SourceLocation identLoc, firstBracketLoc, lastBracketLoc;

public:
    LValExpr(std::string ident, std::vector<Expr> subs, SourceLocation identLoc, SourceLocation firstBracketLoc,
             SourceLocation lastBracketLoc) :
            ident(std::move(ident)), subs(subs), identLoc(identLoc), firstBracketLoc(firstBracketLoc),
            lastBracketLoc(lastBracketLoc) {};

    std::string getIdent() { return ident; }

    std::vector<Expr> getSubs() { return subs; }

    SourceLocation getIdentLoc() { return identLoc; }

    SourceLocation getFirstBracketLoc() { return firstBracketLoc; }

    SourceLocation getLastBracketLoc() { return lastBracketLoc; }

};

/**
 * 函数实参表 FuncRParams → Exp { ',' Exp }
 */
class FuncRParams {
private:
    std::vector<Expr> args;

    SourceLocation lParenthesisLoc, rParenthesisLoc;

public:
    FuncRParams(std::vector<Expr> args, SourceLocation lParenthesisLoc, SourceLocation rParenthesisLoc) :
            args(args), lParenthesisLoc(lParenthesisLoc), rParenthesisLoc(rParenthesisLoc) {};

    std::vector<Expr> getArgs() { return args; }

    SourceLocation getLParenthesisLoc() { return lParenthesisLoc; }

    SourceLocation getRParenthesisLoc() { return rParenthesisLoc; }

};

/**
 * 函数调用一元表达式 Ident '(' [FuncRParams] ')'
 */
class CallExpr : public Expr {
private:
    std::string ident;

    FuncRParams funcArgExpr;

    SourceLocation callLoc;

public:
    CallExpr(std::string ident, FuncRParams funcArgExpr, SourceLocation callLoc) :
            ident(std::move(ident)), funcArgExpr(funcArgExpr), callLoc(callLoc) {};

    std::string getIdent() { return ident; }

    FuncRParams getFuncArgExpr() { return funcArgExpr; }

    SourceLocation getCallLoc() { return callLoc; }

};

/**
 * 一元表达式 UnaryExp → '(' Exp ')' | LValExpr | NumberExpr | CallExpr | UnaryOpType UnaryExp
 */
class UnaryExpr : public Expr {
private:
    Expr expr;

    int opType;

    SourceLocation opLoc, lParenthesisLoc, rParenthesisLoc;

public:
    /**
     * 一元表达式构造函数
     * @param expr Expr
     * @param opType 表 UnaryExpr -> UnaryOpType UnaryExp
     * @param uOpLoc 表 UnaryExpr -> UnaryOpType UnaryExp
     * @param lParenthesisLoc 表 UnaryExp -> '(' Exp ')'
     * @param rParenthesisLoc 表 UnaryExp -> '(' Exp ')'
     */
    UnaryExpr(Expr expr, int opType, SourceLocation uOpLoc, SourceLocation lParenthesisLoc,
              SourceLocation rParenthesisLoc) :
            expr(expr), opType(opType), opLoc(uOpLoc), lParenthesisLoc(lParenthesisLoc),
            rParenthesisLoc(rParenthesisLoc) {};

    Expr getExpr() { return expr; }

    int getOpType() const { return opType; }

    SourceLocation getOpLoc() { return opLoc; }

    SourceLocation getLParenthesisLoc() { return lParenthesisLoc; }

    SourceLocation getRParenthesisLoc() { return rParenthesisLoc; }

};

/**
 * 乘除模表达式 MulExpr → UnaryExpr | MulExpr ('*' | '/' | '%') UnaryExpr
 */
class MulExpr : public Expr {
private:
    UnaryExpr unaryExpr;
    MulExpr *mulExpr;
    int opType;
    SourceLocation opLoc;

public:
    MulExpr(UnaryExpr unaryExpr, MulExpr *mulExpr, int opType, SourceLocation opLoc) :
            unaryExpr(unaryExpr), mulExpr(mulExpr), opType(opType), opLoc(opLoc) {};

    UnaryExpr getUnaryExpr() { return unaryExpr; }

    MulExpr *getMulExpr() { return mulExpr; }

    int getOpType() const { return opType; }

    SourceLocation getOpLoc() { return opLoc; }

};

/**
 * 加减表达式 AddExpr → MulExpr | AddExpr ('+' | '−') MulExpr
 */
class AddExpr : public Expr {
private:
    MulExpr mulExpr;

    AddExpr *addExpr;

    int opType;

    SourceLocation opLoc;

public:
    AddExpr(MulExpr mulExpr, AddExpr *addExpr, int opType, SourceLocation opLoc) :
            addExpr(addExpr), mulExpr(mulExpr), opType(opType), opLoc(opLoc) {};

    MulExpr getMulExpr() { return mulExpr; }

    AddExpr *getAddExpr() { return addExpr; }

    int getOpType() const { return opType; }

    SourceLocation getOpLoc() { return opLoc; }

};

/**
 * 关系表达式 RelExpr → AddExpr | RelExpr ('<' | '>' | '<=' | '>=') AddExpr
 */
class RelExpr : public Expr {
private:
    RelExpr *relExpr;

    AddExpr addExpr;

    int opType;

    SourceLocation opLoc;

public:
    RelExpr(AddExpr addExpr, RelExpr *relExpr, int opType, SourceLocation opLoc) :
            addExpr(addExpr), relExpr(relExpr), opType(opType), opLoc(opLoc) {};

    AddExpr getAddExpr() { return addExpr; }

    RelExpr *geRelExpr() { return relExpr; }

    int getOpType() const { return opType; }

    SourceLocation getOpLoc() { return opLoc; }

};

/**
 * 相等性表达式 EqExpr → RelExpr | EqExp ('==' | '!=') RelExpr
 */
class EqExpr : public Expr {
private:
    RelExpr relExpr;

    EqExpr *eqExpr;

    int opType;

    SourceLocation opLoc;

public:
    EqExpr(RelExpr relExpr, EqExpr *eqExpr, int opType, SourceLocation opLoc) :
            relExpr(relExpr), eqExpr(eqExpr), opType(opType), opLoc(opLoc) {};

    EqExpr *getEqExpr() { return eqExpr; }

    RelExpr getRelExpr() { return relExpr; }

    int getOpType() const { return opType; }

    SourceLocation getOpLoc() { return opLoc; }

};

/**
 * 逻辑与表达式 LAndExpr → EqExpr | LAndExpr '&&' EqExpr
 */
class LAndExpr : public Expr {
private:
    EqExpr eqExpr;

    LAndExpr *lAndExpr;

    SourceLocation opLoc;

public:
    LAndExpr(EqExpr eqExpr, LAndExpr *lAndExpr, SourceLocation opLoc) :
            eqExpr(eqExpr), lAndExpr(lAndExpr), opLoc(opLoc) {};

    EqExpr getEqExpr() { return eqExpr; }

    LAndExpr *getLAndExpr() { return lAndExpr; }

    SourceLocation getOpLoc() { return opLoc; }
};

/**
 * 逻辑或表达式 LOrExpr → LAndExpr | LOrExpr '||' LAndExpr
 */
class LOrExpr : public Expr {
private:
    LAndExpr lAndExpr;

    LOrExpr *lOrExpr;

    SourceLocation opLoc;

public:
    LOrExpr(LAndExpr lAndExpr, LOrExpr *lOrExpr, SourceLocation opLoc) :
            lAndExpr(lAndExpr), lOrExpr(lOrExpr), opLoc(opLoc) {};

    LAndExpr getLAndExpr() { return lAndExpr; }

    LOrExpr *getLOrExpr() { return lOrExpr; }

    SourceLocation getOpLoc() { return opLoc; }
};

/**
 * Binary Operation Expression -> express priority by relation in tree
 */
//class BinaryExpr : public Expr {
//private:
//    Expr lExpr, rExpr;
//
//    int opType;
//
//    SourceLocation opType;
//
//public:
//    BinaryExpr(Expr lExpr, Expr rExpr, int opType, SourceLocation opType) :
//            lExpr(lExpr), rExpr(rExpr), opType(opType), opType(opType) {};
//
//    Expr getLExpr() { return lExpr; }
//
//    Expr getRExpr() { return rExpr; }
//
//    int getOpType() { return opType; }
//
//    SourceLocation getBOpLoc() { return opType; }
//
//};

#endif //COMPILER_EXPR_H
