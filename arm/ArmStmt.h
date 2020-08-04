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

public:
    ArmStmt(int opType, const char *opd1) :
            opType(opType), opd1(opd1), opd2(OPD_NULL), opd3(OPD_NULL) {};

    ArmStmt(int opType, const char *opd1, const char *opd2) :
            opType(opType), opd1(opd1), opd2(opd2), opd3(OPD_NULL) {};

    ArmStmt(int opType, const char *opd1, const char *opd2, const char *opd3) :
            opType(opType), opd1(opd1), opd2(opd2), opd3(opd3) {};

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

private:
    std::string opType2String() {
        // todo type转相应str
        switch (opType) {
            case ARM_STMT_STR:
                return "str";
            case ARM_STMT_LDR:
                return "ldr";
            case ARM_STMT_PUSH:
                return "push";
            case ARM_STMT_POP:
                return "pop";
            case ARM_STMT_ADD:
                return "add";
            case ARM_STMT_SUB:
                return "sub";
            case ARM_STMT_MUL:
                return "mul";
            case ARM_STMT_RSB:
                return "rsb";
            case ARM_STMT_MOV:
                return "mov";
            case ARM_STMT_MOVEQ:
                return "moveq";
            case ARM_STMT_MOVNE:
                return "movne";
            case ARM_STMT_MOVLE:
                return "movle";
            case ARM_STMT_MOVGE:
                return "movge";
            case ARM_STMT_MOVGT:
                return "movgt";
            case ARM_STMT_MOVLT:
                return "movlt";
            case ARM_STMT_MOVW:
                return "movw";
            case ARM_STMT_MOVT:
                return "movt";
            case ARM_STMT_BL:
                return "bl";
            case ARM_STMT_B:
                return "b";
            case ARM_STMT_CMP:
                return "cmp";
            case ARM_STMT_BEQ:
                return "beq";
            case ARM_STMT_BNE:
                return "bne";
        }
        return "";
    }

public:
    std::string genString() {
        if (opd2.empty() && opd3.empty()) {
            return opType2String() + " " + opd1 + "\n";
        } else if (opd3.empty()) {
            return opType2String() + " " + opd1 + "," + opd2 + "\n";
        } else {
            return opType2String() + " " + opd1 + "," + opd2 + "," + opd3 + "\n";
        }
    }
};

class ArmBlock {
private:
    std::string blockName;

    std::vector<ArmStmt *> *armStmts;

public:
    /**
     *
     * @param blockName L"DIGIT" 表代码块名
     * @param armStmts atm语句
     */
    ArmBlock(const char *blockName, std::vector<ArmStmt *> *armStmts) :
            blockName(blockName), armStmts(armStmts) {};

    [[nodiscard]] const std::string &getBlockName() const {
        return blockName;
    }

    [[nodiscard]] std::vector<ArmStmt *> *getArmStmts() const {
        return armStmts;
    }

    /// ArmBlock输出
    std::string genString() {
        std::string re = "";
        if (blockName != "entry") {
            re = blockName + ":\n";
        }
        for (auto armStmt:*armStmts) {
            re += "\t"+armStmt->genString();
        }
        return re;
    }

};

#endif //COMPILER_ARMSTMT_H
