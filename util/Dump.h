#ifndef COMPILER_DUMP_H
#define COMPILER_DUMP_H

#include "../ast/AST.h"
#include "../lexer/Token.h"

class Dump {
public:
    static void dump(Token *token, int level);

    static void dump(AST *ast, int level);

    static void dump(ConstDecl *constDecl, int level);

    static void dump(ConstDef *constDef, int level);

    static void dump(ConstInitVal *constInitVal, int level);

    static void dump(VarDecl *varDecl, int level);

    static void dump(VarDef *varDef, int level);

    static void dump(InitVal *initVal, int level);

    static void dump(FuncDef *funcDef, int level);

    static void dump(FuncFParams *funcFParams, int level);

    static void dump(FuncFParam *funcFParam, int level);

    static void dump(Stmt *stmt, int level);

    static void dump(Block *block, int level);

    static void dump(BlockItem *blockItem, int level);

    static void dump(ConstExp *constExp, int level);

    static void dump(Exp *exp, int level);

    static void dump(Cond *cond, int level);

    static void dump(LOrExp *lOrExp, int level);

    static void dump(LAndExp *lAndExp, int level);

    static void dump(EqExp *eqExp, int level);

    static void dump(RelExp *relExp, int level);

    static void dump(AddExp *addExp, int level);

    static void dump(MulExp *mulExp, int level);

    static void dump(UnaryExp *unaryExp, int level);

    static void dump(PrimaryExp *primaryExp, int level);

    static void dump(LVal *lVal, int level);

    static void dump(FuncRParams *funcRParams, int level);
};

#endif //COMPILER_DUMP_H
