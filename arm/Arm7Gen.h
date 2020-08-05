#ifndef COMPILER_ARM7GEN_H
#define COMPILER_ARM7GEN_H

#include "../semantic/SymbolTable.h"
#include "../arm/ArmGlobal.h"
#include "../parser/Parse.h"
#include "ArmRegManager.h"

class BlockReg{
private:
    ArmBlock *armBlock;

    ArmReg *armReg;

public:
    BlockReg(ArmBlock *armBlock1, ArmReg *armReg1):armBlock(armBlock1), armReg(armReg1){};

    [[nodiscard]] ArmBlock *getArmBlock() const {
        return armBlock;
    }

    [[nodiscard]] ArmReg *getArmReg() const {
        return armReg;
    }
};

class Arm7Gen {
private:
    /// 抽象语法树->语法分析器结果
    AST *ast;
    /// 符号表（加强版）
    std::vector<SymbolTable *> *symbolTables;
    /// 汇编语法树
    Arm7Tree *armTree;
    /// 当前代码块 level 层次
    int levelNow;
    /// 当前函数名，用以变量查找
    std::string funcNameNow;
//    /// 当前函数形参参数表，用以变量查找
//    std::vector<Arm7Var *> *funcParamNow;
//    /// 当前符号表，用以检查复杂 level 下的变量查找
//    SymbolTable *symbolTableNow;
//    /// 局部变量内存分配用，semanticFunc 结束后，会赋给 Arm7Func
//    int capacity;
    /// break,continue用
    std::vector<std::string> *whilePos;
    std::vector<std::string> *whileEndPos;
    /// blockName 分配 如4: .L4
    int blockName;
    /// 寄存器管理
    ArmRegManager *armRegManager;

    std::vector<FuncInnerBlockAux *> *blockAuxs;

    ///===-----------------------------------------------------------------------===///
    /// 基本声明定义
    ///===-----------------------------------------------------------------------===///
    void genArm7Func(FuncDef *funcDef, std::vector<ArmBlock *> *armBlocks);

    ///===-----------------------------------------------------------------------===///
    /// 语句块中声明语句
    ///===-----------------------------------------------------------------------===///
    void genArm7Var(BlockItem *blockItem, std::vector<ArmStmt *> *armStmts);

    void genVar(VarDef *varDef, std::vector<ArmStmt *> *armStmts);

    void genConstVar(ConstDef *constDef, std::vector<ArmStmt *> *armStmts);

    void genVarArray(VarDef *varDef, std::vector<ArmStmt *> *armStmts);

    void genConstVarArray(ConstDef *constDef, std::vector<ArmStmt *> *armStmts);

    ///===-----------------------------------------------------------------------===///
    /// 语句、语句块
    ///===-----------------------------------------------------------------------===///

    ArmBlock *genBlock(Block *block, std::vector<ArmBlock *> *basicBlocks, ArmBlock *lastBlock,
                       std::vector<ArmStmt *> *lastBlockStmts);

//    ArmBlock *genStmt(Stmt *stmt, std::vector<ArmBlock *> *basicBlocks, ArmBlock *lastBlock,
//                      std::vector<ArmStmt *> *lastBlockStmts);
    /// 返回必须有时要分配但有分配的结束 labelBlock 块
    ArmBlock *genStmt(Stmt *stmt, std::vector<ArmBlock *> *basicBlocks, ArmBlock *lastBlock);

    void addArmRetStmts(std::vector<ArmStmt *> *ArmStmts);

//    ArmBlock *genStmtAuxIf(Stmt *stmt, std::vector<ArmBlock *> *basicBlocks, ArmBlock *lastBlock,
//                           std::vector<ArmStmt *> *lastBlockStmts);
//
//    ArmBlock *genStmtAuxWhile(Stmt *stmt, std::vector<ArmBlock *> *basicBlocks, ArmBlock *lastBlock,
//                              std::vector<ArmStmt *> *lastBlockStmts);

    /// 返回必须要分配但有分配的结束 labelBlock 块
    ArmBlock *genStmtAuxIf(Stmt *stmt, std::vector<ArmBlock *> *basicBlocks, ArmBlock *lastBlock);
    /// 返回必须要分配但有分配的结束 labelBlock 块
    ArmBlock *genStmtAuxWhile(Stmt *stmt, std::vector<ArmBlock *> *basicBlocks, ArmBlock *lastBlock);

    ///===-----------------------------------------------------------------------===///
    /// 表达式 计算 不生成代码
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
    /// 表达式 不计算 生成代码
    ///===-----------------------------------------------------------------------===///

    std::vector<Exp *> *genVarArrayInitVals(InitVal *initVal, std::vector<int> *subs);

//    ArmReg *genCondExp(Cond *cond, std::vector<ArmBlock *> *basicBlocks,
//                       ArmBlock *lastBlock, std::vector<ArmStmt *> *lastBlockStmts, std::string &newBlockName);
//
//    ArmReg *genLOrExp(LOrExp *lOrExp, std::vector<ArmBlock *> *basicBlocks,
//                      ArmBlock *lastBlock, std::vector<ArmStmt *> *lastBlockStmts, std::string &newBlockName);
//
//    ArmReg *genLAndExp(LAndExp *lAndExp, std::vector<ArmBlock *> *basicBlocks,
//                       ArmBlock *lastBlock, std::vector<ArmStmt *> *lastBlockStmts, std::string &newBlockName);
    /// 返回比较结果寄存器, 引用参数返回必分配且已经分配好的 labelBlock
    BlockReg *genLOrExp(LOrExp *lOrExp, std::vector<ArmBlock *> *basicBlocks, ArmBlock *lastBlock);
    /// 返回比较结果寄存器, 引用参数返回必分配且已经分配好的 labelBlock
    BlockReg *genLAndExp(LAndExp *lAndExp, std::vector<ArmBlock *> *basicBlocks, ArmBlock *lastBlock);

    ArmReg *genEqExp(EqExp *eqExp, std::vector<ArmStmt *> *ArmStmts);

    ArmReg *genRelExp(RelExp *relExp, std::vector<ArmStmt *> *ArmStmts);

    ArmReg *genAddExp(AddExp *addExp, std::vector<ArmStmt *> *ArmStmts);

    ArmReg *genMulExp(MulExp *mulExp, std::vector<ArmStmt *> *ArmStmts);

    ArmReg *genUnaryExp(UnaryExp *unaryExp, std::vector<ArmStmt *> *ArmStmts);

    ArmReg *genPrimaryExp(PrimaryExp *primaryExp, std::vector<ArmStmt *> *ArmStmts);

    ArmReg *genLVal(LVal *lVal, std::vector<ArmStmt *> *ArmStmts, int ifGetPos);


public:
    Arm7Gen();

    /**
     * 启动接口
     * @param ast_ 声明处以外的变量被转化为实际地址的 ast
     * @param symbolTables_ 符号表，只有两个元素：libFuncs、global; 生成 arm 时仅维护常量
     */
    void startGen(AST *ast_, std::vector<SymbolTable *> *symbolTables_);

    Arm7Tree *genArmTree() { return armTree; }
};


#endif //COMPILER_ARM7GEN_H
