#include "ArmDagBuilder.h"
#include "ArmDag.h"

//主方法  遍历DAG生成ARMDAG
void ArmDAGBuilder::generateArmDag() {
    //TODO
    return;
}

/*
 * 运算语句
 */

ArmDAGNode *ArmDAGBuilder::genAddNode(DAGNode *nd){
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), ADD, opd1,opd2,opd3);
    return node;
}

ArmDAGNode *ArmDAGBuilder::genSubNode(DAGNode *nd){
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), SUB, opd1,opd2,opd3);
    return node;
}

ArmDAGNode *ArmDAGBuilder::genMulNode(DAGNode *nd){
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), MUL, opd1,opd2,opd3);
    return node;
}


ArmDAGNode *ArmDAGBuilder::genModNode(DAGNode *nd){
    //TODO: 在.s文件开头添加    .global __aeabi_imod

    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), MOV, (std::string &) "r0",opd2);
    ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(),MOV,(std::string &) "r1",opd3);
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),BL,opd1,(std::string &) "__aeabi_imod");
    ArmDAGNode *node3 = new ArmDAGNode(count++, nd->getID(),MOV,opd1,(std::string &) "r0");

    node1->addDependUse(node);
    node2->addDependUse(node1);
    node3->addDependUse(node2);

    return node;

}


ArmDAGNode *ArmDAGBuilder::genDivNode(DAGNode *nd){
    //TODO: 在.s文件开头添加    .global __aeabi_idiv

    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), MOV, (std::string &) "r0",opd2);
    ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(),MOV,(std::string &) "r1",opd3);
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),BL,opd1,(std::string &) "__aeabi_idiv");
    ArmDAGNode *node3 = new ArmDAGNode(count++, nd->getID(),MOV,opd1,(std::string &) "r0");

    node1->addDependUse(node);
    node2->addDependUse(node1);
    node3->addDependUse(node2);

    return node;
}




ArmDAGNode *ArmDAGBuilder::genEQNode(DAGNode *nd){

    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), CMP, opd2,opd3);
    ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(),MOVEQ,opd1,(std::string &) "#1");
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),MOVNE,opd1,(std::string &) "#0");

    node1->addDependUse(node);
    node2->addDependUse(node1);

    return node;

}
ArmDAGNode *ArmDAGBuilder::genNEQNode(DAGNode *nd){

    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), CMP, opd2,opd3);
    ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(),MOVNE,opd1,(std::string &) "#1");
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),MOVEQ,opd1,(std::string &) "#0");

    node1->addDependUse(node);
    node2->addDependUse(node1);

    return node;
}

ArmDAGNode *ArmDAGBuilder::genGTNode(DAGNode *nd){
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), CMP, opd2,opd3);
    ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(),MOVGT,opd1,(std::string &) "#1");
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),MOVLE,opd1,(std::string &) "#0");

    node1->addDependUse(node);
    node2->addDependUse(node1);

    return node;
}

ArmDAGNode *ArmDAGBuilder::genLTNode(DAGNode *nd){
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), CMP, opd2,opd3);
    ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(),MOVLT,opd1,(std::string &) "#1");
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),MOVGE,opd1,(std::string &) "#0");

    node1->addDependUse(node);
    node2->addDependUse(node1);

    return node;
}

ArmDAGNode *ArmDAGBuilder::genGTENode(DAGNode *nd){
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), CMP, opd2,opd3);
    ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(),MOVGE,opd1,(std::string &) "#1");
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),MOVLT,opd1,(std::string &) "#0");

    node1->addDependUse(node);
    node2->addDependUse(node1);

    return node;
}

ArmDAGNode *ArmDAGBuilder::genLTENode(DAGNode *nd){
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), CMP, opd2,opd3);
    ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(),MOVLE,opd1,(std::string &) "#1");
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),MOVGT,opd1,(std::string &) "#0");

    node1->addDependUse(node);
    node2->addDependUse(node1);

    return node;
}

ArmDAGNode *ArmDAGBuilder::genAndNode(DAGNode *nd){
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++, nd->getID(),ADD,opd1,opd2,opd3);
    ArmDAGNode *node1 = new ArmDAGNode(count++,  nd->getID(), CMP, opd1,(std::string&)"#1");
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),MOVEQ,opd1,(std::string &) "#1");
    ArmDAGNode *node3 = new ArmDAGNode(count++, nd->getID(),MOVNE,opd1,(std::string &) "#0");


    node1->addDependUse(node);
    node2->addDependUse(node1);
    node3->addDependUse(node2);

    return node;
}

ArmDAGNode *ArmDAGBuilder::genOrNode(DAGNode *nd){

    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++, nd->getID(),ADD,opd1,opd2,opd3);
    ArmDAGNode *node1 = new ArmDAGNode(count++,  nd->getID(), CMP, opd1,(std::string&)"#1");
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),MOVGE,opd1,(std::string &) "#1");
    ArmDAGNode *node3 = new ArmDAGNode(count++, nd->getID(),MOVLT,opd1,(std::string &) "#0");


    node1->addDependUse(node);
    node2->addDependUse(node1);
    node3->addDependUse(node2);

    return node;
}


/*
 * 声明语句
 */

ArmDAGNode *ArmDAGBuilder::genAlloca(DAGNode *nd){
    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), SUB, (std::string &) "SP", (std::string &)"SP", (std::string &)"4");
    //TODO: StackStatus记录所在栈中位置
    return node;
}

ArmDAGNode *ArmDAGBuilder::genConAlloca(DAGNode *nd){
    //TODO: 记录对应值（ 在stackstatus中？）
}

ArmDAGNode *ArmDAGBuilder::genAllocaArray(DAGNode *nd){

    //TODO: 获取维度之积
    int space = 0;

    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), SUB, (std::string&)"SP",(std::string&)"SP",(std::string&)space);

    //TODO: StackStatus 记录变量位置
    return node;
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
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), CMP, opd1,(std::string&)"#1");
    ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(),BEQ,opd2);
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),B,opd3);

    node1->addDependUse(node);
    node2->addDependUse(node1);

    return node;
}

ArmDAG *ArmDAGBuilder::getArmDag() const {
    return armDag;
}


