#ifndef COMPILER_SEMANTIC_H
#define COMPILER_SEMANTIC_H

#include "SymbolTable.h"
#include "../arm/ArmGlobal.h"
#include "../parser/Parse.h"

class Semantic {
private:
    /// 抽象语法树->语法分析器结果
    AST *ast;
    /// 符号表（加强版）
    std::vector<SymbolTable *> *symbolTables;
    /// 汇编语法树
    Arm7Tree *armTree;

    int levelNow;

    std::string funcNameNow;

    std::vector<Arm7Var *> *funcParamNow;

    ///===-----------------------------------------------------------------------===///
    /// 静态库文件里面的函数
    ///===-----------------------------------------------------------------------===///
    void setSyLib_H();

    ///===-----------------------------------------------------------------------===///
    /// 分析 ast 得到符号表相关内容
    ///===-----------------------------------------------------------------------===///
    void semanticArm7Var(Decl *decl, std::vector<Symbol *> *symbols);

    void semanticArm7Var(BlockItem *blockItem, std::vector<Symbol *> *symbols);

    Arm7Func *semanticArm7Func(FuncDef *funcDef);

    ///===-----------------------------------------------------------------------===///
    /// Stmt 语句
    ///===-----------------------------------------------------------------------===///
    void semanticBlock(Block *block, int funcRetType, std::vector<Arm7Var *> *params);

    void semanticStmt(Stmt *stmt, int funcRetType);

    ///===-----------------------------------------------------------------------===///
    /// Expr 表达式 CalConst
    /// @param one kind of Exp or initVal
    /// @return const value of that Exp or initVal
    ///===-----------------------------------------------------------------------===///

    std::vector<int> *calConstArrayInitVals(ConstInitVal *constInitVal, std::vector<int> *subs);

    std::vector<int> *calVarArrayInitVals(InitVal *initVal, std::vector<int> *subs);

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

    void checkRParams(FuncRParams *funcRParams, Arm7Func *arm7Func);

    void checkExps(std::vector<Exp *> *exps, const char *errMsg);

public:
    /// 构造方法
    Semantic();

    ///===-----------------------------------------------------------------------===///
    /// startSemantic 启动接口
    ///===-----------------------------------------------------------------------===///

    void startSemantic();

    Arm7Tree *getArm7Tree() { return armTree; }

};


#endif //COMPILER_SEMANTIC_H
