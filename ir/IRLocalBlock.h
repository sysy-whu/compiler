#ifndef COMPILER_IRLOCALBLOCK_H
#define COMPILER_IRLOCALBLOCK_H
/**
 * 详细说明见 SysY2020-WHU_中间码设计.docx
 */
#include <utility>
#include <vector>
#include <string>
#include <set>

#include "IRStmt.h"
#include "IRLocalDAG.h"
#include "ArmDag.h"

/**
 * IRLocalBlock 标签块类
 */
class IRLocalBlock {
private:
    std::string blockName;

    std::vector<IRStmt *> *stmts;

    std::set<std::string> *preBlocks;

    DAGRoot *dagRoot = nullptr;

public:
    /**
     * IRLocalBlock 标签块类 构造方法
     * @param blockName 块名
     * @param stmts 块内有序语句
     * @param preBlocks 哪些块会跳转于此
     */
    IRLocalBlock(const char *blockName, std::vector<IRStmt *> *stmts, std::set<std::string> *preBlocks)
            : blockName(blockName), stmts(stmts), preBlocks(preBlocks) {};

    const std::string &getBlockName() const {
        return blockName;
    }

    std::vector<IRStmt *> *getStmts() const {
        return stmts;
    }

    void setStmts(std::vector<IRStmt *> *stmts_) {
        IRLocalBlock::stmts = stmts_;
    }

    void addStmt(IRStmt *irStmt) {
        IRLocalBlock::stmts->push_back(irStmt);
    }

    std::set<std::string> *getPreBlocks() const {
        return preBlocks;
    }

    void addPreBlock(std::string preBlockName) {
        IRLocalBlock::preBlocks->insert(preBlockName);
    }

    DAGRoot *getDagRoot() {
        return dagRoot;
    }

    void setDagRoot(DAGRoot *dagRoot_) {
        IRLocalBlock::dagRoot = dagRoot_;
    }
};

class ArmBlock {
private:
    std::string blockName;

    std::set<std::string> *preBlocks;

    ArmDAGRoot *armDagRoot = nullptr;

public:
    /**
     * IRLocalBlock 标签块类 构造方法
     * @param blockName 块名
     * @param preBlocks 哪些块会跳转于此
     */
    ArmBlock(const char *blockName, ArmDAGRoot *armDagRoot) : blockName(blockName), armDagRoot(armDagRoot) {};

    const std::string &getBlockName() const {
        return blockName;
    }

    std::set<std::string> *getPreBlocks() const {
        return preBlocks;
    }

    void addPreBlock(std::string preBlockName) {
        ArmBlock::preBlocks->insert(preBlockName);
    }

    ArmDAGRoot *getArmDagRoot() {
        return armDagRoot;
    }

    void setArmDagRoot(ArmDAGRoot *armDagRoot) {
        ArmBlock::armDagRoot = armDagRoot;
    }
};


#endif //COMPILER_IRLOCALBLOCK_H
