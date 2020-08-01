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
    std::string varName;

    int varType;

    std::vector<int> *arraySubs;

    std::vector<int> *globalValue;

    std::vector<std::string> *armList;

public:
    /**
     * 全局变量类IRGlobalVar构造方法
     * @param irStmt 变量语句
     */
    explicit IRGlobalVar(const char *varName, int varType, std::vector<int> *globalVar, std::vector<int> *arraySubs) :
            varName(varName), varType(varType), globalValue(globalVar), arraySubs(arraySubs) {};

    [[nodiscard]] const std::string &getVarName() const {
        return varName;
    }

    [[nodiscard]] int getVarType() const {
        return varType;
    }

    [[nodiscard]] std::vector<int> *getGlobalValue() const {
        return globalValue;
    }

    [[nodiscard]] std::vector<int> *getArraySubs() const {
        return arraySubs;
    }

    void setArmList(std::vector<std::string> *armList) {
        IRGlobalVar::armList = armList;
    }

//    std::vector<IRStmt *> *getIrStmt() const {
//        return irStmt;
//    }
    std::string genString(){
        std::string re = "";
        for(auto it: *armList){
            re += it;
        }
        return re;
    }
};

struct VarInfo {
    /// 变量的栈位置
    int stackLoc;
    /// 变量的level
    int level;
    /// 变量所在的basicBlock名
    std::string block;
};

struct ArrayInfo {
    /// 数组的维度信息
    std::vector<int> dimension;
    ///数组的level
    int level;
    /// 变量所在basicBlock名
    std::string block;
};

// 当前函数的栈状态与变量对应表
class StackStatus {
    // 栈状态
    int currentLoc = 0;
    // 变量名称与栈地址的映射
    std::multimap<std::string, VarInfo> *varMap;
    // 数组变量与维度信息的对应关系
    std::multimap<std::string, ArrayInfo> *arrDimensionMap;
public:
    StackStatus() {
        varMap = new std::multimap<std::string, VarInfo>();
        arrDimensionMap = new std::multimap<std::string, ArrayInfo>();
    };

    int getCurrentLoc() const {
        return currentLoc;
    }

    std::multimap<std::string, VarInfo> *getVarMap() const {
        return varMap;
    }

    std::multimap<std::string, ArrayInfo> *getArrDimensionMap() const {
        return arrDimensionMap;
    }

    void setCurrentLoc(int currentLoc) {
        StackStatus::currentLoc = currentLoc;
    }

    void setVarMap(std::multimap<std::string, VarInfo> *varMap) {
        StackStatus::varMap = varMap;
    }

    void setArrDimensionMap(std::multimap<std::string, ArrayInfo> *arrDimensionMap) {
        StackStatus::arrDimensionMap = arrDimensionMap;
    }
};

class IRGlobalFuncParam {
private:
    std::string paramName;

    int paramType;

//    // void arrayFunc(int a[][3]);
//    std::vector<int> *arrayParam;

public:
    IRGlobalFuncParam(const char *paramName, int paramType/*, std::vector<int> *arrayParam*/) :
            paramName(paramName), paramType(paramType)/*, arrayParam(arrayParam)*/ {};

    [[nodiscard]] const std::string &getParamName() const {
        return paramName;
    }

    [[nodiscard]] int getParamType() const {
        return paramType;
    }

//    [[nodiscard]] std::vector<int> *getArrayParam() const {
//        return arrayParam;
//    }
};

/**
 * 函数类IRGlobalFunc
 */
class IRGlobalFunc {
private:
    std::string funcName;

    int retType;

    std::vector<IRGlobalFuncParam *> *irGlobalFuncParams;

    std::vector<IRLocalBlock *> *baseBlocks;

    std::vector<ArmBlock *> *armBlocks;

    std::multimap<std::string, std::string> *predLocs;

    // 当前函数的栈状态与变量对应表
    StackStatus *stackStatus;

public:
    /**
     * 函数类IRGlobalFunc构造方法
     * @param funcName 函数名
     * @param retType 返回类型
     * @param baseBlocks 函数内部IR代码块
     * @param predLocs 调用此函数的代码块位置
     */
    IRGlobalFunc(const char *funcName, int retType, std::vector<IRLocalBlock *> *baseBlocks,
                 std::vector<ArmBlock *> *armBlocks, std::multimap<std::string, std::string> *predLocs,
                 std::vector<IRGlobalFuncParam *> *irGlobalFuncParams) :
            funcName(funcName), retType(retType), baseBlocks(baseBlocks), armBlocks(armBlocks), predLocs(predLocs),
            irGlobalFuncParams(irGlobalFuncParams), stackStatus(new StackStatus()) {
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

    StackStatus *getStackStatus() const {
        return stackStatus;
    }

    std::vector<IRGlobalFuncParam *> *getIrGlobalFuncParams() const {
        return irGlobalFuncParams;
    }

    std::string genString() {
        std::string re = funcName + ":\n";
        for (auto armBlock:*armBlocks) {
            re += armBlock->genString();
        }
        return re;
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

    std::string genString(){
        if (irGlobalFunc != nullptr){

            return "";
        }
        if (irGlobalFunc != nullptr){
            return irGlobalFunc->genString();
        }
        return "";
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

    std::string genString(){
        std::string re = "";
        for(auto irGlobal: *irGlobals){
            re += irGlobal->genString();
        }
        return re;
    }
};

#endif //COMPILER_IRGLOBAL_H
