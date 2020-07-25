
#ifndef COMPILER_ARMDAG_H
#define COMPILER_ARMDAG_H

#include <string>
#include <vector>
#include "../util/MyConstants.h"

class ArmDAGNode;

class ArmDAGValue {
private:
  ArmDAGNode *Node = nullptr;
  unsigned ResNo = 0;
public:
  ArmDAGValue() = default;

  ArmDAGValue(ArmDAGNode *node, unsigned resno);

  /// get the index which selects a specific result in the ArmDAGNode
  unsigned getResNo() const { return ResNo; }

  /// get the ArmDAGNode which holds the desired result
  ArmDAGNode *getNode() const { return Node; }

  /// set the ArmDAGNode
  void setNode(ArmDAGNode *N) { Node = N; }

  ArmDAGValue getValue(unsigned R) const {
    return ArmDAGValue(Node, R);
  }
};

class ArmDAGUse {
private:
  /// Val - The value being used.
  ArmDAGValue Val;
  /// User - The user of this value.
  ArmDAGNode *User = nullptr;
  /// Prev, Next - Pointers to the uses list of the ArmDAGNode referred by
  /// this operand.
  ArmDAGUse **Prev = nullptr;
  ArmDAGUse *Next = nullptr;

public:
  ArmDAGUse() = default;

  ArmDAGUse(const ArmDAGValue &V, ArmDAGNode *p) {
    setInitial(V);
    setUser(p);
  }

  /// If implicit conversion to ArmDAGValue doesn't work, the get() method returns
  /// the ArmDAGValue.
  const ArmDAGValue &get() const { return Val; }

  /// This returns the ArmDAGNode that contains this Use.
  ArmDAGNode *getUser() { return User; }

  /// Get the next ArmDAGUse in the use list.
  ArmDAGUse *getNext() const { return Next; }

  /// Convenience function for get().getNode().
  ArmDAGNode *getNode() const { return Val.getNode(); }

  /// Convenience function for get().getResNo().
  unsigned getResNo() const { return Val.getResNo(); }

private:
  friend class ArmDAGRoot;

  friend class ArmDAGNode;

  void setUser(ArmDAGNode *p) { User = p; }

  inline void set(const ArmDAGValue &V);

  inline void setInitial(const ArmDAGValue &V);

  inline void setNode(ArmDAGNode *N);

  void addToList(ArmDAGUse **List) {
    Next = *List;
    if (Next)
      Next->Prev = &Next;
    Prev = List;
    *List = this;
  }

  void removeFromList() {
    *Prev = Next;
    if (Next)
      Next->Prev = Prev;
  }
};

class ArmDAGNode {
private:
  // 唯一标识
  int id;
  // 操作类型
  ARM_DAG opType;
  // 指定节点输出名称
  std::string retName;
  // 指定输入的ArmDAGUse
  std::vector<ArmDAGUse *> OperandList;
  // 用到自身的ArmDAGUse的头结点
  ArmDAGUse *UseList = nullptr;

public:
  ArmDAGNode(int id, ARM_DAG opType, const std::string retName) : id(id), opType(opType), retName(retName) {}

  /// 这个方法只允许在ArmDAGUse中使用
  void addUse(ArmDAGUse &U) { U.addToList(&UseList); }

  /**
   * get函数
   */
public:
  ARM_DAG getOpType() const {
    return opType;
  }

  const std::string &getRetName() const {
    return retName;
  }

  std::vector<ArmDAGUse *> getOperandList() const {
    return OperandList;
  }

  ArmDAGUse *getUseList() const {
    return UseList;
  }

  /**
   * set函数
   */
public:
  void setOpType(ARM_DAG opType) {
    ArmDAGNode::opType = opType;
  }

  void setRetName(const std::string &retName) {
    ArmDAGNode::retName = retName;
  }

  void setOperandList(std::vector<ArmDAGUse *> operandList) {
    OperandList = operandList;
  }
};

class ImmediateArmDAGNode : public ArmDAGNode {
private:
  int value;

public:
  ImmediateArmDAGNode(int id, int value) : ArmDAGNode(
          id, ARM_DAG::IMMEDIATE_DATA, "IMMEDIATE_DATA"),
                                           value(value) {}
};

class ArmDAGRoot {
private:
  std::vector<ArmDAGNode *> nodes;
  // 标记当前语句的数量
  int count = 0;
  // 已存在变量列表
  std::vector<std::string> existVarList;

private:
  /// 对node进行深度遍历,返回一个ArmDAGNode，其retName与opd相等
  ArmDAGNode *DepthArmDAGNode(ArmDAGNode *armDAGNode, const char *opd) {
    for (auto &item : armDAGNode->getOperandList()) {
      if (item->get().getNode()->getRetName() == opd) {
        return item->get().getNode();
      } else {
        ArmDAGNode *node = DepthArmDAGNode(item->get().getNode(), opd);
        if (node != nullptr)
          return node;
      }
    }
    return nullptr;
  }

  /// 寻找DAG中对应的指定targetRetName的node;
  /// 然后将此node作为newNode的输入;
  /// 如果newNodeRetName与targetRetName相同，则将指向此node的ArmDAGUse全部改为指向newNode;
  void FindNode(ArmDAGNode *newNode, const char *newNodeRetName, const char *targetRetName);

  /// 先判断opdName是否为临时变量
  ///   是临时变量：
  ///     不管existVarList存不存该变量在都返回true，不存在的话要将其添加到existVarList。
  ///   不是临时变量
  ///     existVarList存在该变量则直接返回true，不存在的话则为外部变量，先将其添加到existVarList，再返回false
  bool AnalysisOpdName(const char *opdName);

public:
  ArmDAGRoot() = default;
  // ArmDAGRoot(std::vector<ArmDAGNode *> *nodes) : nodes(nodes){};

};

#endif //COMPILER_ARMDAG_H
