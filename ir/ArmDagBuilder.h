
#ifndef COMPILER_ARMDAGBUILDER_H
#define COMPILER_ARMDAGBUILDER_H

#include "IRLocalDAG.h"
#include "ArmDag.h"
#include "IRStmt.h"
#include "IRGlobal.h"

class ArmDAGBuilder {
public:
    explicit ArmDAGBuilder(DAG *dag0) {
        dag = dag0;
        ArmDAGRoot *root = new ArmDAGRoot();
        armDag = new ArmDAG(root, dag->getName().data());
    }

    ArmDAG *getArmDag() const;

private:
    DAG *dag;
    ArmDAG *armDag;

    int count = 0;

    /// 生成ArmDAG主方法
    void generateArmDag();

    /// 声明语句
    ArmDAGNode *genAlloca(DAGNode *nd);

    ArmDAGNode *genConAlloca(DAGNode *nd);

    ArmDAGNode *genAllocaArray(DAGNode *nd);

    ArmDAGNode *genConAllocaArray(DAGNode *nd);

    ArmDAGNode *genGlobal(DAGNode *nd);

    ArmDAGNode *genConGlobalArray(DAGNode *nd);

    /// 运算
    ArmDAGNode *genAddNode(DAGNode *nd);

    ArmDAGNode *genSubNode(DAGNode *nd);

    ArmDAGNode *genMulNode(DAGNode *nd);

    ArmDAGNode *genDivNode(DAGNode *nd);

    ArmDAGNode *genModNode(DAGNode *nd);


    /// 控制语句
    ArmDAGNode *genBRNode(DAGNode *nd);

    ArmDAGNode *genBRCondNode(DAGNode *nd);

    ArmDAGNode *genRetNode(DAGNode *nd);

    /// 函数调用
    ArmDAGNode *genCallNode(DAGNode *nd);

    ///存取语句
    ArmDAGNode *genStoreNode(DAGNode *nd);

    ArmDAGNode *genLoadNode(DAGNode *nd);

    ArmDAGNode *genGetPtrNode(DAGNode *nd);

};

class ArmDAGGen {
private:
    IRTree *irTree;

private:
    /// 处理全局变量
    void IRGlobalValGen(IRGlobalVar *irGlobalVar) {

    }

    /// 处理函数
    void IRGlobalFuncGen(IRGlobalFunc *irGlobalFunc) {
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
        auto armDagBuilder = new ArmDAGBuilder(dagRoot->getDag());
        // ==== todo 处理  ========


        // ===========
        ArmBlock *armBlock = new ArmBlock(blockName, armDagBuilder->getArmDag()->getRoot());
        armBlocks->emplace_back(armBlock);
    }

public:
    explicit ArmDAGGen(IRTree *irTree) {
        this->irTree = irTree;
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
