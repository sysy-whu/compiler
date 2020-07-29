#include "IRLocalDAG.h"
#include "../util/MyConstants.h"

#include <string>
#include<iostream>
#include<fstream>

inline DAGValue::DAGValue(DAGNode *node, unsigned resno)
        : Node(node), ResNo(resno) {}

inline void DAGUse::set(const DAGValue &V) {
  if (Val.getNode())
    removeFromList();
  Val = V;
  if (V.getNode())
    V.getNode()->addUse(*this);
}

inline void DAGUse::setInitial(const DAGValue &V) {
  Val = V;
  V.getNode()->addUse(*this);
}

inline void DAGUse::setNode(DAGNode *N) {
  if (Val.getNode())
    removeFromList();
  Val.setNode(N);
  if (N)
    N->addUse(*this);
}

void DAGRoot::FindNode(DAGNode *newNode, const char *newNodeRetName, const char *targetRetName) {
  std::vector<DAGUse *> OperandList = newNode->getOperandList();
  // 分析该targetRetName在DAG中的情况
  bool isInternalVal = AnalysisOpdName(targetRetName);

  if (isInternalVal) {
    bool isNotOnRoot = true;
    for (auto item = nodes.end()-1; item >= nodes.begin(); item--) {
      // DAGRoot存在目标node，
      if ((*item)->getRetName() == targetRetName) {
        isNotOnRoot = false;
        auto *dagUse1 = new DAGUse(DAGValue(*item, 0), newNode);
        OperandList.push_back(dagUse1);
        //  删除该节点与根节点的联系

        nodes.erase(item);
        break;
      }
      if (item == nodes.begin())
        break;
    }
    // DAGRoot不存在目标node，开始深度遍历寻找

    if (isNotOnRoot) {

      for (auto item = nodes.rbegin(); item != nodes.rend(); item++) {
        DAGNode *targetNode = DepthDAGNode(*item, targetRetName);
        if (targetNode != nullptr) {
          auto *dagUse1 = new DAGUse(DAGValue(targetNode, 0), newNode);
          OperandList.push_back(dagUse1);
        }
      }
    }
  } else {
    // 创建外部变量的node
    DAGNode *externalNode = new DAGNode(count++, DAG_EXTERNAL_DATA, targetRetName);

    dag->addNode(externalNode);

    DAGUse *dagUse1 = new DAGUse(DAGValue(externalNode, 0), newNode);
    // 添加到OperandList中
    OperandList.push_back(dagUse1);
  }
  newNode->setOperandList(OperandList);
}


bool DAGRoot::AnalysisOpdName(const char *opdName) {
  // 判断是否为临时变量

  if (opdName[1] >= '0' && opdName[1] <= '9') {
    for (auto &it : existVarList) {
      if (it == opdName) {
        // 临时变量存在直接返回true
        return true;
      }
    }
    // 临时变量不存在，先将其添加到existVarList，再返回true
    existVarList.emplace_back(opdName);
    return true;
  } else {
    for (auto &it : existVarList) {
      if (it == opdName) {
        // 存在声明变量，直接返回true
        return true;
      }
    }
    // 不存在声明变量，即为外部变量，先将其添加到existVarList，再返回false
    existVarList.emplace_back(opdName);
    return false;
  }
}

void DAGRoot::AddRet() {
    DAGNode *retNode = new DAGNode(count++, DAG_RET, "void");
    nodes.push_back(retNode);
    dag->addNode(retNode);
}

void DAGRoot::AddRet(std::string &opd1) {
  DAGNode *retNode = new DAGNode(count++, DAG_RET, "void");
  nodes.push_back(retNode);
  dag->addNode(retNode);
  FindNode(retNode, retNode->getRetName().c_str(), opd1.c_str());
}

void DAGRoot::AddBr(std::string &opd1) {
  DAGNode *brNode = new DAGNode(count++, DAG_BR, "void");
  nodes.push_back(brNode);
  dag->addNode(brNode);

  FindNode(brNode, brNode->getRetName().c_str(), opd1.c_str());
}

void DAGRoot::AddAlloca(std::string &opd1, int level) {
  // 添加块内变量
  existVarList.push_back(opd1);
  DAGNode *allocaNode =  new DAGNode(count++, DAG_ALLOCA_i32, opd1);
  allocaNode->setLevel(level);
  nodes.push_back(allocaNode);
  dag->addNode(allocaNode);
}

void DAGRoot::AddCon_Alloca(std::string &opd1, int level) {

  existVarList.push_back(opd1);
  DAGNode *conAllocaNode =new DAGNode(count++, DAG_Con_ALLOCA_i32, opd1);
  conAllocaNode->setLevel(level);
  nodes.push_back(conAllocaNode);
  dag->addNode(conAllocaNode);
}

void DAGRoot::AddGlobal(std::string &opd1) {
  existVarList.push_back(opd1);
  DAGNode *globalNode = new DAGNode(count++, DAG_GLOBAL_i32, opd1);
  nodes.push_back(globalNode);
  dag->addNode(globalNode);
}

void DAGRoot::AddCon_Global(std::string &opd1) {
  existVarList.push_back(opd1);
  DAGNode *conGlobalNode = new DAGNode(count++, DAG_Con_GLOBAL_i32, opd1);
  nodes.push_back(conGlobalNode);
  dag->addNode(conGlobalNode);
}

void DAGRoot::AddStore(std::string &opd1, std::string &opd2) {
  DAGNode *storeNode = new DAGNode(count++, DAG_STORE, opd1);
  nodes.push_back(storeNode);
  dag->addNode(storeNode);
  FindNode(storeNode, storeNode->getRetName().c_str(), opd1.c_str());
  FindNode(storeNode, storeNode->getRetName().c_str(), opd2.c_str());
}

void DAGRoot::AddStore(std::string &opd1, int opd2) {
  DAGNode *storeNode = new DAGNode(count++, DAG_STORE, opd1);
  nodes.push_back(storeNode);
  dag->addNode(storeNode);
  FindNode(storeNode, storeNode->getRetName().c_str(), opd1.c_str());

  // 创建立即数的node
  DAGNode *immedNode = new ImmediateDAGNode(count++, opd2);
  DAGUse *dagUse2 = new DAGUse(DAGValue(immedNode, 0), storeNode);
  std::vector<DAGUse *> OperandList = storeNode->getOperandList();
  OperandList.push_back(dagUse2);
  storeNode->setOperandList(OperandList);

  dag->addNode(immedNode);
}

void DAGRoot::AddLoad(std::string &opd1, std::string &opd2) {
  DAGNode *loadNode = new DAGNode(count++, DAG_LOAD, opd1);
  nodes.push_back(loadNode);
  dag->addNode(loadNode);

  FindNode(loadNode, loadNode->getRetName().c_str(), opd1.c_str());
  FindNode(loadNode, loadNode->getRetName().c_str(), opd2.c_str());
}

void DAGRoot::AddLoad(std::string &opd1, int opd2) {
  DAGNode *loadNode = new DAGNode(count++, DAG_STORE, opd1);
  nodes.push_back(loadNode);
  dag->addNode(loadNode);

  FindNode(loadNode, loadNode->getRetName().c_str(), opd1.c_str());

  // 创建立即数的node
  DAGNode *immedNode = new ImmediateDAGNode(count++, opd2);
  DAGUse *dagUse2 = new DAGUse(DAGValue(immedNode, 0), loadNode);
  std::vector<DAGUse *> OperandList = loadNode->getOperandList();
  OperandList.push_back(dagUse2);
  loadNode->setOperandList(OperandList);

  dag->addNode(immedNode);
}

void DAGRoot::AddAlloca_Array(std::string &opd1, std::vector<std::string> dismensionList, int level) {
  existVarList.push_back(opd1);
  DAGNode *allocaArrayNode = new DAGNode(count++, DAG_ALLOCA_i32_ARRAY, opd1);
  allocaArrayNode->setLevel(level);
  nodes.push_back(allocaArrayNode);
  dag->addNode(allocaArrayNode);
  for (auto &item:dismensionList) {
    FindNode(allocaArrayNode, allocaArrayNode->getRetName().c_str(), item.c_str());
  }
}

void DAGRoot::AddCon_Alloca_Array(std::string &opd1, std::vector<std::string> dismensionList, int level) {
  existVarList.push_back(opd1);
  DAGNode *conAllocaArrayNode = new DAGNode(count++, DAG_Con_ALLOCA_ARRAY_i32, opd1);
  conAllocaArrayNode->setLevel(level);
  nodes.push_back(conAllocaArrayNode);
  dag->addNode(conAllocaArrayNode);
  for (auto &item:dismensionList) {
    FindNode(conAllocaArrayNode, conAllocaArrayNode->getRetName().c_str(), item.c_str());
  }
}

void DAGRoot::AddGlobal_Array(std::string &opd1, std::vector<std::string> dismensionList) {
  existVarList.push_back(opd1);
  DAGNode *globalArrayNode = new DAGNode(count++, DAG_GLOBAL_i32_ARRAY, opd1);
  nodes.push_back(globalArrayNode);
  dag->addNode(globalArrayNode);

  for (auto &item:dismensionList) {
    FindNode(globalArrayNode, globalArrayNode->getRetName().c_str(), item.c_str());
  }
}

void DAGRoot::AddCon_Global_Array(std::string &opd1, std::vector<std::string> dismensionList) {
  existVarList.push_back(opd1);
  DAGNode *conGlobalArrayNode = new DAGNode(count++, DAG_Con_GLOBAL_ARRAY_i32, opd1);
  nodes.push_back(conGlobalArrayNode);
  dag->addNode(conGlobalArrayNode);

  for (auto &item:dismensionList) {
    FindNode(conGlobalArrayNode, conGlobalArrayNode->getRetName().c_str(), item.c_str());
  }

}

void DAGRoot::AddBO(std::string &opd1, std::string &opd2, std::string &opd3, int opType) {
  DAGNode *opNode = new DAGNode(count++, opType, opd1);
  nodes.push_back(opNode);
  dag->addNode(opNode);
  FindNode(opNode, opNode->getRetName().c_str(), opd2.c_str());
  FindNode(opNode, opNode->getRetName().c_str(), opd3.c_str());
}

void DAGRoot::AddUO(std::string &opd1, std::string &opd2, int opType) {
    DAGNode *opNode = new DAGNode(count++, opType, opd1);
    nodes.push_back(opNode);
    dag->addNode(opNode);
    FindNode(opNode, opNode->getRetName().c_str(), opd2.c_str());
}

void DAGRoot::AddBr(std::string &opd1, std::string &opd2, std::string &opd3) {
  DAGNode *brNode = new DAGNode(count++, DAG_BR, "void");
  nodes.push_back(brNode);
  dag->addNode(brNode);

  FindNode(brNode, brNode->getRetName().c_str(), opd2.c_str());
  FindNode(brNode, brNode->getRetName().c_str(), opd3.c_str());
}

void DAGRoot::AddGetPtr(std::string &opd1, std::string &opd2, std::string &opd3) {
  DAGNode *getPtrNode = new DAGNode(count++, DAG_GETPTR, opd1);
  nodes.push_back(getPtrNode);
  dag->addNode(getPtrNode);

  FindNode(getPtrNode, getPtrNode->getRetName().c_str(), opd2.c_str());
  FindNode(getPtrNode, getPtrNode->getRetName().c_str(), opd3.c_str());
}

void DAGRoot::AddGetPtr(std::string &opd1, int opd2, std::string &opd3) {
  DAGNode *getPtrNode = new DAGNode(count++, DAG_GETPTR, opd1);
  nodes.push_back(getPtrNode);
  dag->addNode(getPtrNode);

  FindNode(getPtrNode, getPtrNode->getRetName().c_str(), opd3.c_str());

  // 创建立即数的node
  DAGNode *immedNode = new ImmediateDAGNode(count++, opd2);
  dag->addNode(immedNode);

  DAGUse *dagUse2 = new DAGUse(DAGValue(immedNode, 0), getPtrNode);
  std::vector<DAGUse *> OperandList = getPtrNode->getOperandList();
  OperandList.push_back(dagUse2);
  getPtrNode->setOperandList(OperandList);
}

void DAGRoot::AddGetPtr(std::string &opd1, std::string &opd2, std::vector<std::string> &opd3) {
    DAGNode *getPtrNode = new DAGNode(count++, DAG_GETPTR, opd1);
    nodes.push_back(getPtrNode);
    dag->addNode(getPtrNode);

    FindNode(getPtrNode, getPtrNode->getRetName().c_str(), opd2.c_str());
    for (auto &item:opd3) {
        FindNode(getPtrNode, getPtrNode->getRetName().c_str(), item.c_str());
    }
}

void DAGRoot::AddCall(std::string &opd1, std::string &opd2, std::vector<std::string> &paramList) {
  DAGNode *callNode = new DAGNode(count++, DAG_CALL, opd1);
  nodes.push_back(callNode);
  dag->addNode(callNode);

  FindNode(callNode, callNode->getRetName().c_str(), opd2.c_str());

  for (auto &item:paramList) {
    FindNode(callNode, callNode->getRetName().c_str(), item.c_str());
  }
}

DAGRoot::DAGRoot() {
    dag = new DAG(this, "sad");
}


void DAG::generateDOT(){
    std::ofstream fout;
    std::string cont;

    std::string fname  = "../output/" + Name + ".txt";

    fout.open( fname, std::ios::out | std::ios::app);
    if(!fout.is_open())
        std::perror("Failed to open file dot file!" );

    // start building the graph in DOT language
    cont += "digraph struct { \n "
            " rankdir = BT \n"
            "node [shape=record]; \n";


    /// Node Structure

    ///  ----------------
    ///  | OperandList   |
    ///  ----------------
    ///  |   opType      |
    ///  ----------------
    ///  |  retName      |
    ///  ----------------

    for (DAGNode* nd : Nodes){
        std::string id = std::to_string( nd->getID());
        cont += "node" + id + " [label=\"{ ";

        //operandList  e.g.  {<n1i1> a | <n1i2> b }
        std::vector<DAGUse*> opList= nd->getOperandList();
        int opNum = opList.size();
        if(opNum == 0){
            cont += " {} ";
        }
        else{
            std::string opStr = "{";
            for( int i = 0; i < opNum; i++){
                opStr += " <n"+ std::to_string(nd->getID()) + "i" + std::to_string(i+1) + ">";
                opStr += opList[i]->Val.getNode()->getRetName() + " |";
            }
            opStr.pop_back(); //remove the surplus '|'
            cont += opStr + " }";
        }


        // opType   e.g.  store
        cont += " | " + std::to_string( nd->getOpType()) + " | ";


        //retName     e.g.  <n1i0> a
        cont += "<n" + std::to_string( nd->getID()) + "i0" + nd->getRetName();

        cont += "}\"] \n";


        ///  Node Connection  e.g.   node2: n2i1 -> node1:n1i0
        std::string connStr = "";
        for( int j = 0; j < opNum; j++){

            connStr += "node" + std::to_string(nd->getID()) +
                       ": n" + std::to_string(nd->getID()) + std::to_string(j+1) ;

            connStr += " -> ";

            connStr += "node" + std::to_string(opList[j]->Val.getNode()->getID()) +
                       ": n" + std::to_string(opList[j]->Val.getNode()->getID()) + "i0" + " \n";

            cont += connStr;
            connStr = "";
        }
    }

    cont += "}";
    fout << cont;
    fout.close();
}

void DAG::setTopoList() {


    int length = Nodes.size();
    // 动态分配一维数组
    int *inDegree = (int *)malloc(sizeof(int)*length);
    // 动态分配二维数组
    int **adjace = (int **)malloc(sizeof(int *)*length);
    for (int k = 0; k < length; ++k) {
        adjace[k] = (int *)malloc(sizeof(int)*length);
    }
    for (int i = 0; i< length; i++)
        for(int j = 0; j<length; j++)
            adjace[i][j] = 0;

    std::queue<DAGNode*> queue;

    // get inDegree and adjace
    for (int i = length; i<length; i++){
        DAGNode *nd = Nodes[i];

        std::vector<DAGUse*> opList = nd->getOperandList();
        inDegree[i] = opList.size();

        for( int j = 0; j < opList.size() ; j++){
            int prev = opList[j]->getNode()->getID(); // prev node
            adjace[j][prev] = 1;
        }
    }


    // get ID topological order
    std::queue<int> q;
    std::queue<int> seq;

    int temp;
    while (!q.empty()){

        temp = q.front();
        seq.push(temp);
        q.pop();

        for(int i = 1; i<= length; i++){
            if(adjace[temp][i]){
                inDegree[i] --;
                if(inDegree[i]==0)
                    q.push(i);
            }
        }
    }

    if(seq.size() != length){
        printf("No topological order!");
        return;
    }

    int tmp;
    for(int i = 0; i< seq.size(); i++){
        tmp = seq.front();
        seq.pop();
        queue.push(Nodes[tmp]);
    }
}



















