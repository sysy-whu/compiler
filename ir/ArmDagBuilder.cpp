#include "ArmDagBuilder.h"
#include "ArmDag.h"
#include "IRGlobal.h"

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

    std::string opd1 = nd->getOperandList()[0]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), SUB, (std::string &) "$SP", (std::string &)"$SP", (std::string &)"4");

    //记录变量位置
    std::multimap<std::string,VarInfo> varMap =  * (status->getVarMap());
    varMap.emplace(opd1,VarInfo{status->getCurrentLoc(),nd->getLevel()});
    int loc = status->getCurrentLoc() + 4;
    status->setCurrentLoc( loc );
    status->setVarMap(&varMap);

    return node;
}

ArmDAGNode *ArmDAGBuilder::genConAlloca(DAGNode *nd){
    std::string opd1 = nd->getOperandList()[0]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), SUB, (std::string &) "$SP", (std::string &)"$SP", (std::string &)"4");

    //记录变量位置
    std::multimap<std::string,VarInfo> varMap =  * (status->getVarMap());
    varMap.emplace(opd1,VarInfo{status->getCurrentLoc(),nd->getLevel()});
    int loc = status->getCurrentLoc() + 4;
    status->setCurrentLoc( loc );
    status->setVarMap(&varMap);
    return node;
}

ArmDAGNode *ArmDAGBuilder::genAllocaArray(DAGNode *nd){

    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();

    //保存维度信息，并获取维度之积
    int dim = opList.size() - 2;
    std::vector<int> dims;
    int space = 4;

    for(int i = 0; i<dim; i++){
        int tmp = std::stoi( opList[i+2]->getNode()->getRetName());
        dims.emplace_back( tmp);
        space*=tmp;
    }

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), SUB, (std::string&)"$SP",(std::string&)"$SP",(std::string&)space);

    //记录变量位置,写入维度信息
    std::multimap<std::string,VarInfo> varMap =  * (status->getVarMap());
    std::multimap<std::string,ArrayInfo> dimMap = *(status->getArrDimensionMap());

    varMap.emplace(opd1,VarInfo{status->getCurrentLoc(),nd->getLevel()});
    dimMap.emplace(opd1,ArrayInfo{dims,nd->getLevel()});

    int loc = status->getCurrentLoc() + space;
    status->setCurrentLoc( loc );
    status->setArrDimensionMap(&dimMap);
    status->setVarMap(&varMap);

    return node;
}

ArmDAGNode *ArmDAGBuilder::genConAllocaArray(DAGNode *nd){

    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();

    //保存维度信息，并获取维度之积
    int dim = opList.size() - 2;
    std::vector<int> dims;
    int space = 4;

    for(int i = 0; i<dim; i++){
        int tmp = std::stoi( opList[i+2]->getNode()->getRetName());
        dims.emplace_back( tmp);
        space*=tmp;
    }

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), SUB, (std::string&)"$SP",(std::string&)"$SP",(std::string&)space);

    //记录变量位置,写入维度信息
    std::multimap<std::string,VarInfo> varMap =  * (status->getVarMap());
    std::multimap<std::string,ArrayInfo> dimMap = *(status->getArrDimensionMap());

    varMap.emplace(opd1,VarInfo{status->getCurrentLoc(),nd->getLevel()});
    dimMap.emplace(opd1,ArrayInfo{dims,nd->getLevel()});

    int loc = status->getCurrentLoc() + space;
    status->setCurrentLoc( loc );
    status->setArrDimensionMap(&dimMap);
    status->setVarMap(&varMap);

    return node;
}

/*
 * 存取语句
 */

ArmDAGNode *ArmDAGBuilder::genLoadNode(DAGNode *nd){
    std::string opd1 = nd->getOperandList()[0]->getNode()->getRetName();
    std::string opd2 = nd->getOperandList()[1]->getNode()->getRetName();
    std::string addr;

    std::multimap<std::string ,VarInfo> *varMap = status->getVarMap();

    int loc; int currLevel = nd->getLevel(); int level=-1;
    int n = varMap->count(opd2);
    if(n == 1){
        VarInfo info = varMap->find(opd2)->second;
        loc = info.stackLoc;
        level = info.level;
    }
    else if ( n > 1 ){
        auto iter = varMap->find(opd2);
        VarInfo result = iter->second;
        for(int i = 0; i < n; i++){
            VarInfo info = iter->second;
            if(currLevel >= info.level || result.level < info.level){
                result = info;
            }
            iter++;
        }
        loc = result.stackLoc;
    }
    else{ // not found
        perror("Compile Error: genLoadNode - variable not found!\n");
    }

    if(level == 0){ // 为全局变量 采用特殊取址方式
        addr = "["+ opd1 +"]";
        std::string lower =  "#:lower16:"+ opd2;    std::string upper =  "#:upper16:"+ opd2;
        ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), MOVW, opd1, lower);
        ArmDAGNode *node1 = new ArmDAGNode(count++,  nd->getID(), MOVT,opd1, upper);
        ArmDAGNode *node2 = new ArmDAGNode(count++,  nd->getID(), LDR, opd1, addr);

        node1->addDependUse(node);
        node2->addDependUse(node1);

        return node;
    }
    else{
        addr = "[fp, #-"+(std::string&)loc+"]";

        ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), LDR, opd1, addr);
        return node;
    }

}

ArmDAGNode *ArmDAGBuilder::genStoreNode(DAGNode *nd){
    std::string opd1 = nd->getOperandList()[0]->getNode()->getRetName();
    std::string opd2 = nd->getOperandList()[1]->getNode()->getRetName();
    std::string addr;

    std::multimap<std::string ,VarInfo> *varMap = status->getVarMap();

    int loc; int currLevel = nd->getLevel(); int level = -1;
    int n = varMap->count(opd1);
    if(n == 1){
        VarInfo info = varMap->find(opd1)->second;
        loc = info.stackLoc;
        level = info.level;
    }
    else if ( n > 1 ){
        auto iter = varMap->find(opd1);
        VarInfo result = iter->second;
        for(int i = 0; i < n; i++){
            VarInfo info = iter->second;
            if(currLevel >= info.level || result.level < info.level){
                result = info;
            }
            iter++;
        }
        loc = result.stackLoc;
    }
    else{ // not found
        perror("Compile Error: genLoadNode - variable not found!\n");
    }


    if(level == 0){ // 为全局变量 采用特殊取址方式
        addr = "[R2]";  //FIXME: 这里用了R2寄存器来保存地址  !!!
        std::string lower =  "#:lower16:"+ opd1;    std::string upper =  "#:upper16:"+ opd1;

        ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), MOVW, (std::string&)"R2", lower);
        ArmDAGNode *node1 = new ArmDAGNode(count++,  nd->getID(), MOVT,(std::string&)"R2", upper);
        ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(), MOV, opd1, opd2);
        ArmDAGNode *node3 = new ArmDAGNode(count++,  nd->getID(), LDR, opd1, addr);

        node1->addDependUse(node);
        node2->addDependUse(node1);
        node3->addDependUse(node2);

        return node;
    }
    else{
        addr = "[fp, #-"+(std::string&)loc+"]";


        ArmDAGNode *node = new ArmDAGNode(count++, nd->getID(), MOV, opd1, opd2);
        ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(), STR, opd1,addr);
        node1->addDependUse(node);
        return node;
    }

}

ArmDAGNode *ArmDAGBuilder::genGetPtrNode(DAGNode *nd){

    /*
     * mov %3, %array
       mul %r, %2, #2
       add %r, %r, #2
       sub %3, %3, %r
     */

    // 加载各个参数 opd3为保存各维数字的vector
    std::string opd1 = nd->getOperandList()[0]->getNode()->getRetName();
    std::string opd2 = nd->getOperandList()[1]->getNode()->getRetName();

    int length = nd->getOperandList().size()-2;
    std::vector<std::string> opd3;
    for(int i = 0; i < length; i++){
        std::string dim = nd->getOperandList()[i+2]->getNode()->getRetName();
        opd3.emplace_back(dim);
    }

    std::multimap<std::string,ArrayInfo> *dimMap = status->getArrDimensionMap();
    std::multimap<std::string ,VarInfo> *varMap = status->getVarMap();

    std::string addr;

    int loc; int currLevel = nd->getLevel(); int level = -1;
    int n = varMap->count(opd1);
    if(n == 1){
        VarInfo info = varMap->find(opd1)->second;
        loc = info.stackLoc;
        level = info.level;
    }
    else if ( n > 1 ){
        auto iter = varMap->find(opd1);
        VarInfo result = iter->second;
        for(int i = 0; i < n; i++){
            VarInfo info = iter->second;
            if(currLevel >= info.level || result.level < info.level){
                result = info;
            }
            iter++;
        }
        loc = result.stackLoc;
    }
    else{ // not found
        perror("Compile Error: compile error: genGetPtrNode - array not found!\"\n");
    }



    //TODO: 计算偏移量

    return nullptr;
}

/*
 * 函数调用
 */

ArmDAGNode *ArmDAGBuilder::genCallNode(DAGNode *nd){
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
        ArmDAGNode *node = new ArmDAGNode(count++, nd->getID(), MOV, (std::string &)"$R0", opd1);
        ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(), BX, (std::string &)"$LR");
        node1->addDependUse(node);
        return node;
    }
    else{
        ArmDAGNode *node = new ArmDAGNode(count++, nd->getID(), BX, (std::string &)"$LR");
        return node;
    }
}


ArmDAGNode *ArmDAGBuilder::genBRCondNode(DAGNode *nd){
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), CMP, opd1,(std::string&)"1");
    ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(),BEQ,opd2);
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),B,opd3);

    node1->addDependUse(node);
    node2->addDependUse(node1);

    return node;
}

std::vector<std::string> *ArmDAGBuilder::genGlobal(IRGlobalVar *var){
    auto *armStmt= new std::vector<std::string>();
    if(var->getGlobalValue()->size() == 0){ //未初始化的全局变量
        armStmt->push_back("    .comm "+var->getVarName()+", 4\n");
    }
    else{ //初始化的全局变量
        std::string name = var->getVarName();
        armStmt->emplace_back("    .global "+name+"\n");
        armStmt->emplace_back("    .data\n");
        armStmt->emplace_back("    .align 2\n");
        armStmt->emplace_back("    .type "+name+", %object\n");
        armStmt->emplace_back("    .size "+name+","+"4\n");
        armStmt->emplace_back(var->getVarName()+":\n");
        armStmt->emplace_back("    .word " + (std::string&)var->getGlobalValue()[0]+"\n");
    }

    return armStmt;

}

std::vector<std::string> *ArmDAGBuilder::genConGlobal(IRGlobalVar *var){
    auto *armStmt= new std::vector<std::string>();

    const std::string& name = var->getVarName();
    armStmt->emplace_back("    .global "+name+"\n");
    armStmt->emplace_back("    .section .rodata\n");
    armStmt->emplace_back("    .align 2\n");
    armStmt->emplace_back("    .type "+name+", %object\n");
    armStmt->emplace_back("    .size "+name+","+"4\n");
    armStmt->emplace_back(var->getVarName()+":\n");
    armStmt->emplace_back("    .word " + (std::string&)var->getGlobalValue()[0]+"\n");

    return armStmt;
}

std::vector<std::string> *ArmDAGBuilder::genGlobalArray(IRGlobalVar *var){
    auto *armStmt= new std::vector<std::string>();
    int length = var->getGlobalValue()->size() * 4;

    const std::string& name = var->getVarName();
    armStmt->emplace_back("    .global "+name+"\n");
    armStmt->emplace_back("    .data\n");
    armStmt->emplace_back("    .align 2\n");
    armStmt->emplace_back("    .type "+name+", %object\n");
    armStmt->emplace_back("    .size "+name+", "+ (std::string&)length+"\n");
    armStmt->emplace_back(var->getVarName()+":\n");

    for(int i = 0; i < length/4; i++)
    {
        armStmt->emplace_back("    .word " + (std::string&)var->getGlobalValue()[i]+"\n");
    }

    return armStmt;

}

std::vector<std::string> *ArmDAGBuilder::genConGlobalArray(IRGlobalVar *var){
    auto *armStmt= new std::vector<std::string>();
    int length = var->getGlobalValue()->size() * 4;

    const std::string& name = var->getVarName();
    armStmt->emplace_back("    .global "+name+"\n");
    armStmt->emplace_back("    .section .rodata\n");
    armStmt->emplace_back("    .align 2\n");
    armStmt->emplace_back("    .type "+name+", %object\n");
    armStmt->emplace_back("    .size "+name+", "+ (std::string&)length+"\n");
    armStmt->emplace_back(var->getVarName()+":\n");

    for(int i = 0; i < length/4; i++)
    {
        armStmt->emplace_back("    .word " + (std::string&)var->getGlobalValue()[i]+"\n");
    }

    return armStmt;
}


