#ifndef COMPILER_IRLOCALDAG_H
#define COMPILER_IRLOCALDAG_H

#include <string>
#include <utility>
#include <vector>
#include <set>
#include <queue>
#include "../semantic/SymbolTable.h"
#include "../util/MyConstants.h"


class DAGNode;
class DAGValue;
class DAGUse;
class DAGRoot;
class DAG;


class DAGValue {
private:
  DAGNode *Node = nullptr; // The node defining the value we are using.
  unsigned ResNo = 0;      // Which return value of the node we are using.
public:
  DAGValue() = default;

  DAGValue(DAGNode *node, unsigned resno);

  /// get the index which selects a specific result in the DAGNode
  unsigned getResNo() const { return ResNo; }

  /// get the DAGNode which holds the desired result
  DAGNode *getNode() const { return Node; }

  /// set the DAGNode
  void setNode(DAGNode *N) { Node = N; }

  DAGValue getValue(unsigned R) const {
    return DAGValue(Node, R);
  }
};

class DAGUse {
private:
  /// Val - The value being used.
  DAGValue Val;
  /// User - The user of this value.
  DAGNode *User = nullptr;
  /// Prev, Next - Pointers to the uses list of the DAGNode referred by
  /// this operand.
  DAGUse **Prev = nullptr;
  DAGUse *Next = nullptr;

public:
  DAGUse() = default;

  DAGUse(const DAGValue &V, DAGNode *p) {
    setInitial(V);
    setUser(p);
  }

  /// If implicit conversion to DAGValue doesn't work, the get() method returns
  /// the DAGValue.
  const DAGValue &get() const { return Val; }

  /// This returns the DAGNode that contains this Use.
  DAGNode *getUser() { return User; }

  /// Get the next DAGUse in the use list.
  DAGUse *getNext() const { return Next; }

  /// Convenience function for get().getNode().
  DAGNode *getNode() const { return Val.getNode(); }

  /// Convenience function for get().getResNo().
  unsigned getResNo() const { return Val.getResNo(); }

private:
  friend class DAGRoot;

  friend class DAG;

  friend class DAGNode;

  void setUser(DAGNode *p) { User = p; }

  inline void set(const DAGValue &V);

  inline void setInitial(const DAGValue &V);

  inline void setNode(DAGNode *N);

  void addToList(DAGUse **List) {
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

class DAGNode {
private:
  // 唯一标识
  int id;
  // 操作类型
  int opType;
  // 指定节点输出名称
  std::string retName;
  // 指定输入的DAGUse
  std::vector<DAGUse *> OperandList;
  // 用到自身的DAGUse的头结点
  DAGUse *UseList = nullptr;

public:

  DAGNode(int id, int opType, const std::string &retName) : id(id), opType(opType), retName(retName) {}

  /// 这个方法只允许在DAGUse中使用
  void addUse(DAGUse &U) { U.addToList(&UseList); }

  /**
   * get函数
   */
public:
  int getOpType() const {
    return opType;
  }

  const std::string &getRetName() const {
    return retName;
  }

  std::vector<DAGUse *> getOperandList() const {
    return OperandList;
  }

  DAGUse *getUseList() const {
    return UseList;
  }

  int getID() const{
      return id;
  }


  /**
   * set函数
   */
public:
  void setOpType(int opType) {
    DAGNode::opType = opType;
  }

  void setRetName(const std::string &retName) {
    DAGNode::retName = retName;
  }

  void setOperandList(std::vector<DAGUse *> operandList) {
    OperandList = operandList;
  }
};

class ImmediateDAGNode : public DAGNode {
private:
  int value;
public:
  ImmediateDAGNode(int id, int value) : DAGNode(
          id, DAG_IMMEDIATE_DATA, "IMMEDIATE_DATA"), value(value) {}
};

class DAGRoot {
private:
  std::vector<DAGNode *> nodes;
  // 标记当前语句的数量
  int count = 0;
  // 已存在变量列表
  std::vector<std::string> existVarList;

  //所在的DAG
  DAG *dag;

private:
  /// 对node进行深度遍历,返回一个DAGNode，其retName与opd相等
  DAGNode *DepthDAGNode(DAGNode *dagNode, const char *opd) {
    for (auto &item:dagNode->getOperandList()) {
      if (item->get().getNode()->getRetName() == opd) {
        return item->get().getNode();
      } else {
        DAGNode *node = DepthDAGNode(item->get().getNode(), opd);
        if (node != nullptr)
          return node;
      }
    }
    return nullptr;
  }

  /// 寻找DAG中对应的指定targetRetName的node;
  /// 然后将此node作为newNode的输入;
  /// 如果newNodeRetName与targetRetName相同，则将指向此node的DAGUse全部改为指向newNode;
  void FindNode(DAGNode *newNode, const char *newNodeRetName, const char *targetRetName);

  /// 先判断opdName是否为临时变量
  ///   是临时变量：
  ///     不管existVarList存不存该变量在都返回true，不存在的话要将其添加到existVarList。
  ///   不是临时变量
  ///     existVarList存在该变量则直接返回true，不存在的话则为外部变量，先将其添加到existVarList，再返回false
  bool AnalysisOpdName(const char *opdName);

public:
  DAGRoot();

  void AddRet();

  void AddRet(std::string &opd1);

  void AddBr(std::string &opd1);

  /// 添加alloca节点
  void AddAlloca(std::string &opd1);

  void AddCon_Alloca(std::string &opd1);


  void AddGlobal(std::string &opd1);

  void AddCon_Global(std::string &opd1);

  /// 添加store节点
  void AddStore(std::string &opd1, std::string &opd2);

  /// 添加store节点, opd2为立即数
  void AddStore(std::string &opd1, int opd2);

  void AddLoad(std::string &opd1, std::string &opd2);

  void AddLoad(std::string &opd1, int opd2);

  void AddAlloca_Array(std::string &opd1, std::vector<std::string> dismensionList);

  void AddCon_Alloca_Array(std::string &opd1, std::vector<std::string> dismensionList);

  void AddGlobal_Array(std::string &opd1, std::vector<std::string> dismensionList);

  void AddCon_Global_Array(std::string &opd1, std::vector<std::string> dismensionList);


  /// 添加算术逻辑节点
  void AddBO(std::string &opd1, std::string &opd2, std::string &opd3, int opType);

  void AddUO(std::string &opd1, std::string &opd2, int opType);

  void AddBr(std::string &opd1, std::string &opd2, std::string &opd3);

  void AddGetPtr(std::string &opd1, std::string &opd2, std::string &opd3);

  void AddGetPtr(std::string &opd1, int opd2, std::string &opd3);

  void AddGetPtr(std::string &opd1, std::string &opd2, std::vector<std::string> &opd3);

  void AddCall(std::string &opd1, std::string &opd2, std::vector<std::string> &paramList);



};



class DAG{

    DAGRoot *Root;

    std::string Name;

    std::vector<DAGNode*> Nodes;

    std::queue<DAGNode*> TopoList;

public:

    friend class DAGNode;

    DAG(DAGRoot *root, const char *name)
            : Root(root), Name(name)  {};

    void setRoot(DAGRoot* dagRoot){
        Root = dagRoot;
    }

    DAGRoot *getRoot(){
        return Root;
    }

    std::string getName(){
        return Name;
    }

    void addNode(DAGNode *node){
        Nodes.push_back(node);
    }


    // Operations about the overall DAG are defined in this class

    void setTopoList();

    /// generate dot language file for graph
    void generateDOT();

};



#endif //COMPILER_IRLOCALDAG_H
