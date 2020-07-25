#include "ArmDag.h"
#include "../util/MyConstants.h"

inline ArmDAGValue::ArmDAGValue(ArmDAGNode *node, unsigned resno)
        : Node(node), ResNo(resno) {}

inline void ArmDAGUse::set(const ArmDAGValue &V) {
  if (Val.getNode())
    removeFromList();
  Val = V;
  if (V.getNode())
    V.getNode()->addUse(*this);
}


inline void ArmDAGUse::setInitial(const ArmDAGValue &V) {
  Val = V;
  V.getNode()->addUse(*this);
}

inline void ArmDAGUse::setNode(ArmDAGNode *N) {
  if (Val.getNode())
    removeFromList();
  Val.setNode(N);
  if (N)
    N->addUse(*this);
}


void ArmDAGRoot::FindNode(ArmDAGNode *newNode, const char *newNodeRetName, const char *targetRetName) {
  std::vector<ArmDAGUse *> OperandList = newNode->getOperandList();
  // 分析该targetRetName在DAG中的情况
  bool isInternalVal = AnalysisOpdName(targetRetName);

  if (isInternalVal) {
    bool isNotOnRoot = true;
    for (auto item = nodes.end() - 1; item >= nodes.begin(); item--) {
      // ArmDAGRoot存在目标node，
      if ((*item)->getRetName() == targetRetName) {
        isNotOnRoot = false;
        auto *ArmDAGUse1 = new ArmDAGUse(ArmDAGValue(*item, 0), newNode);
        OperandList.push_back(ArmDAGUse1);
        //  删除该节点与根节点的联系

        nodes.erase(item);
        break;
      }
      if (item == nodes.begin())
        break;
    }
    // ArmDAGRoot不存在目标node，开始深度遍历寻找

    if (isNotOnRoot) {

      for (auto item = nodes.rbegin(); item != nodes.rend(); item++) {
        ArmDAGNode *targetNode = DepthArmDAGNode(*item, targetRetName);
        if (targetNode != nullptr) {
          auto *ArmDAGUse1 = new ArmDAGUse(ArmDAGValue(targetNode, 0), newNode);
          OperandList.push_back(ArmDAGUse1);
        }
      }
    }
  } else {
    // 创建外部变量的node
    ArmDAGNode *externalNode = new ArmDAGNode(count++, DAG_EXTERNAL_DATA, targetRetName);
    ArmDAGUse *ArmDAGUse1 = new ArmDAGUse(ArmDAGValue(externalNode, 0), newNode);
    // 添加到OperandList中
    OperandList.push_back(ArmDAGUse1);
  }
  newNode->setOperandList(OperandList);
}


bool ArmDAGRoot::AnalysisOpdName(const char *opdName) {
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
