#ifndef COMPILER_IRGLOBAL_H
#define COMPILER_IRGLOBAL_H

#include <utility>

#include "IRLocalLabel.h"
#include "IRLocalStmt.h"

class IRGlobal {

};

/**
 * 全局变量类IRGlobalVar
 */
class IRGlobalVar : public IRGlobal {
private:
    IRLocalStmt irLocalStmt;

public:
    /**
     * 全局变量类IRGlobalVar构造方法
     * @param irLocalStmt 变量语句
     */
    IRGlobalVar(IRLocalStmt irLocalStmt) : irLocalStmt(std::move(irLocalStmt)) {};

    IRLocalStmt *getIrLocalStmt() { return &irLocalStmt; }

    void setIrLocalStmt(IRLocalStmt irLocalStmt) { this->irLocalStmt = std::move(irLocalStmt); }

};

/**
 * 函数类IRGlobalFunc
 */
class IRGlobalFunc : public IRGlobal {
private:
    std::string funcName;

    std::string retType;

    std::vector<IRLocalLabel> labels;

    std::set<std::string> fromLineNames;

    unsigned int line;
    unsigned int row;
    unsigned int col;

public:
    /**
     * 函数类IRGlobalFunc构造方法
     * @param funcName 函数名
     * @param retType 返回类型
     * @param labels 函数内部IR代码块
     * @param fromLineNames 哪些行调用了这个函数
     * @param line 行号所在
     * @param row 源代码位置行号
     * @param col 源代码位置列号
     */
    IRGlobalFunc(std::string funcName, std::string retType, std::vector<IRLocalLabel> labels,
                 std::set<std::string> fromLineNames, unsigned int line, unsigned int row, unsigned int col) :
            funcName(std::move(funcName)), retType(std::move(retType)), labels(std::move(labels)),
            fromLineNames(std::move(fromLineNames)), line(line), row(row), col(col) {};

    std::string getFuncName() { return funcName; }

    std::string getRetType() { return retType; }

    // label代码块
    std::vector<IRLocalLabel> *getLabels() { return &labels; }

    void setLabels(std::vector<IRLocalLabel> labels) { this->labels = labels; }

    void modifyOneLabelByPos(IRLocalLabel localLabel, unsigned int pos) {
        IRLocalLabel &irLocalLabel = this->labels.at(pos);
        irLocalLabel = localLabel;
    }

    // 来源
    std::set<std::string> getFromLabelNames() { return fromLineNames; }

    void addFromLabelNames(const std::string &oneLabelName) { fromLineNames.insert(oneLabelName); }

    void deleteFromLabelNames(const std::string &oneLabelName) { fromLineNames.erase(oneLabelName); }

    void clearFromLabelNames() { fromLineNames.clear(); }


    // 行列号
    unsigned int getLine() const { return line; }

    void setLine(unsigned int line) { this->line = line; }

    unsigned int getRow() const { return row; }

    unsigned int getColumn() const { return col; }
};


#endif //COMPILER_IRGLOBAL_H
