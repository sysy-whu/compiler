#include "ArmRegManager.h"
#include "ArmStmt.h"

int ArmRegManager::newestNum = 0;

ArmRegManager::ArmRegManager() {
    ArmRegManager::armRegs = new std::vector<ArmReg *>();
    for (int i = 0; i < 10; i++) {
        auto *armReg = new ArmReg(("R" + std::to_string(i)).c_str(), nullptr, 0);
        armRegs->emplace_back(armReg);
    }
}

/// 按照 最先分配最先释放 来分配新寄存器
ArmReg *ArmRegManager::getFreeArmReg(std::vector<ArmStmt *> *armStmts) {
    ArmReg *armRegRet = armRegs->at(0);

    for (auto &armRegI : *ArmRegManager::armRegs) {
        if (armRegI->getArm7Var() == nullptr && armRegI->getIfLock() == ARM_REG_LOCK_FALSE) {
            return armRegI;
        } else {
            if (armRegI->getIfLock() == ARM_REG_LOCK_FALSE) {
                /// if(armRegRet Locked) return armRegI; else if(armRegRet numBigger) return armRegI;
                armRegRet = armRegRet->getIfLock() == ARM_REG_LOCK_TRUE ? armRegI :
                            armRegI->getNewNum() < armRegRet->getNewNum() ? armRegI : armRegRet;
            }
        }
    }

    if (armRegRet->getArm7Var() != nullptr) {
        auto *armSTRStmt =
                new ArmStmt(ARM_STMT_STR, armRegRet->getRegName().c_str(),
                            ("[fp, #" + std::to_string(armRegRet->getArm7Var()->getMemoryLoc()) + "]").c_str());
        armStmts->emplace_back(armSTRStmt);
        armRegRet->setArm7Var(nullptr);
    }
    return armRegRet;
}


int ArmRegManager::getNewestNum() {
    return newestNum;
}

void ArmRegManager::setNewestNum(int newestNum_) {
    ArmRegManager::newestNum = newestNum_;
}

void ArmRegManager::addNewestNum() {
    ArmRegManager::newestNum += 1;
}

std::vector<ArmReg *> *ArmRegManager::getArmRegs() const {
    return armRegs;
}

void ArmRegManager::setArmRegs(std::vector<ArmReg *> *armRegs_) {
    ArmRegManager::armRegs = armRegs_;
}

void ArmRegManager::freeAllArmReg(std::vector<ArmStmt *> *ArmStmts) {
    for (auto *armReg: *armRegs) {
        if (armReg->getArm7Var() != nullptr) {
            auto *armSTRStmt =
                    new ArmStmt(ARM_STMT_STR, armReg->getRegName().c_str(),
                                ("[fp, #" + std::to_string(armReg->getArm7Var()->getMemoryLoc()) + "]").c_str());
            ArmStmts->emplace_back(armSTRStmt);
            armReg->setArm7Var(nullptr);
        }
    }
}

void ArmRegManager::pushOneArmReg(int i, std::vector<ArmStmt *> *ArmStmts) {
    if (armRegs->at(i)->getArm7Var() != nullptr) {
        /// push {rI }
        auto *pushRIStmt = new ArmStmt(ARM_STMT_PUSH, ("{r" + std::to_string(i) + " }").c_str());
        ArmStmts->emplace_back(pushRIStmt);
    }
}

void ArmRegManager::popOneArmReg(int i, std::vector<ArmStmt *> *ArmStmts) {
    if (armRegs->at(i)->getArm7Var() != nullptr) {
        /// pop {rI }
        auto *popRIStmt = new ArmStmt(ARM_STMT_POP, ("{r" + std::to_string(i) + " }").c_str());
        ArmStmts->emplace_back(popRIStmt);
    }
}

void ArmRegManager::freeOneArmReg(int i, std::vector<ArmStmt *> *ArmStmts) {
    if (armRegs->at(i)->getArm7Var() != nullptr) {
        auto *armSTRStmt =
                new ArmStmt(ARM_STMT_STR, armRegs->at(i)->getRegName().c_str(),
                            ("[fp, #" + std::to_string(armRegs->at(i)->getArm7Var()->getMemoryLoc()) + "]").c_str());
        ArmStmts->emplace_back(armSTRStmt);
        armRegs->at(i)->setArm7Var(nullptr);
    }
}
