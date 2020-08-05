#ifndef COMPILER_ARMREGMANAGER_H
#define COMPILER_ARMREGMANAGER_H

#include <string>
#include <vector>

#include "../semantic/SymbolTable.h"
#include "ArmStmt.h"

/// 某一寄存器
class ArmReg;

/// Arm寄存器分配管理类
class ArmRegManager {
private:
    std::vector<ArmReg *> *armRegs;

public:
    ArmRegManager();

    static int newestNum;

    ArmReg *getFreeArmReg(std::vector<ArmStmt *> *armStmts);

    ArmReg *getArmRegByArmVar(Arm7Var *arm7Var, std::vector<ArmStmt *> *armStmts);

    ArmReg *getArmRegByNamePos(const char *name, int memoryLoc, std::vector<ArmStmt *> *armStmts);

    ArmReg *getArmRegByLVal(LVal *lVal, std::vector<ArmStmt *> *armStmts);

    void freeAllArmReg(std::vector<ArmStmt *> *ArmStmts);

    ArmReg *getOneArmReg(int i, std::vector<ArmStmt *> *ArmStmts);

    void freeOneArmReg(int i, std::vector<ArmStmt *> *ArmStmts);

    void pushOneArmReg(int i, std::vector<ArmStmt *> *ArmStmts);

    void popOneArmReg(int i, std::vector<ArmStmt *> *ArmStmts);

    void pushAllArmReg(std::vector<ArmStmt *> *ArmStmts);

    void popAllArmReg(std::vector<ArmStmt *> *ArmStmts);

    //    static int getNewestNum();
//
//    static void setNewestNum(int newestNum_);
//
//    static void addNewestNum();
//
    [[nodiscard]] std::vector<ArmReg *> *getArmRegs() const;
//
//    void setArmRegs(std::vector<ArmReg *> *armRegs);

};

class ArmReg {
private:
    std::string regName;

    Arm7Var *arm7Var;

    int newNum;
    /// 仅在已有某个寄存器存了二元运算临时结果，需要一个新寄存器时有效。同一时刻有且只有可能一个寄存器 ifLock 为真
    /// 0804更新，函数传参时，最多要使用四个寄存器，则存在锁三个寄存器的情况
    int ifLock;

public:
    ArmReg(const char *regName, Arm7Var *arm7Var, int newNum) :
            regName(regName), arm7Var(arm7Var), newNum(newNum), ifLock(ARM_REG_LOCK_FALSE) {};

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

    [[nodiscard]] int getIfLock() const {
        return ifLock;
    }

    void setIfLock(int ifLock_) {
        ArmReg::ifLock = ifLock_;
    }
};

#endif //COMPILER_ARMREGMANAGER_H
