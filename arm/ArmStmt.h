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
private:
    std::string opType2String() {
//        switch (opType) {
//            case ARM_TYPE::MOV:
//                return "MOV";
//            case ARM_TYPE::MOVEQ:
//                return "MOVEQ";
//            case ARM_TYPE::MOVNE:
//                return "MOVNE";
//            case ARM_TYPE::MOVGE:
//                return "MOVGE";
//            case ARM_TYPE::MOVGT:
//                return "MOVGT";
//            case ARM_TYPE::MOVLT:
//                return "MOVLT";
//            case ARM_TYPE::MOVW:
//                return "MOVW";
//            case ARM_TYPE::MOVT:
//                return "MOVT";
//            case ARM_TYPE::ADD:
//                return "ADD";
//            case ARM_TYPE::SUB:
//                return "SUB";
//            case ARM_TYPE::MUL:
//                return "MUL";
//            case ARM_TYPE::DIV:
//                return "DIV";
//            case ARM_TYPE::MOD:
//                return "MOD";
//            case ARM_TYPE::CMP:
//                return "CMP";
//            case ARM_TYPE::LDR:
//                return "LDR";
//            case ARM_TYPE::STR:
//                return "STR";
//            case ARM_TYPE::B:
//                return "B";
//            case ARM_TYPE::BL:
//                return "BL";
//            case ARM_TYPE::BX:
//                return "BX";
//            case ARM_TYPE::BEQ:
//                return "BEQ";
//            case ARM_TYPE::PUSH:
//                return "PUSH";
//            case ARM_TYPE::POP:
//                return "POP";
//        }
        return "";
    }

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

    std::string genString() {
        return opType2String() + " " + opd1 + " " + opd2 + " " + opd3 + "\n";
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

    std::string genString() {
        std::string re = "";
        if (blockName != "entry") {
            re = blockName + ":\n";
        }
        for (auto armStmt:*armStmts) {
            re += armStmt->genString();
        }
        return re;
    }
};

#endif //COMPILER_ARMSTMT_H
