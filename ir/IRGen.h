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
    std::vector<std::string> *whilePos;
    std::vector<std::string> *whileEndPos;

    FuncFParams *funcFParamsNow;

    int levelNow;

    ///===-----------------------------------------------------------------------===///
    /// 基本声明定义
    ///===-----------------------------------------------------------------------===///

    void genVar(VarDef *varDef, std::vector<IRStmt *> *irStmts, const char *levelLoc, int allocaType, int &stepName);

    void genVarArray(VarDef *varDef, std::vector<IRStmt *> *irStmts, const char *levelLoc, int allocaType,
                     int &stepName);

    ConstVar *genConstVar(ConstDef *constDef, int constType, std::vector<IRStmt *> *irStmts,
                          const std::string &locType, int allocaType);

    ConstVarArray *genConstVarArray(ConstDef *constDef, int constType, std::vector<IRStmt *> *irStmts,
                                    const std::string &locType, int allocaType, int &stepName);

    void genFunc(FuncDef *funcDef, std::vector<IRLocalBlock *> *basicBlocks);

    ///===-----------------------------------------------------------------------===///
    /// 语句、语句块
    ///===-----------------------------------------------------------------------===///

    const char *genStmt(Stmt *stmt, std::vector<IRLocalBlock *> *basicBlocks, IRLocalBlock *lastBlock,
                        std::vector<IRStmt *> *lastBlockStmts, int &stepName);

    const char *genStmtAuxIf(Stmt *stmt, std::vector<IRLocalBlock *> *basicBlocks, IRLocalBlock *lastBlock,
                             std::vector<IRStmt *> *lastBlockStmts, int &stepName);

    const char *genStmtAuxWhile(Stmt *stmt, std::vector<IRLocalBlock *> *basicBlocks, IRLocalBlock *lastBlock,
                                std::vector<IRStmt *> *lastBlockStmts, int &stepName);

    const char *genBlock(Block *block, std::vector<IRLocalBlock *> *basicBlocks, IRLocalBlock *lastBlock,
                         std::vector<IRStmt *> *lastBlockStmts, int &stepName);

    ///===-----------------------------------------------------------------------===///
    /// 表达式 计算 不生成代码
    ///===-----------------------------------------------------------------------===///

    std::vector<int> *calConstArrayInitVals(ConstInitVal *constInitVal, std::vector<int> *subs,
                                            const std::string &locType, int allocaType);

    int calConstExp(ConstExp *constExp);

    int calAddExp(AddExp *addExp);

    int calMulExp(MulExp *mulExp);

    int calUnaryExp(UnaryExp *unaryExp);

    int calPrimaryExp(PrimaryExp *primaryExp);

    int calLVal(LVal *lVal);

    ///===-----------------------------------------------------------------------===///
    /// 表达式 不计算 生成代码
    ///===-----------------------------------------------------------------------===///

    std::vector<Exp *> *genVarArrayInitVals(InitVal *initVal, std::vector<int> *subs,
                                            std::vector<IRStmt *> *irStmts, const char *levelLoc, int &stepName);

    const char *genCondExp(Cond *cond, std::vector<IRStmt *> *irStmts, int &stepName);

    const char *genLOrExp(LOrExp *lOrExp, std::vector<IRStmt *> *irStmts, int &stepName);

    const char *genLAndExp(LAndExp *lAndExp, std::vector<IRStmt *> *irStmts, int &stepName);

    const char *genEqExp(EqExp *eqExp, std::vector<IRStmt *> *irStmts, int &stepName);

    const char *genRelExp(RelExp *relExp, std::vector<IRStmt *> *irStmts, int &stepName);

    const char *genAddExp(AddExp *addExp, std::vector<IRStmt *> *irStmts, int &stepName);

    const char *genMulExp(MulExp *mulExp, std::vector<IRStmt *> *irStmts, int &stepName);

    const char *genUnaryExp(UnaryExp *unaryExp, std::vector<IRStmt *> *irStmts, int &stepName);

    const char *genPrimaryExp(PrimaryExp *primaryExp, std::vector<IRStmt *> *irStmts, int &stepName);

    const char *genLVal(LVal *lVal, std::vector<IRStmt *> *irStmts, int &stepName);

public:
    IRGen();

    void startIrGen();

    [[nodiscard]] IRTree *getIrTree() { return irTree; };
};


#endif //COMPILER_IRGEN_H
