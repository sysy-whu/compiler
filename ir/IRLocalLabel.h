#ifndef COMPILER_IRLOCALLABEL_H
#define COMPILER_IRLOCALLABEL_H


#include <utility>
#include <vector>
#include <string>
#include <set>
#include "IRLocalStmt.h"

/**
 * IRLocalLabel标签块类
 */
class IRLocalLabel {
private:
    std::vector<IRLocalStmt> localStmts;

    std::string labelName;

    unsigned int line;

    std::set<std::string> fromLineNames;

public:
    /**
     * label标签块类构造方法
     * @param labelName 分配的标签名
     * @param localStmts 标签块中的语句
     * @Param line 行号
     * @param fromLabelNames 哪些标签块可以跳转到这里
     */
    IRLocalLabel(std::string labelName, std::vector<IRLocalStmt> localStmts, unsigned int line,
                 std::set<std::string> fromLabelNames)
            : labelName(std::move(labelName)), localStmts(std::move(localStmts)), line(line),
              fromLineNames(std::move(fromLabelNames)) {};

    std::string getLabelName() { return labelName; }

    // stmt语句代码
    std::vector<IRLocalStmt> *getLocalStmts() { return &localStmts; }

    void clearLocalStmts() { localStmts.clear(); }

    void modifyLocalStmts(std::vector<IRLocalStmt> localStmts) {
        localStmts.clear();
        this->localStmts = localStmts;
    }

    void modifyOneLocalStmtByPos(IRLocalStmt localStmt, unsigned int pos) {
        IRLocalStmt &stmt = this->localStmts.at(pos);
        stmt = std::move(localStmt);
    }

    void deleteOneLocalStmtByPos(unsigned int pos) {
        this->localStmts.erase(this->localStmts.begin() + pos);
    }

    // 行列号
    unsigned int getLine() const { return line; }

    void setLine(unsigned int line) { this->line = line; }

    // 来源
    std::set<std::string> getFromLabelNames() { return fromLineNames; }

    void addFromLabelNames(const std::string &oneLabelName) { fromLineNames.insert(oneLabelName); }

    void deleteFromLabelNames(const std::string &oneLabelName) { fromLineNames.erase(oneLabelName); }

    void clearFromLabelNames() { fromLineNames.clear(); }
};


#endif //COMPILER_IRLOCALLABEL_H
