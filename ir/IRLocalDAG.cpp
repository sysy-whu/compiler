#include "IRLocalDAG.h"
#include "../util/MyConstants.h"

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

void DAGRoot::AddRet(std::string &opd1) {
  DAGNode *retNode = new DAGNode(count++, DAG_RET, "void");
  nodes.push_back(retNode);
  FindNode(retNode, retNode->getRetName().c_str(), opd1.c_str());
}

void DAGRoot::AddBr(std::string &opd1) {
  DAGNode *brNode = new DAGNode(count++, DAG_BR, "void");
  nodes.push_back(brNode);
  FindNode(brNode, brNode->getRetName().c_str(), opd1.c_str());
}

void DAGRoot::AddAlloca(std::string &opd1) {
  // 添加块内变量
  existVarList.push_back(opd1);
  nodes.push_back(new DAGNode(count++, DAG_ALLOCA_i32, opd1));
}

void DAGRoot::AddCon_Alloca(std::string &opd1) {

  existVarList.push_back(opd1);
  nodes.push_back(new DAGNode(count++, DAG_Con_ALLOCA_i32, opd1));
}

void DAGRoot::AddGlobal(std::string &opd1) {
  existVarList.push_back(opd1);
  nodes.push_back(new DAGNode(count++, DAG_GLOBAL_i32, opd1));
}

void DAGRoot::AddCon_Global(std::string &opd1) {
  existVarList.push_back(opd1);
  nodes.push_back(new DAGNode(count++, DAG_Con_GLOBAL_i32, opd1));
}

void DAGRoot::AddStore(std::string &opd1, std::string &opd2) {
  DAGNode *storeNode = new DAGNode(count++, DAG_STORE, opd1);
  nodes.push_back(storeNode);
  FindNode(storeNode, storeNode->getRetName().c_str(), opd1.c_str());
  FindNode(storeNode, storeNode->getRetName().c_str(), opd2.c_str());
}

void DAGRoot::AddStore(std::string &opd1, int opd2) {
  DAGNode *storeNode = new DAGNode(count++, DAG_STORE, opd1);
  nodes.push_back(storeNode);
  FindNode(storeNode, storeNode->getRetName().c_str(), opd1.c_str());

  // 创建立即数的node
  DAGNode *immedNode = new ImmediateDAGNode(count++, opd2);
  DAGUse *dagUse2 = new DAGUse(DAGValue(immedNode, 0), storeNode);
  std::vector<DAGUse *> OperandList = storeNode->getOperandList();
  OperandList.push_back(dagUse2);
  storeNode->setOperandList(OperandList);
}

void DAGRoot::AddLoad(std::string &opd1, std::string &opd2) {
  DAGNode *loadNode = new DAGNode(count++, DAG_LOAD, opd1);
  nodes.push_back(loadNode);

  FindNode(loadNode, loadNode->getRetName().c_str(), opd1.c_str());
  FindNode(loadNode, loadNode->getRetName().c_str(), opd2.c_str());
}

void DAGRoot::AddLoad(std::string &opd1, int opd2) {
  DAGNode *loadNode = new DAGNode(count++, DAG_STORE, opd1);
  nodes.push_back(loadNode);

  FindNode(loadNode, loadNode->getRetName().c_str(), opd1.c_str());

  // 创建立即数的node
  DAGNode *immedNode = new ImmediateDAGNode(count++, opd2);
  DAGUse *dagUse2 = new DAGUse(DAGValue(immedNode, 0), loadNode);
  std::vector<DAGUse *> OperandList = loadNode->getOperandList();
  OperandList.push_back(dagUse2);
  loadNode->setOperandList(OperandList);
}

void DAGRoot::AddAlloca_Array(std::string &opd1, std::vector<std::string> dismensionList) {
  existVarList.push_back(opd1);
  DAGNode *allocaArrayNode = new DAGNode(count++, DAG_ALLOCA_i32_ARRAY, opd1);
  nodes.push_back(allocaArrayNode);
  for (auto &item:dismensionList) {
    FindNode(allocaArrayNode, allocaArrayNode->getRetName().c_str(), item.c_str());
  }
}

void DAGRoot::AddCon_Alloca_Array(std::string &opd1, std::vector<std::string> dismensionList) {
  existVarList.push_back(opd1);
  DAGNode *conAllocaArrayNode = new DAGNode(count++, DAG_Con_ALLOCA_ARRAY_i32, opd1);
  nodes.push_back(conAllocaArrayNode);
  for (auto &item:dismensionList) {
    FindNode(conAllocaArrayNode, conAllocaArrayNode->getRetName().c_str(), item.c_str());
  }
}

void DAGRoot::AddGlobal_Array(std::string &opd1, std::vector<std::string> dismensionList) {
  existVarList.push_back(opd1);
  DAGNode *globalArrayNode = new DAGNode(count++, DAG_GLOBAL_i32_ARRAY, opd1);
  nodes.push_back(globalArrayNode);
  for (auto &item:dismensionList) {
    FindNode(globalArrayNode, globalArrayNode->getRetName().c_str(), item.c_str());
  }
}

void DAGRoot::AddCon_Global_Array(std::string &opd1, std::vector<std::string> dismensionList) {
  existVarList.push_back(opd1);
  DAGNode *conGlobalArrayNode = new DAGNode(count++, DAG_Con_GLOBAL_ARRAY_i32, opd1);
  nodes.push_back(conGlobalArrayNode);
  for (auto &item:dismensionList) {
    FindNode(conGlobalArrayNode, conGlobalArrayNode->getRetName().c_str(), item.c_str());
  }

}

void DAGRoot::AddOp(std::string &opd1, std::string &opd2, std::string &opd3, int opType) {
  DAGNode *opNode = new DAGNode(count++, opType, opd1);
  nodes.push_back(opNode);
  FindNode(opNode, opNode->getRetName().c_str(), opd2.c_str());
  FindNode(opNode, opNode->getRetName().c_str(), opd3.c_str());
}

void DAGRoot::AddBr(std::string &opd1, std::string &opd2, std::string &opd3) {
  DAGNode *brNode = new DAGNode(count++, DAG_BR, "void");
  nodes.push_back(brNode);

  FindNode(brNode, brNode->getRetName().c_str(), opd2.c_str());
  FindNode(brNode, brNode->getRetName().c_str(), opd3.c_str());
}

void DAGRoot::AddGetPtr(std::string &opd1, std::string &opd2, std::string &opd3) {
  DAGNode *getPtrNode = new DAGNode(count++, DAG_GETPTR, opd1);
  nodes.push_back(getPtrNode);

  FindNode(getPtrNode, getPtrNode->getRetName().c_str(), opd2.c_str());
  FindNode(getPtrNode, getPtrNode->getRetName().c_str(), opd3.c_str());
}

void DAGRoot::AddGetPtr(std::string &opd1, int opd2, std::string &opd3) {
  DAGNode *getPtrNode = new DAGNode(count++, DAG_GETPTR, opd1);
  nodes.push_back(getPtrNode);

  FindNode(getPtrNode, getPtrNode->getRetName().c_str(), opd3.c_str());

  // 创建立即数的node
  DAGNode *immedNode = new ImmediateDAGNode(count++, opd2);
  DAGUse *dagUse2 = new DAGUse(DAGValue(immedNode, 0), getPtrNode);
  std::vector<DAGUse *> OperandList = getPtrNode->getOperandList();
  OperandList.push_back(dagUse2);
  getPtrNode->setOperandList(OperandList);
}

void DAGRoot::AddCall(std::string &opd1, std::string &opd2, std::vector<std::string> &paramList) {
  DAGNode *callNode = new DAGNode(count++, DAG_CALL, opd1);
  nodes.push_back(callNode);

  FindNode(callNode, callNode->getRetName().c_str(), opd2.c_str());

  for (auto &item:paramList) {
    FindNode(callNode, callNode->getRetName().c_str(), item.c_str());
  }
}





















