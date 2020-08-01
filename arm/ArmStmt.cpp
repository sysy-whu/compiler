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