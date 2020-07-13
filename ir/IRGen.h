#ifndef COMPILER_IRGEN_H
#define COMPILER_IRGEN_H

#include <vector>

#include "../semantic/SymbolTable.h"
#include "../ast/AST.h"
#include "IRGlobal.h"

class IRGen {
private:
    /// 语法分析器返回结果
    AST *ast = nullptr;
    /// 符号表(未考虑多文件，但需要考虑静态链接头文件相应方法！)
    std::vector<SymbolTable *> *symbolTables;
    /// 可供返回的中间码
    IRTree *irTree = nullptr;
    // 临时命名(寄存器)
    int stepNameGlobal = 0;
    int stepNameLocal = 0;

    ///===-----------------------------------------------------------------------===///
    /// 基本声明定义
    ///===-----------------------------------------------------------------------===///

    void genVar(VarDef *varDef, int varType, std::vector<IRStmt *> *irStmts);

    void genVarArray(VarDef *varDef, int varType, std::vector<IRStmt *> *irStmts);

    ConstVar *genConstVar(ConstDef *constDef, int constType, std::vector<IRStmt *> *irStmts,
                          const std::string &locType);

    ConstVarArray *genConstVarArray(ConstDef *constDef, int constType, std::vector<IRStmt *> *irStmts,
                                    const std::string &locType);

    void genFunc(FuncDef *funcDef, std::vector<IRLocalBlock *> *basicBlocks);

    ///===-----------------------------------------------------------------------===///
    /// 语句、语句块
    ///===-----------------------------------------------------------------------===///

    void genStmt(Stmt *stmt, std::vector<IRLocalBlock *> *basicBlocks, IRLocalBlock *lastBlock,
                 std::vector<IRStmt *> *lastBlockStmts);

    void genBlock(Block *block, std::vector<IRLocalBlock *> *basicBlocks, IRLocalBlock *lastBlock,
                  std::vector<IRStmt *> *lastBlockStmts);

    ///===-----------------------------------------------------------------------===///
    /// 表达式 计算 不生成代码
    ///===-----------------------------------------------------------------------===///

    std::vector<int> *calConstArrayInitVals(ConstInitVal *constInitVal, std::vector<int> *subs);

    int calConstExp(ConstExp *constExp);

    int calAddExp(AddExp *addExp);

    int calMulExp(MulExp *mulExp);

    int calUnaryExp(UnaryExp *unaryExp);

    int calPrimaryExp(PrimaryExp *primaryExp);

    int calLVal(LVal *lVal);

    ///===-----------------------------------------------------------------------===///
    /// 表达式 不计算 生成代码
    ///===-----------------------------------------------------------------------===///

    std::vector<int> *genVarArrayInitVals(InitVal *initVal, std::vector<int> *subs,
                                               std::vector<IRStmt *> *irStmts, const char *levelLoc);

    const char *genCondExp(Cond *cond, std::vector<IRStmt *> *irStmts);

    const char *genLOrExp(LOrExp *lOrExp, std::vector<IRStmt *> *irStmts);

    const char *genLAndExp(LAndExp *lAndExp, std::vector<IRStmt *> *irStmts);

    const char *genEqExp(EqExp *eqExp, std::vector<IRStmt *> *irStmts);

    const char *genRelExp(RelExp *relExp, std::vector<IRStmt *> *irStmts);

    const char *genAddExp(AddExp *addExp, std::vector<IRStmt *> *irStmts);

    const char *genMulExp(MulExp *mulExp, std::vector<IRStmt *> *irStmts);

    const char *genUnaryExp(UnaryExp *unaryExp, std::vector<IRStmt *> *irStmts);

    const char *genPrimaryExp(PrimaryExp *primaryExp, std::vector<IRStmt *> *irStmts);

    const char *genLVal(LVal *lVal, std::vector<IRStmt *> *irStmts);

public:
    IRGen();

    void startIrGen();
};


#endif //COMPILER_IRGEN_H
