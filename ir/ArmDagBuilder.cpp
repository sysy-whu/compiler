#include "ArmDagBuilder.h"
#include "ArmDag.h"

//主方法
void ArmDAGBuilder::generateArmDag() {
    //TODO
    return;
}

/*
 * 运算语句
 */

ArmDAGNode *ArmDAGBuilder::genAddNode(DAGNode *nd){
    std::vector<DAGUse*> opList = nd->getOperandList();
    count++;

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), ADD, opd1,opd2,opd3);
    return node;
}

ArmDAGNode *ArmDAGBuilder::genSubNode(DAGNode *nd){
    std::vector<DAGUse*> opList = nd->getOperandList();
    count++;

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), SUB, opd1,opd2,opd3);
    return node;
}

ArmDAGNode *ArmDAGBuilder::genMulNode(DAGNode *nd){
    std::vector<DAGUse*> opList = nd->getOperandList();
    count++;

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), MUL, opd1,opd2,opd3);
    return node;
}


ArmDAGNode *ArmDAGBuilder::genModNode(DAGNode *nd){
    //TODO
}


ArmDAGNode *ArmDAGBuilder::genDivNode(DAGNode *nd){
    //TODO
}


/*
 * 声明语句
 */

ArmDAGNode *ArmDAGBuilder::genAlloca(DAGNode *nd){
    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), SUB, (std::string &) "SP", (std::string &)"SP", (std::string &)"4");
    //TODO: 记录所在栈中位置
    return node;
}

ArmDAGNode *ArmDAGBuilder::genConAlloca(DAGNode *nd){
    //TODO
}

ArmDAGNode *ArmDAGBuilder::genAllocaArray(DAGNode *nd){
    //TODO
}

ArmDAGNode *ArmDAGBuilder::genConAllocaArray(DAGNode *nd){
    //TODO
}

ArmDAGNode *ArmDAGBuilder::genGlobal(DAGNode *nd){
    //TODO
}

ArmDAGNode *ArmDAGBuilder::genConGlobalArray(DAGNode *nd){
    //TODO
}


/*
 * 存取语句
 */

ArmDAGNode *ArmDAGBuilder::genLoadNode(DAGNode *nd){
    std::string opd1 = nd->getOperandList()[0]->getNode()->getRetName();
    //TODO: 此处需要查表获取记录的变量地址并保存到addr中
    std::string addr = nd->getOperandList()[1]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), LDR, opd1, addr);
    return node;
}

ArmDAGNode *ArmDAGBuilder::genStoreNode(DAGNode *nd){
    std::string opd1 = nd->getOperandList()[0]->getNode()->getRetName();
    std::string opd2 = nd->getOperandList()[0]->getNode()->getRetName();

    //TODO: 此处需要查表获取记录的变量地址并保存到addr中
    std::string addr = "addr";

    ArmDAGNode *node = new ArmDAGNode(count++, nd->getID(), MOV, opd1, opd2);
    ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(), STR, opd1,addr);
    node1->addDependUse(node);
    return node;

}

ArmDAGNode *ArmDAGBuilder::genGetPtrNode(DAGNode *nd){
    //TODO
}

/*
 * 函数调用
 */

ArmDAGNode *ArmDAGBuilder::genCallNode(DAGNode *nd){

    //TODO: 此处应有if-else，问题：无return值时，oplist[0]是？

    std::string opd2 = nd->getOperandList()[1]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++, nd->getID(),BL, opd2);
    return node;
}


/*
 * 控制语句
 */

ArmDAGNode *ArmDAGBuilder::genBRNode(DAGNode *nd){
    std::string opd2 = nd->getOperandList()[1]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++, nd->getID(),B, opd2);
    return node;
}

ArmDAGNode *ArmDAGBuilder::genRetNode(DAGNode *nd){
    if(nd->getOperandList().size()!=0){
        std::string opd1 = nd->getOperandList()[0]->getNode()->getRetName();
        ArmDAGNode *node = new ArmDAGNode(count++, nd->getID(), MOV, (std::string &)"R0", opd1);
        ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(), BX, (std::string &)"LR");
        node1->addDependUse(node);
        return node;
    }
    else{
        ArmDAGNode *node = new ArmDAGNode(count++, nd->getID(), BX, (std::string &)"LR");
        return node;
    }
}


ArmDAGNode *ArmDAGBuilder::genBRCondNode(DAGNode *nd){
    //TODO
}

ArmDAG *ArmDAGBuilder::getArmDag() const {
    return armDag;
}


