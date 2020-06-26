#ifndef COMPILER_IRLOCALBLOCK_H
#define COMPILER_IRLOCALBLOCK_H


#include <utility>
#include <vector>
#include <string>
#include <set>
#include "IRLocalStmt.h"

/**
 * IRLocalBlock 标签块类
 */
class IRLocalBlock {
private:
    std::vector<IRLocalStmt *> *localStmts;

    std::string labelName;

    unsigned int line;

    std::set<std::string> *fromLineNames;

public:
    /**
     * IRLocalBlock 标签块类构造方法
     * @param labelName 分配的标签名
     * @param localStmts 标签块中的语句
     * @Param line 行号
     * @param fromLabelNames 哪些标签块可以跳转到这里
     */
    IRLocalBlock(const char *labelName, std::vector<IRLocalStmt *> *localStmts, unsigned int line,
                 std::set<std::string> *fromLabelNames)
            : labelName(labelName), localStmts(localStmts), line(line), fromLineNames(fromLabelNames) {};

    std::string getLabelName() { return labelName; }

    // stmt语句代码
    std::vector<IRLocalStmt *> *getLocalStmts() { return localStmts; }

    void clearLocalStmts() { localStmts->clear(); }

    void modifyLocalStmts(std::vector<IRLocalStmt *> *newLocalStmts) {
        localStmts->clear();
        this->localStmts = newLocalStmts;
    }

    void modifyOneLocalStmtByPos(IRLocalStmt *localStmt, unsigned int pos) {
        localStmts->erase(localStmts->begin() + pos);
        localStmts->insert(localStmts->begin() + pos, localStmt);
    }

    void deleteOneLocalStmtByPos(unsigned int pos) {
        this->localStmts->erase(this->localStmts->begin() + pos);
    }

    // 行列号
    [[nodiscard]] unsigned int getLine() const { return line; }

    void setLine(unsigned int line_) { this->line = line_; }

    // 来源
    std::set<std::string> *getFromLabelNames() { return fromLineNames; }

    void addFromLabelNames(const std::string &oneLabelName) { fromLineNames->insert(oneLabelName); }

    void deleteFromLabelNames(const std::string &oneLabelName) { fromLineNames->erase(oneLabelName); }

    void clearFromLabelNames() { fromLineNames->clear(); }
};


#endif //COMPILER_IRLOCALBLOCK_H
