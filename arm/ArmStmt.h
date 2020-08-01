#ifndef COMPILER_ARMSTMT_H
#define COMPILER_ARMSTMT_H

#include <string>
#include <vector>

#include "../util/MyConstants.h"

class ArmStmt {
private:
    int opType;

    std::string opd1;

    std::string opd2;

    std::string opd3;

    int level;

public:
    ArmStmt(int opType, const char *opd1, int level) :
            opType(opType), opd1(opd1), opd2(OPD_NULL), opd3(OPD_NULL), level(level) {};

    ArmStmt(int opType, const char *opd1, const char *opd2, int level) :
            opType(opType), opd1(opd1), opd2(opd2), opd3(OPD_NULL), level(level) {};

    ArmStmt(int opType, const char *opd1, const char *opd2, const char *opd3, int level) :
            opType(opType), opd1(opd1), opd2(opd2), opd3(opd3), level(level) {};

    [[nodiscard]] int getOpType() const {
        return opType;
    }

    [[nodiscard]] const std::string &getOpd1() const {
        return opd1;
    }

    [[nodiscard]] const std::string &getOpd2() const {
        return opd2;
    }

    [[nodiscard]] const std::string &getOpd3() const {
        return opd3;
    }

    [[nodiscard]] int getLevel() const {
        return level;
    }
};

class ArmBlock{
private:
    std::string blockName;

    std::vector<ArmStmt *> *armStmts;

public:
    /**
     *
     * @param blockName L"DIGIT" 表代码块名
     * @param armStmts atm语句
     */
    ArmBlock(const char *blockName, std::vector<ArmStmt *> *armStmts):
            blockName(blockName), armStmts(armStmts){};

    [[nodiscard]] const std::string &getBlockName() const {
        return blockName;
    }

    [[nodiscard]] std::vector<ArmStmt *> *getArmStmts() const {
        return armStmts;
    }

};

#endif //COMPILER_ARMSTMT_H
