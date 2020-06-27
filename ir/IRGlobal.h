#ifndef COMPILER_IRGLOBAL_H
#define COMPILER_IRGLOBAL_H

#include <utility>

#include "IRLocalBlock.h"
#include "IRLocalStmt.h"

/**
 * 全局变量类IRGlobalVar
 */
class IRGlobalVar {
private:
    IRLocalStmt *irLocalStmt;

public:
    /**
     * 全局变量类IRGlobalVar构造方法
     * @param irLocalStmt 变量语句
     */
    explicit IRGlobalVar(IRLocalStmt *irLocalStmt) : irLocalStmt(irLocalStmt) {};

    IRLocalStmt *getIrLocalStmt() { return irLocalStmt; }

    void setIrLocalStmt(IRLocalStmt *irLocalStmt_) { this->irLocalStmt = irLocalStmt_; }

};

/**
 * 函数类IRGlobalFunc
 */
class IRGlobalFunc {
private:
    std::string funcName;

    std::string retType;

    std::vector<IRLocalBlock *> *baseBlocks;

    std::set<std::string> *fromLineNames;

    unsigned int line;
    unsigned int row;
    unsigned int col;

public:
    /**
     * 函数类IRGlobalFunc构造方法
     * @param funcName 函数名
     * @param retType 返回类型
     * @param baseBlocks 函数内部IR代码块
     * @param fromLineNames 哪些行调用了这个函数
     * @param line 行号所在
     * @param row 源代码位置行号
     * @param col 源代码位置列号
     */
    IRGlobalFunc(const char *funcName, const char *retType, std::vector<IRLocalBlock *> *baseBlocks,
                 std::set<std::string> *fromLineNames, unsigned int line, unsigned int row, unsigned int col) :
            funcName(funcName), retType(retType), baseBlocks(baseBlocks), fromLineNames(fromLineNames),
            line(line), row(row), col(col) {};

    std::string getFuncName() { return funcName; }

    std::string getRetType() { return retType; }

    // label代码块
    [[nodiscard]] std::vector<IRLocalBlock *> *getBaseBlocks() const {
        return baseBlocks;
    }

    void setBaseBlocks(std::vector<IRLocalBlock *> *irLocalBlocks) {
        IRGlobalFunc::baseBlocks = irLocalBlocks;
    }

    void modifyOneLabelByPos(IRLocalBlock *irLocalBlock, unsigned int pos) {
        baseBlocks->erase(baseBlocks->begin() + pos);
        baseBlocks->emplace_back(irLocalBlock);
    }

    // 来源
    std::set<std::string> *getFromLabelNames() { return fromLineNames; }

    void addFromLabelNames(const std::string &oneLabelName) { fromLineNames->insert(oneLabelName); }

    void deleteFromLabelNames(const std::string &oneLabelName) { fromLineNames->erase(oneLabelName); }

    void clearFromLabelNames() { fromLineNames->clear(); }


    // 行列号
    [[nodiscard]] unsigned int getLine() const { return line; }

    void setLine(unsigned int line_) { this->line = line_; }

    [[nodiscard]] unsigned int getRow() const { return row; }

    [[nodiscard]] unsigned int getColumn() const { return col; }
};

/**
 * IRGlobal 单个可供外链的全局变量或函数
 */
class IRGlobal {
private:
    IRGlobalVar *irGlobalVar;
    IRGlobalFunc *irGlobalFunc;

public:
    /**
     * IRGlobal 单个可供外链的全局变量或函数
     * @param irGlobalVar 可为null
     * @param irGlobalFunc 可为null
     */
    IRGlobal(IRGlobalVar *irGlobalVar, IRGlobalFunc *irGlobalFunc) :
            irGlobalVar(irGlobalVar), irGlobalFunc(irGlobalFunc) {};

    [[nodiscard]] IRGlobalVar *getIrGlobalVar() const {
        return irGlobalVar;
    }

    [[nodiscard]] IRGlobalFunc *getIrGlobalFunc() const {
        return irGlobalFunc;
    }
};

/**
 * ir根节点
 */
class IRTree{
private:
    std::vector<IRGlobal *> *irGlobals;

public:
    /**
     * IRTree ir根节点
     * @param irGlobals 全局变量/函数列表
     */
    explicit IRTree(std::vector<IRGlobal *> *irGlobals): irGlobals(irGlobals){};

    [[nodiscard]] std::vector<IRGlobal *> *getIrGlobals() const {
        return irGlobals;
    }
};

#endif //COMPILER_IRGLOBAL_H
