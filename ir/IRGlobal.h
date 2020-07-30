#ifndef COMPILER_IRGLOBAL_H
#define COMPILER_IRGLOBAL_H
/**
 * 详细说明见 SysY2020-WHU_中间码设计.docx
 */
#include <utility>
#include <map>
#include <set>

#include "IRLocalBlock.h"
#include "IRStmt.h"
#include "ArmDag.h"

/**
 * 全局变量类IRGlobalVar
 */
class IRGlobalVar {
private:
    std::vector<IRStmt *> *irStmt;

public:
    /**
     * 全局变量类IRGlobalVar构造方法
     * @param irStmt 变量语句
     */
    explicit IRGlobalVar(std::vector<IRStmt *> *irStmt) : irStmt(irStmt) {};

    std::vector<IRStmt *> *getIrStmt() const {
        return irStmt;
    }

};

// 当前函数的栈状态与变量对应表
class StackStatus {
    // 栈状态
    int currentLoc = 0;
    // 临时存储进入一个block前的栈状态
    //      进入前 lastLoc = currentLoc; 退出后 currentLoc = lastLoc
    int lastLoc = 0;
    // 变量名称与栈地址的映射
    std::map<std::string, int> varMap;
    // 数组变量与维度信息的对应关系
    std::map<std::string, std::vector<int>> arrDimensionMap;
};


/**
 * 函数类IRGlobalFunc
 */
class IRGlobalFunc {
private:
    std::string funcName;

    int retType;

    std::vector<IRLocalBlock *> *baseBlocks;

    std::vector<ArmBlock *> *armBlocks;

    std::multimap<std::string, std::string> *predLocs;

    // 当前函数的栈状态与变量对应表
    StackStatus stackStatus;

public:
    /**
     * 函数类IRGlobalFunc构造方法
     * @param funcName 函数名
     * @param retType 返回类型
     * @param baseBlocks 函数内部IR代码块
     * @param predLocs 调用此函数的代码块位置
     */
    IRGlobalFunc(const char *funcName, int retType, std::vector<IRLocalBlock *> *baseBlocks,
                 std::vector<ArmBlock *> *armBlocks, std::multimap<std::string, std::string> *predLocs) :
            funcName(funcName), retType(retType), baseBlocks(baseBlocks), armBlocks(armBlocks), predLocs(predLocs) {
    };

    std::string getFuncName() { return funcName; }

    [[nodiscard]] int getRetType() const { return retType; }

    [[nodiscard]] std::vector<IRLocalBlock *> *getBaseBlocks() const {
        return baseBlocks;
    }

    void setBaseBlocks(std::vector<IRLocalBlock *> *baseBlocks_) {
        IRGlobalFunc::baseBlocks = baseBlocks_;
    }

    [[nodiscard]] std::multimap<std::string, std::string> *getPredLocs() const {
        return predLocs;
    }

    void addPredLocs(const char *callerFuncName, const char *callerBlockName) {
        IRGlobalFunc::predLocs->insert(std::make_pair(callerFuncName, callerBlockName));
    }

    [[nodiscard]] std::vector<ArmBlock *> *getArmBlocks() const {
        return armBlocks;
    }

    void setArmBlocks(std::vector<ArmBlock *> *armBlocks_) {
        IRGlobalFunc::armBlocks = armBlocks_;
    }

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
class IRTree {
private:
    std::vector<IRGlobal *> *irGlobals;

    DAGRoot *globalDagRoot;

public:
    /**
     * IRTree ir根节点
     * @param irGlobals 全局变量/函数列表
     */
    explicit IRTree(std::vector<IRGlobal *> *irGlobals, DAGRoot *globalDagRoot) :
            irGlobals(irGlobals), globalDagRoot(globalDagRoot) {};

    [[nodiscard]] std::vector<IRGlobal *> *getIrGlobals() const {
        return irGlobals;
    }

    DAGRoot *getGlobalDagRoot() {
        return globalDagRoot;
    }

    void setGlobalDagRoot(DAGRoot *globalDagRoot_) {
        IRTree::globalDagRoot = globalDagRoot_;
    }
};

#endif //COMPILER_IRGLOBAL_H
