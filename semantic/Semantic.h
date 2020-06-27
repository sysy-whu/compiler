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

    int semanticStmt(Stmt *stmt);

    int semanticBlock(Block *block);

    int semanticBlockItem(BlockItem *blockItem);

    ///===-----------------------------------------------------------------------===///
    /// Expr 表达式
    ///===-----------------------------------------------------------------------===///
    std::vector<int> *calConstArrayInitVals(ConstInitVal *constInitVal, std::vector<int> *subs);
    int calConstExp(ConstExp *constExp);

    int calAddExp(AddExp *addExp);

    int calMulExp(MulExp *mulExp);

    int calUnaryExp(UnaryExp *unaryExp);

    int calPrimaryExp(PrimaryExp *primaryExp);

    int calLVal(LVal *lVal);

    int semanticAddExp(AddExp *addExp);

    int semanticLOrExp(LOrExp *lOrExp);

public:
    Semantic();

    void startSemantic();

    IRTree *getIRTree() { return irTree; }
};


#endif //COMPILER_SEMANTIC_H
