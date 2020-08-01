//
// Created by 77350 on 2020/8/1.
//

#include "ArmStmt.h"

ArmStmt::ArmStmt(ARM_TYPE opType, const std::string &opNum1, const std::string &opNum2, const std::string &opNum3)
        : opType(opType), opNum1(opNum1), opNum2(opNum2), opNum3(opNum3) {}

ArmStmt::ArmStmt(ARM_TYPE opType, const std::string &opNum1, const std::string &opNum2) : opType(opType),
                                                                                          opNum1(opNum1),
                                                                                          opNum2(opNum2) {}

ArmStmt::ArmStmt(ARM_TYPE opType, const std::string &opNum1) : opType(opType), opNum1(opNum1) {}

ArmStmt::ArmStmt(ARM_TYPE opType) : opType(opType) {}

std::string ArmStmt::opType2String() {
    switch (opType) {
        case ARM_TYPE::MOV:
            return "MOV";
        case ARM_TYPE::MOVEQ:
            return "MOVEQ";
        case ARM_TYPE::MOVNE:
            return "MOVNE";
        case ARM_TYPE::MOVGE:
            return "MOVGE";
        case ARM_TYPE::MOVGT:
            return "MOVGT";
        case ARM_TYPE::MOVLT:
            return "MOVLT";
        case ARM_TYPE::MOVW:
            return "MOVW";
        case ARM_TYPE::MOVT:
            return "MOVT";
        case ARM_TYPE::ADD:
            return "ADD";
        case ARM_TYPE::SUB:
            return "SUB";
        case ARM_TYPE::MUL:
            return "MUL";
        case ARM_TYPE::DIV:
            return "DIV";
        case ARM_TYPE::MOD:
            return "MOD";
        case ARM_TYPE::CMP:
            return "CMP";
        case ARM_TYPE::LDR:
            return "LDR";
        case ARM_TYPE::STR:
            return "STR";
        case ARM_TYPE::B:
            return "B";
        case ARM_TYPE::BL:
            return "BL";
        case ARM_TYPE::BX:
            return "BX";
        case ARM_TYPE::BEQ:
            return "BEQ";
        case ARM_TYPE::PUSH:
            return "PUSH";
        case ARM_TYPE::POP:
            return "POP";
    }
}

ARM_TYPE ArmStmt::getOpType() const {
    return opType;
}

const std::string &ArmStmt::getOpNum1() const {
    return opNum1;
}

const std::string &ArmStmt::getOpNum2() const {
    return opNum2;
}

const std::string &ArmStmt::getOpNum3() const {
    return opNum3;
}

void ArmStmt::setOpType(ARM_TYPE opType) {
    ArmStmt::opType = opType;
}

void ArmStmt::setOpNum1(const std::string &opNum1) {
    ArmStmt::opNum1 = opNum1;
}

void ArmStmt::setOpNum2(const std::string &opNum2) {
    ArmStmt::opNum2 = opNum2;
}

void ArmStmt::setOpNum3(const std::string &opNum3) {
    ArmStmt::opNum3 = opNum3;
}

std::string ArmStmt::genString() {
    return opType2String() + " " + opNum1 + " " + opNum2 + " " + opNum3+"\n";
}

