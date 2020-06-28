#ifndef COMPILER_SEMANTIC_H
#define COMPILER_SEMANTIC_H

#include "SymbolTable.h"
#include "../ir/IRGlobal.h"
#include "../parser/Parse.h"

class Semantic {
private:
    /// 语法分析器返回结果
    AST *ast = nullptr;
    /// 符号表(未考虑多文件，但需要考虑静态链接头文件相应方法！)
    std::vector<SymbolTable *> *symbolTables;
    /// 可供返回的中间码
    IRTree *irTree = nullptr;


    void setSyLib_H();

    ///===-----------------------------------------------------------------------===///
    /// 分析 ast 得到符号表相关内容
    ///===-----------------------------------------------------------------------===///

    Var *semanticVar(VarDef *varDef, int varType);

    VarArray *semanticVarArray(VarDef *varDef, int varType);

    ConstVar *semanticConstVar(ConstDef *constDef, int constType);

    ConstVarArray *semanticConstVarArray(ConstDef *constDef, int constType);

    Func *semanticFunc(FuncDef *funcDef);

    ///===-----------------------------------------------------------------------===///
    /// Stmt 语句
    ///===-----------------------------------------------------------------------===///

    void semanticStmt(Stmt *stmt, int funcRetType);

    void semanticBlock(Block *block, int funcRetType, std::vector<Symbol *> *paramSymbols);

    ///===-----------------------------------------------------------------------===///
    /// Expr 表达式 CalConst
    /// @param one kind of Exp or initVal
    /// @return const value of that Exp or initVal
    ///===-----------------------------------------------------------------------===///

    std::vector<int> *calConstArrayInitVals(ConstInitVal *constInitVal, std::vector<int> *subs);

    int calConstExp(ConstExp *constExp);

    int calAddExp(AddExp *addExp);

    int calMulExp(MulExp *mulExp);

    int calUnaryExp(UnaryExp *unaryExp);

    int calPrimaryExp(PrimaryExp *primaryExp);

    int calLVal(LVal *lVal);

    ///===-----------------------------------------------------------------------===///
    /// Expr 表达式 Semantic
    /// @param one kind of Exp
    /// @return ret type of that Exp
    ///===-----------------------------------------------------------------------===///

    std::vector<int> *semanticVarArrayInitVals(InitVal *initVal, std::vector<int> *subs);

    int semanticCondExp(Cond *cond);

    int semanticLOrExp(LOrExp *lOrExp);

    int semanticLAndExp(LAndExp *lAndExp);

    int semanticEqExp(EqExp *eqExp);

    int semanticRelExp(RelExp *relExp);

    int semanticAddExp(AddExp *addExp);

    int semanticMulExp(MulExp *mulExp);

    int semanticUnaryExp(UnaryExp *unaryExp);

    int semanticPrimaryExp(PrimaryExp *primaryExp);

    int semanticLVal(LVal *lVal);

    void checkRParams(FuncRParams *funcRParams, Func *func);

    void checkExps(std::vector<Exp *> *exps, const char *errMsg);

public:
    Semantic();

    void startSemantic();

    IRTree *getIRTree() { return irTree; }
};


#endif //COMPILER_SEMANTIC_H
