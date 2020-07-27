
#ifndef COMPILER_ARMDAG_H
#define COMPILER_ARMDAG_H

#include <string>
#include <vector>
#include "../util/MyConstants.h"

class ArmDAG;
class ArmDAGNode;
class ArmDAGValue;
class ArmDAGRoot;

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

    // 对应IR DAG中结点id
    int DAGid;

    // 操作类型
    ARM_DAG opType;

    // arm指令操作数
    std::string opNum1;
    std::string opNum2;
    std::string opNum3;

    // 指定输入的ArmDAGUse
    std::vector<ArmDAGUse *> dependUseList;
    // 用到自身的ArmDAGUse的头结点
    ArmDAGUse *UseList = nullptr;

public:
    ArmDAGNode(int id, int DAGid, ARM_DAG opType) : id(id), DAGid(DAGid), opType(opType) {}

    ArmDAGNode(int id, int DAGid, ARM_DAG opType, std::string &opNum1) : id(id),DAGid(DAGid), opType(opType), opNum1(opNum1) {}

    ArmDAGNode(int id, int DAGid, ARM_DAG opType, std::string &opNum1, std::string &opNum2) : id(id),DAGid(DAGid), opType(opType),
                                                                                   opNum1(opNum1), opNum2(opNum2) {}
    ArmDAGNode(int id, int DAGid, ARM_DAG opType, std::string &opNum1, std::string &opNum2, std::string &opNum3) : id(id),
                                                                                                        DAGid(DAGid),
                                                                                                        opType(opType),
                                                                                                        opNum1(opNum1),
                                                                                                        opNum2(opNum2),
                                                                                                        opNum3(opNum3) {}

    /// armDagNode为依赖的node
    void addDependUse(ArmDAGNode *armDagNode) {
        ArmDAGValue armDagValue(armDagNode, 0);
        ArmDAGUse *armDagUse = new ArmDAGUse(armDagValue, this);
        dependUseList.emplace_back(armDagUse);
    };
    /// 这个方法只允许在ArmDAGUse中使用
    void addUse(ArmDAGUse &U) { U.addToList(&UseList); }

    /**
     * get函数
     */
public:
    ARM_DAG getOpType() const {
        return opType;
    }


    ArmDAGUse *getUseList() const {
        return UseList;
    }

    const std::string &getOpNum1() const {
        return opNum1;
    }

    const std::string &getOpNum2() const {
        return opNum2;
    }

    const std::string &getOpNum3() const {
        return opNum3;
    }

    /**
     * set函数
     */
public:
    void setOpType(ARM_DAG opType) {
        ArmDAGNode::opType = opType;
    }


    void setOpNum1(const std::string &opNum1) {
        ArmDAGNode::opNum1 = opNum1;
    }

    void setOpNum2(const std::string &opNum2) {
        ArmDAGNode::opNum2 = opNum2;
    }

    void setOpNum3(const std::string &opNum3) {
        ArmDAGNode::opNum3 = opNum3;
    }
};


class ArmDAGRoot {
private:
    std::vector<ArmDAGNode *> armDagNodes;

public:
    ArmDAGRoot() = default;

    void addNode(ArmDAGNode *armDagNode) {
        armDagNodes.emplace_back(armDagNode);
    }

};



class ArmDAG{

    ArmDAGRoot *Root;

    std::string Name;

    std::vector<ArmDAGNode*> Nodes;

public:

    friend class DAGNode;

    ArmDAG(ArmDAGRoot *root, const char *name)
            : Root(root), Name(name)  {};

    void setRoot(ArmDAGRoot* dagRoot){
        Root = dagRoot;
    }

    ArmDAGRoot *getRoot(){
        return Root;
    }

    std::string getName(){
        return Name;
    }

    void addNode(ArmDAGNode *node){
        Nodes.push_back(node);
    }

};



#endif //COMPILER_ARMDAG_H
