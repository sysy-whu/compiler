#ifndef COMPILER_ARMREGMANAGER_H
#define COMPILER_ARMREGMANAGER_H

#include <string>
#include <vector>

#include "../semantic/SymbolTable.h"

/// 某一寄存器
class ArmReg;

/// Arm寄存器分配管理类
class ArmRegManager {
private:
    std::vector<ArmReg *> *armRegs;

    static int newestNum;

public:
    ArmRegManager();

    ArmReg *getFreeArmReg();

    static int getNewestNum();

    static void setNewestNum(int newestNum_);

    static void addNewestNum();

};

class ArmReg {
private:
    std::string regName;

    Arm7Var *arm7Var;

    int newNum;

public:
    ArmReg(const char *regName, Arm7Var *arm7Var, int newNum) : regName(regName), arm7Var(arm7Var), newNum(newNum) {};

    [[nodiscard]] const std::string &getRegName() const {
        return regName;
    }

    [[nodiscard]] Arm7Var *getArm7Var() const {
        return arm7Var;
    }

    void setArm7Var(Arm7Var *arm7Var_) {
        ArmReg::arm7Var = arm7Var_;
    }

    [[nodiscard]] int getNewNum() const {
        return newNum;
    }

    void setNewNum(int newNum_) {
        ArmReg::newNum = newNum_;
    }
};

#endif //COMPILER_ARMREGMANAGER_H
