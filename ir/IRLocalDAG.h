#ifndef COMPILER_IRLOCALDAG_H
#define COMPILER_IRLOCALDAG_H

#include <string>
#include <vector>
#include "../semantic/SymbolTable.h"

class DAGNode;

class DAGValue {
private:
    Symbol *symbol;

    std::string str;

public:
    /**
     *
     * @param symbol 变量
     * @param str 用于数组下标索引
     */
    DAGValue(Symbol *symbol, const char *str) : symbol(symbol), str(str) {};

    [[nodiscard]] Symbol *getSymbol() const {
        return symbol;
    }

    void setSymbol(Symbol *symbol_) {
        DAGValue::symbol = symbol_;
    }

    [[nodiscard]] const std::string &getStr() const {
        return str;
    }

    void setStr(const std::string &str_) {
        DAGValue::str = str_;
    }
};

class DAGDirection {
private:
    DAGNode *director{};

    DAGNode *target{};

public:
    DAGDirection() = default;;

    DAGDirection(DAGNode *director, DAGNode *target) : director(director), target(target) {};

    [[nodiscard]] DAGNode *getDirector() const {
        return director;
    }

    void setDirector(DAGNode *director_) {
        DAGDirection::director = director_;
    }

    [[nodiscard]] DAGNode *getTarget() const {
        return target;
    }

    void setTarget(DAGNode *target_) {
        DAGDirection::target = target_;
    }
};

class DAGNode {
private:
    int id;

    int opType;

    DAGNode *opData1;

    DAGNode *opData2;

    DAGNode *opData3;

    DAGValue *dagValue;

    std::vector<DAGDirection *> *directors;

    std::vector<DAGDirection *> *subGoals;

public:
    /**
     *
     * @param id 唯一标识
     * @param opType  操作类型
     * @param opData1 操作数，可为空
     * @param opData2 操作数，可为空
     * @param opData3 操作数，可为空
     * @param dagValue “叶子”节点 Value
     * @param directors 指向其的节点
     * @param subGoals 所指向的节点
     */
    DAGNode(int id, int opType, DAGNode *opData1, DAGNode *opData2, DAGNode *opData3, DAGValue *dagValue,
            std::vector<DAGDirection *> *directors, std::vector<DAGDirection *> *subGoals) :
            id(id), opType(opType), opData1(opData1), opData2(opData2), opData3(opData3), dagValue(dagValue),
            directors(directors), subGoals(subGoals) {};

};

class DAGRoot {
private:
    std::vector<DAGNode *> *nodes{};

public:
    DAGRoot() = default;;

    DAGRoot(std::vector<DAGNode *> *nodes) : nodes(nodes) {};
};

#endif //COMPILER_IRLOCALDAG_H
