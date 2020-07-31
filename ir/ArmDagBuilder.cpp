#include "ArmDagBuilder.h"
#include "ArmDag.h"
#include "IRGlobal.h"

//主方法  遍历DAG生成ARMDAG
void ArmDAGBuilder::generateArmDag(const char *blockName, std::vector<IRGlobalFuncParam *> *parmsInfo) {
    if (blockName == "entry") {
        // todo 添加函数入栈和获取实参的armnode

    }
    //TODO



    return;
}

/*
 * 运算语句
 */

ArmNodes ArmDAGBuilder::genAddNode(DAGNode *nd){
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), ADD, opd1,opd2,opd3);
    return ArmNodes{node,node};
}

ArmNodes ArmDAGBuilder::genSubNode(DAGNode *nd){
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), SUB, opd1,opd2,opd3);
    return ArmNodes{node,node};
}

ArmNodes ArmDAGBuilder::genMulNode(DAGNode *nd){
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), MUL, opd1,opd2,opd3);
    return ArmNodes{node,node};
}


ArmNodes ArmDAGBuilder::genModNode(DAGNode *nd){
    //TODO: 在.s文件开头添加    .global __aeabi_imod

    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), MOV, (std::string &) "$r0",opd2);
    ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(),MOV,(std::string &) "$r1",opd3);
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),BL,opd1,(std::string &) "__aeabi_imod");
    ArmDAGNode *node3 = new ArmDAGNode(count++, nd->getID(),MOV,opd1,(std::string &) "$r0");

    node1->addDependUse(node);
    node2->addDependUse(node1);
    node3->addDependUse(node2);

    return ArmNodes{node,node3};

}


ArmNodes ArmDAGBuilder::genDivNode(DAGNode *nd){
    //TODO: 在.s文件开头添加    .global __aeabi_idiv

    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), MOV, (std::string &) "$R0",opd2);
    ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(),MOV,(std::string &) "$R1",opd3);
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),BL,opd1,(std::string &) "__aeabi_idiv");
    ArmDAGNode *node3 = new ArmDAGNode(count++, nd->getID(),MOV,opd1,(std::string &) "$r0");

    node1->addDependUse(node);
    node2->addDependUse(node1);
    node3->addDependUse(node2);

    return ArmNodes{node,node3};
}




ArmNodes ArmDAGBuilder::genEQNode(DAGNode *nd){

    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), CMP, opd2,opd3);
    ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(),MOVEQ,opd1,(std::string &) "1");
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),MOVNE,opd1,(std::string &) "0");

    node1->addDependUse(node);
    node2->addDependUse(node1);

    return ArmNodes{node,node2};

}

ArmNodes ArmDAGBuilder::genNEQNode(DAGNode *nd){

    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), CMP, opd2,opd3);
    ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(),MOVNE,opd1,(std::string &) "1");
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),MOVEQ,opd1,(std::string &) "0");

    node1->addDependUse(node);
    node2->addDependUse(node1);

    return ArmNodes{node,node2};
}

ArmNodes ArmDAGBuilder::genGTNode(DAGNode *nd){
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), CMP, opd2,opd3);
    ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(),MOVGT,opd1,(std::string &) "1");
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),MOVLE,opd1,(std::string &) "0");

    node1->addDependUse(node);
    node2->addDependUse(node1);

    return ArmNodes{node,node2};
}

ArmNodes ArmDAGBuilder::genLTNode(DAGNode *nd){
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), CMP, opd2,opd3);
    ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(),MOVLT,opd1,(std::string &) "1");
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),MOVGE,opd1,(std::string &) "0");

    node1->addDependUse(node);
    node2->addDependUse(node1);

    return ArmNodes{node,node2};
}

ArmNodes ArmDAGBuilder::genGTENode(DAGNode *nd){
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), CMP, opd2,opd3);
    ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(),MOVGE,opd1,(std::string &) "1");
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),MOVLT,opd1,(std::string &) "0");

    node1->addDependUse(node);
    node2->addDependUse(node1);

    return ArmNodes{node,node2};
}

ArmNodes ArmDAGBuilder::genLTENode(DAGNode *nd){
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), CMP, opd2,opd3);
    ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(),MOVLE,opd1,(std::string &) "1");
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),MOVGT,opd1,(std::string &) "0");

    node1->addDependUse(node);
    node2->addDependUse(node1);

    return ArmNodes{node,node2};
}

ArmNodes ArmDAGBuilder::genAndNode(DAGNode *nd){
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++, nd->getID(),ADD,opd1,opd2,opd3);
    ArmDAGNode *node1 = new ArmDAGNode(count++,  nd->getID(), CMP, opd1,(std::string&)"1");
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),MOVEQ,opd1,(std::string &) "1");
    ArmDAGNode *node3 = new ArmDAGNode(count++, nd->getID(),MOVNE,opd1,(std::string &) "0");


    node1->addDependUse(node);
    node2->addDependUse(node1);
    node3->addDependUse(node2);

    return ArmNodes{node,node3};
}

ArmNodes ArmDAGBuilder::genOrNode(DAGNode *nd){

    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++, nd->getID(),ADD,opd1,opd2,opd3);
    ArmDAGNode *node1 = new ArmDAGNode(count++,  nd->getID(), CMP, opd1,(std::string&)"1");
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),MOVGE,opd1,(std::string &) "1");
    ArmDAGNode *node3 = new ArmDAGNode(count++, nd->getID(),MOVLT,opd1,(std::string &) "0");


    node1->addDependUse(node);
    node2->addDependUse(node1);
    node3->addDependUse(node2);

    return ArmNodes{node,node3};
}

// 单目运算

ArmNodes ArmDAGBuilder::genPlus(DAGNode *nd){ // +
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++, nd->getID(),MOV,opd1, opd2);

    return ArmNodes{node,node};
}

ArmNodes ArmDAGBuilder::genMinus(DAGNode *nd){ // -
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++, nd->getID(),MUL,opd1,opd2, (std::string&) "-1");

    return ArmNodes{node,node};
}

ArmNodes ArmDAGBuilder::genExclamation(DAGNode *nd){ // !
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++, nd->getID(),CMP,opd2,(std::string&) "0");
    ArmDAGNode *node1 = new ArmDAGNode(count++,  nd->getID(), MOVEQ, opd1,(std::string&)"0");
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),MOVNE,opd1,(std::string &) "1");

    node1->addDependUse(node);
    node2->addDependUse(node1);

    return ArmNodes{node,node2};
}



/*
 * 声明语句
 */

ArmNodes ArmDAGBuilder::genAlloca(DAGNode *nd){

    std::string opd1 = nd->getOperandList()[0]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), SUB, (std::string &) "$SP", (std::string &)"$SP", (std::string &)"4");

    //记录变量位置
    std::multimap<std::string,VarInfo> varMap =  * (status->getVarMap());
    varMap.emplace(opd1,VarInfo{status->getCurrentLoc(),nd->getLevel()});
    int loc = status->getCurrentLoc() + 4;
    status->setCurrentLoc( loc );
    status->setVarMap(&varMap);

    return ArmNodes{node,node};
}

ArmNodes ArmDAGBuilder::genConAlloca(DAGNode *nd){
    std::string opd1 = nd->getOperandList()[0]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), SUB, (std::string &) "$SP", (std::string &)"$SP", (std::string &)"4");

    //记录变量位置
    std::multimap<std::string,VarInfo> varMap =  * (status->getVarMap());
    varMap.emplace(opd1,VarInfo{status->getCurrentLoc(),nd->getLevel()});
    int loc = status->getCurrentLoc() + 4;
    status->setCurrentLoc( loc );
    status->setVarMap(&varMap);

    return ArmNodes{node,node};
}

ArmNodes ArmDAGBuilder::genAllocaArray(DAGNode *nd){

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

    return ArmNodes{node,node};
}

ArmNodes ArmDAGBuilder::genConAllocaArray(DAGNode *nd){

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

    return ArmNodes{node,node};
}

/*
 * 存取语句
 */

ArmNodes ArmDAGBuilder::genLoadNode(DAGNode *nd){
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

        return ArmNodes{node,node2};
    }
    else{
        addr = "[fp, #-"+(std::string&)loc+"]";

        ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), LDR, opd1, addr);
        return ArmNodes{node,node};
    }

}

ArmNodes ArmDAGBuilder::genStoreNode(DAGNode *nd){
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
        addr = "[R2]";  //FIXME 这里用了R2寄存器来保存地址  !!!
        std::string lower =  "#:lower16:"+ opd1;    std::string upper =  "#:upper16:"+ opd1;

        ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), MOVW, (std::string&)"R2", lower);
        ArmDAGNode *node1 = new ArmDAGNode(count++,  nd->getID(), MOVT,(std::string&)"R2", upper);
        ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(), MOV, opd1, opd2);
        ArmDAGNode *node3 = new ArmDAGNode(count++,  nd->getID(), LDR, opd1, addr);

        node1->addDependUse(node);
        node2->addDependUse(node1);
        node3->addDependUse(node2);

        return ArmNodes{node,node3};
    }
    else{
        addr = "[fp, #-"+(std::string&)loc+"]";

        ArmDAGNode *node = new ArmDAGNode(count++, nd->getID(), MOV, opd1, opd2);
        ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(), STR, opd1,addr);
        node1->addDependUse(node);
        return ArmNodes{node,node1};
    }

}

ArmNodes ArmDAGBuilder::genGetPtrNode(DAGNode *nd){

    // 加载各个参数 opd3为保存各维数字的vector
    std::string opd1 = nd->getOperandList()[0]->getNode()->getRetName();
    std::string opd2 = nd->getOperandList()[1]->getNode()->getRetName();

    std::string addr;

    int length = nd->getOperandList().size()-2;
    std::vector<std::string> opd3; // opd3 保存各维参数
    for(int i = 0; i < length; i++){
        std::string dim = nd->getOperandList()[i+2]->getNode()->getRetName();
        opd3.emplace_back(dim);
    }

    std::multimap<std::string,ArrayInfo> *dimMap = status->getArrDimensionMap();
    std::multimap<std::string ,VarInfo> *varMap = status->getVarMap();

    // 查找位置 loc
    int loc; int currLevel = nd->getLevel(); int level = -1;
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
        perror("Compile Error: compile error: genGetPtrNode - array not found!\"\n");
    }

    // 查找维度 dims
    std::vector<int> dims;    int arrLevel = -1;
    int n2 = varMap->count(opd2);
    if(n2 == 1){
        ArrayInfo info = dimMap->find(opd2)->second;
        dims = info.dimension;
    }
    else if ( n2 > 1 ){
        auto iter = dimMap->find(opd2);
        ArrayInfo result = iter->second;
        for(int i = 0; i < n2; i++){
            ArrayInfo info = iter->second;
            if(level == info.level){
                result = info;
            }
            iter++;
        }
        dims = result.dimension;
    }
    else{ // not found
        perror("Compile Error: compile error: genGetPtrNode - array not found!\"\n");
    }

    int dimNum = dims.size();
    std::vector<int> dimScale; // dimScale[i] 表示 dim[i], dim[i+1], ...dim[dimNum-1] 之积. 最末元素为1
    for(int i = dimNum; i > 0; i++){
        int scale = 1;
        for( int j = i; j>0; j++){
            scale *= dims[j];
        }
       dimScale.emplace_back(scale);
    }
    dimScale.emplace_back(1);
    /*
     *  定义: array[dim1][dim2]..[dimn]
     *  调用: array[num1][num2]...[numn]
     *  偏移 = base_address - num1 * (dim2)*(dim3)... -num2 * (dim3)* ... - numn
     */


    if(level == 0){ // 为全局变量 采用特殊取址方式

        addr = "addr";

        std::string lower =  "#:lower16:"+ opd2;    std::string upper =  "#:upper16:"+ opd2;

        // opd1 保存数组基址
        ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), MOVW, opd1, lower);
        ArmDAGNode *node1 = new ArmDAGNode(count++,  nd->getID(), MOVT,opd1, upper);


        // 计算相对基址的偏移
        std::vector<ArmDAGNode*> calcuNodes;
        for(int i = 0; i < opd3.size(); i++){
            calcuNodes.emplace_back(new ArmDAGNode(count++,  nd->getID(), MUL,(std::string&) "$R2", opd3[i], (std::string&)dimScale[i+1]));
            calcuNodes.emplace_back(new ArmDAGNode(count++,  nd->getID(), SUB, opd1, opd1, (std::string&)"$R2"));
        }

        node1->addDependUse(node);
        int tmp = 0; int instNum = calcuNodes.size();// 指令数量
        ArmDAGNode * prev = node1;
        while(tmp < instNum){
            calcuNodes[tmp]->addDependUse(prev);
            calcuNodes[tmp+1];
            calcuNodes[tmp+1]->addDependUse(calcuNodes[tmp]);
            prev = calcuNodes[tmp+1];
            tmp +=2;
        }

        return ArmNodes{node,prev};
    }
    else{
        addr = opd1;

        // opd1 寄存器 保存数组基址
        ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), SUB, opd1,(std::string&)"$FP", (std::string&)loc);

        // 计算相对基址的偏移
        std::vector<ArmDAGNode*> calcuNodes;
        for(int i = 0; i < opd3.size(); i++){
            calcuNodes.emplace_back(new ArmDAGNode(count++,  nd->getID(), MUL,(std::string&) "$R2", opd3[i], (std::string&)dimScale[i+1]));
            calcuNodes.emplace_back(new ArmDAGNode(count++,  nd->getID(), SUB, opd1, opd1, (std::string&)"$R2"));
        }

        int tmp = 0; int instNum = calcuNodes.size();// 指令数量
        ArmDAGNode * prev = node;
        while(tmp < instNum){
            calcuNodes[tmp]->addDependUse(prev);
            calcuNodes[tmp+1];
            calcuNodes[tmp+1]->addDependUse(calcuNodes[tmp]);
            prev = calcuNodes[tmp+1];
            tmp +=2;
        }

        return ArmNodes{node,prev};
    }

}

/*
 * 函数调用
 */

ArmNodes ArmDAGBuilder::genCallNode(DAGNode *nd){
    std::string opd2 = nd->getOperandList()[1]->getNode()->getRetName();
    // 无返回值时 opd1为""

    //  传参处理
    int paramNum = nd->getOperandList().size() - 2;
    if(paramNum <= 0){ //无参数
        ArmDAGNode *node = new ArmDAGNode(count++, nd->getID(),BL, opd2);
        return ArmNodes{node,node};
    }
    else{ // 有参数

        //FIXME: 暂未考虑数组相关
        std::string s ="{"+nd->getOperandList()[2]->getNode()->getRetName()+"}";
        ArmDAGNode *node = new ArmDAGNode(count++, nd->getID(),PUSH,s);
        ArmDAGNode *prev = node;

        for(int i = 1; i < paramNum; i++){
            std::string tmp = "{" + nd->getOperandList()[i+2]->getNode()->getRetName() + "}";
            ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(),PUSH, tmp);
            node1->addDependUse(prev);
            prev = node1;
        }

        ArmDAGNode *BLNode = new ArmDAGNode(count++, nd->getID(),BL, opd2);
        BLNode->addDependUse(prev);

        return {node, BLNode};
    }

}


/*
 * 控制语句
 */

ArmNodes ArmDAGBuilder::genBRNode(DAGNode *nd){
    std::string opd2 = nd->getOperandList()[1]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++, nd->getID(),B, opd2);
    return ArmNodes{node,node};
}

ArmNodes ArmDAGBuilder::genRetNode(DAGNode *nd){

    std::string opd1 = nd->getOperandList()[0]->getNode()->getRetName();
    //ArmDAGNode *node = new ArmDAGNode(count++, nd->getID(), MOV, (std::string &)"$R0", opd1);
    //ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(), BX, (std::string &)"$LR");

    ArmDAGNode *node = new ArmDAGNode(count++, nd->getID(), MOV, (std::string &)"$R0", opd1);
    ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(), SUB, (std::string &)"$SP",
                                       (std::string &)"$FP", (std::string &)"4");
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(), POP, (std::string &)"{fp,pc}");

    node1->addDependUse(node);
    node2->addDependUse(node1);

    return ArmNodes{node,node2};

}


ArmNodes ArmDAGBuilder::genBRCondNode(DAGNode *nd){
    std::vector<DAGUse*> opList = nd->getOperandList();

    std::string opd1 = opList[0]->getNode()->getRetName();
    std::string opd2 = opList[1]->getNode()->getRetName();
    std::string opd3 = opList[2]->getNode()->getRetName();

    ArmDAGNode *node = new ArmDAGNode(count++,  nd->getID(), CMP, opd1,(std::string&)"1");
    ArmDAGNode *node1 = new ArmDAGNode(count++, nd->getID(),BEQ,opd2);
    ArmDAGNode *node2 = new ArmDAGNode(count++, nd->getID(),B,opd3);

    node1->addDependUse(node);
    node2->addDependUse(node1);

    return ArmNodes{node,node2};
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

ArmNodes ArmDAGBuilder::genEntryParam(std::vector<IRGlobalFuncParam *> *parmsInfo){

    int entryCount = -1;


    int paramNum = parmsInfo->size();
    if( paramNum != 0){

        //把实参逐个load到当前栈内并记录  当前栈情况：   param1 | param2 ...
        ArmDAGNode *head = nullptr; ArmDAGNode *tail = nullptr;
        for( int i = 0; i< paramNum; i++){

            // ldr r, [fp,#4]
            // sub sp, sp, 4
            // str r, [fp, #能存到的位置]

            std::string addr1 = (std::string)"[FP,#" + std::to_string(paramNum*4) + "]";
            std::string addr2 = (std::string)"[FP,#" + std::to_string(i*4) + "]";

            ArmDAGNode *node0 =  new ArmDAGNode(entryCount--, -1, LDR, (std::string &)"$R2",addr1);
            ArmDAGNode *node1 =  new ArmDAGNode(entryCount--, -1, SUB, (std::string &)"$SP",(std::string &)"$SP", (std::string &)"4");
            ArmDAGNode *node2 =  new ArmDAGNode(entryCount--, -1, STR, (std::string &)"$R2",addr2);

            if(head == nullptr)
                head = node0;
            tail = node2;

            node1->addDependUse(node0);
            node2->addDependUse(node1);

            //在stackStatus中记录变量位置 level为1
            std::multimap<std::string,VarInfo> varMap = * status->getVarMap();
            std::string varName = parmsInfo->at(i)->getParamName();
            int loc = status->getCurrentLoc() + 4;
            varMap.insert({varName, VarInfo{loc,1}});
            status->setCurrentLoc(loc);
            status->setVarMap(&varMap);
        }

        // PUSH {fp,lr}    ADD fp, sp, #4
        ArmDAGNode *nodePush =  new ArmDAGNode(entryCount--, -1, PUSH, (std::string &)"{fp,lr}");
        ArmDAGNode *nodeAdd =  new ArmDAGNode(entryCount--, -1, ADD, (std::string &)"$FP",(std::string &)"$SP", (std::string &)"4");

        nodePush->addDependUse(tail);
        nodeAdd->addDependUse(nodePush);

        return ArmNodes{nodePush,nodeAdd};
    }
    else{ // 无参数

        // PUSH {fp,lr}    ADD fp, sp, #4
        ArmDAGNode *node =  new ArmDAGNode(entryCount--, -1, PUSH, (std::string &)"{fp,lr}");
        ArmDAGNode *node1 =  new ArmDAGNode(entryCount--, -1, ADD, (std::string &)"$FP",
                (std::string &)"$SP", (std::string &)"4");

        node1->addDependUse(node);
        return ArmNodes{node,node1};

    }
}
