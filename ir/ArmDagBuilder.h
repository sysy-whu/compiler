
#ifndef COMPILER_ARMDAGBUILDER_H
#define COMPILER_ARMDAGBUILDER_H

#include "IRLocalDAG.h"
#include "ArmDag.h"
#include "IRStmt.h"

class ArmDAGBuilder {

    explicit ArmDAGBuilder(DAG *dag0){
        dag = dag0;
        ArmDAGRoot *root = new ArmDAGRoot();
        armDag = new ArmDAG(root,dag->getName().data());
    }

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


#endif //COMPILER_ARMDAGBUILDER_H
