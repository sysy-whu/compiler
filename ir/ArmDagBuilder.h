
#ifndef COMPILER_ARMDAGBUILDER_H
#define COMPILER_ARMDAGBUILDER_H

#include "IRLocalDAG.h"
#include "ArmDag.h"
#include "IRStmt.h"
#include "IRGlobal.h"

struct ArmNodes{
    ArmDAGNode* head;
    ArmDAGNode* tail;
};

class ArmDAGBuilder {
public:

    explicit ArmDAGBuilder(DAGRoot *root, StackStatus *stackStatus) {

        dagRoot = root;
        this->status = stackStatus;
        armRoot = new ArmDAGRoot();

    }

    DAGRoot *dagRoot;

    ArmDAGRoot *armRoot;

    StackStatus *status;

    int count = 0;

    /// 全局变量处理

    static std::vector<std::string> * genGlobal(IRGlobalVar *var);

    static std::vector<std::string> * genConGlobal(IRGlobalVar *var);

    static std::vector<std::string> * genGlobalArray(IRGlobalVar *var);

    static std::vector<std::string> * genConGlobalArray(IRGlobalVar *var);

    /// 生成ArmDAG主方法
    void generateArmDag();

    /// 声明语句
    ArmNodes genAlloca(DAGNode *nd);

    ArmNodes genConAlloca(DAGNode *nd);

    ArmNodes genAllocaArray(DAGNode *nd);

    ArmNodes genConAllocaArray(DAGNode *nd);

    /// 运算
    ArmNodes genAddNode(DAGNode *nd);

    ArmNodes genSubNode(DAGNode *nd);

    ArmNodes genMulNode(DAGNode *nd);

    ArmNodes genDivNode(DAGNode *nd);

    ArmNodes genModNode(DAGNode *nd);

    ArmNodes genAndNode(DAGNode *nd);

    ArmNodes genOrNode(DAGNode *nd);

    ArmNodes genEQNode(DAGNode *nd);

    ArmNodes genGTNode(DAGNode *nd);

    ArmNodes genLTNode(DAGNode *nd);

    ArmNodes genNEQNode(DAGNode *nd);

    ArmNodes genLTENode(DAGNode *nd);

    ArmNodes genGTENode(DAGNode *nd);



    /// 控制语句
    ArmNodes genBRNode(DAGNode *nd);

    ArmNodes genBRCondNode(DAGNode *nd);

    ArmNodes genRetNode(DAGNode *nd);

    /// 函数调用
    ArmNodes genCallNode(DAGNode *nd);

    ///存取语句
    ArmNodes genStoreNode(DAGNode *nd);

    ArmNodes genLoadNode(DAGNode *nd);

    ArmNodes genGetPtrNode(DAGNode *nd);

};

class ArmDAGGen {
private:
    IRTree *irTree;
    StackStatus *globalStatus;
private:
    /// 处理全局变量
    void IRGlobalValGen(IRGlobalVar *irGlobalVar) {

        if(irGlobalVar->getVarType() == TYPE_INT){ // int

            //记录变量位置,写入维度信息
            std::multimap<std::string,VarInfo> varMap =  * (globalStatus->getVarMap());

            varMap.emplace(irGlobalVar->getVarName(),VarInfo{globalStatus->getCurrentLoc(),0});

            globalStatus->setCurrentLoc(-1);
            globalStatus->setVarMap(&varMap);
        }
        else if(irGlobalVar->getVarType() == TYPE_INT_STAR){ //int*

            //记录变量位置,写入维度信息
            std::multimap<std::string,VarInfo> varMap =  * (globalStatus->getVarMap());
            std::multimap<std::string,ArrayInfo> dimMap = *(globalStatus->getArrDimensionMap());

            //TODO: [维度信息保存到dims]
            std::vector<int> dims;

            varMap.emplace(irGlobalVar->getVarName(),VarInfo{globalStatus->getCurrentLoc(),0});
            dimMap.emplace(irGlobalVar->getVarName(),ArrayInfo{dims,0});

            globalStatus->setCurrentLoc( -1);
            globalStatus->setArrDimensionMap(&dimMap);
            globalStatus->setVarMap(&varMap);

        }
        else{
            perror("Error in ArmDAGGen.IRGloabalVarGen()!\n"
                   "unexpected vartype. \n");
        }
    }

    /// 处理函数
    void IRGlobalFuncGen(IRGlobalFunc *irGlobalFunc) {
        // todo 初始化stackStatus,全局变量的信息放到每个函数的stackStatus
        StackStatus *stackStatus = irGlobalFunc->getStackStatus();
        //  深度遍历globalStatus的varMap
        auto *varMap = new std::multimap<std::string, VarInfo>();
        *varMap = *globalStatus->getVarMap();
        stackStatus->setVarMap(varMap);
        //  深度遍历globalStatus的varMap
        auto *arrDimensionMap = new std::multimap<std::string, ArrayInfo>();
        *arrDimensionMap = *globalStatus->getArrDimensionMap();
        stackStatus->setArrDimensionMap(arrDimensionMap);

        // 开始遍历BaseBlocks并处理
        for (auto &it : *irGlobalFunc->getBaseBlocks()) {
            // 开始处理DagRoot
            DagRootGen(it->getDagRoot(), it->getBlockName().c_str(), irGlobalFunc->getArmBlocks(),
                       irGlobalFunc->getStackStatus());
        }
    }

    /**
     * 处理DagRoot
     * @param dagRoot：待解析的dagRoot(解析dagRoot生成armDagRoot)
     * @param armBlocks：将生成的dagRoot放到这里
     * @param stackStatus：该dagRoot所在函数的栈状态信息
     */
    void
    DagRootGen(DAGRoot *dagRoot, const char *blockName, std::vector<ArmBlock *> *armBlocks, StackStatus *stackStatus) {
        auto armDagBuilder = new ArmDAGBuilder(dagRoot, stackStatus);
        // ==== todo 处理  ========

        armDagBuilder->generateArmDag();


        // ===========
        ArmBlock *armBlock = new ArmBlock(blockName, armDagBuilder->armRoot);
        armBlocks->emplace_back(armBlock);

        delete armDagBuilder;
    }

public:
    explicit ArmDAGGen(IRTree *irTree) {
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


#endif //COMPILER_ARMDAGBUILDER_H
