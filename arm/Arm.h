//
// Created by 77350 on 2020/8/1.
//

#ifndef COMPILER_ARM_H
#define COMPILER_ARM_H


#include "../ir/IRGlobal.h"

class ArmBuilder {
    // ir的stmts
    std::vector<IRStmt *> *stmts;
    // 存储解析后的armStmts
    std::vector<ArmStmt *> *armStmts;

    StackStatus *status;

    int count = 0;
public:
    std::vector<ArmStmt *> *getArmStmts() const {
        return armStmts;
    }

public:

    explicit ArmBuilder(std::vector<IRStmt *> *stmts, StackStatus *stackStatus) {
        this->stmts = stmts;
        this->status = stackStatus;
        armStmts = new std::vector<ArmStmt *>();
    }

    void generateArmStmts(const char *string, std::vector<IRGlobalFuncParam *> *funcParms) {

    }

};


class ArmGen {
private:
    IRTree *irTree;
    StackStatus *globalStatus;
private:
    /// 处理全局变量
    void IRGlobalValGen(IRGlobalVar *irGlobalVar) {

        if (irGlobalVar->getVarType() == TYPE_INT) { // int

            //记录变量位置,写入维度信息
            std::multimap<std::string, VarInfo> varMap = *(globalStatus->getVarMap());

            varMap.emplace(irGlobalVar->getVarName(), VarInfo{globalStatus->getCurrentLoc(), 0});

            globalStatus->setCurrentLoc(-1);
            globalStatus->setVarMap(&varMap);
        } else if (irGlobalVar->getVarType() == TYPE_INT_STAR) { //int*

            //记录变量位置,写入维度信息
            std::multimap<std::string, VarInfo> varMap = *(globalStatus->getVarMap());
            std::multimap<std::string, ArrayInfo> dimMap = *(globalStatus->getArrDimensionMap());

            //维度信息写入到dims]
            std::vector<int> dims = *(irGlobalVar->getArraySubs());

            varMap.emplace(irGlobalVar->getVarName(), VarInfo{globalStatus->getCurrentLoc(), 0});
            dimMap.emplace(irGlobalVar->getVarName(), ArrayInfo{dims, 0});

            globalStatus->setCurrentLoc(-1);
            globalStatus->setArrDimensionMap(&dimMap);
            globalStatus->setVarMap(&varMap);

        } else {
            perror("Error in ArmDAGGen.IRGloabalVarGen()!\n"
                   "unexpected vartype. \n");
        }
    }

    /// 处理函数
    void IRGlobalFuncGen(IRGlobalFunc *irGlobalFunc) {
        StackStatus *stackStatus = irGlobalFunc->getStackStatus();

        //  深度拷贝globalStatus的varMap
        auto *varMap = new std::multimap<std::string, VarInfo>();
        *varMap = *globalStatus->getVarMap();
        stackStatus->setVarMap(varMap);
        //  深度拷贝globalStatus的varMap
        auto *arrDimensionMap = new std::multimap<std::string, ArrayInfo>();
        *arrDimensionMap = *globalStatus->getArrDimensionMap();
        stackStatus->setArrDimensionMap(arrDimensionMap);

        // 开始遍历BaseBlocks并处理
        for (auto &it : *irGlobalFunc->getBaseBlocks()) {
            // 开始处理IrStmts
            IrStmtsGen(it->getStmts(), it->getBlockName().c_str(), irGlobalFunc->getArmBlocks(),
                       irGlobalFunc->getStackStatus(), irGlobalFunc->getIrGlobalFuncParams());
        }
    }

    /**
     * 处理DagRoot
     * @param dagRoot：待解析的dagRoot(解析dagRoot生成armDagRoot)
     * @param armBlocks：将生成的dagRoot放到这里
     * @param stackStatus：该dagRoot所在函数的栈状态信息
     */
    void IrStmtsGen(std::vector<IRStmt *> *stmts, const char *blockName, std::vector<ArmBlock *> *armBlocks,
                    StackStatus *stackStatus,
                    std::vector<IRGlobalFuncParam *> *parmsInfo) {
        auto armBuilder = new ArmBuilder(stmts, stackStatus);
        // ==== todo 处理  ========

        armBuilder->generateArmStmts(blockName, parmsInfo);


        // ===========
        ArmBlock *armBlock = new ArmBlock(blockName, armBuilder->getArmStmts());
        armBlocks->emplace_back(armBlock);

        delete armBuilder;
    }

public:
    explicit ArmGen(IRTree *irTree) {
        this->irTree = irTree;
        globalStatus = new StackStatus();
    };

    /// 启动函数
    void startGen() {
        auto iRGlobals = this->irTree->getIrGlobals();
        // 开始遍历iRGlobals
        for (auto &irGlobal : *iRGlobals) {
            if (irGlobal->getIrGlobalVar() != nullptr) {
                this->IRGlobalValGen(irGlobal->getIrGlobalVar());
            }
            if (irGlobal->getIrGlobalFunc() != nullptr) {
                this->IRGlobalFuncGen(irGlobal->getIrGlobalFunc());
            }
        }
    };
};


#endif //COMPILER_ARM_H
