#include "ArmRegManager.h"
#include "ArmStmt.h"

int ArmRegManager::newestNum = 0;

ArmRegManager::ArmRegManager() {
    ArmRegManager::armRegs = new std::vector<ArmReg *>();
    for (int i = 0; i < 10; i++) {
        auto *armReg = new ArmReg(("r" + std::to_string(i)).c_str(), nullptr, 0);
        armRegs->emplace_back(armReg);
        armReg->setNewNum(++newestNum);
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
                if (armRegRet->getIfLock() == ARM_REG_LOCK_TRUE) {
                    armRegRet = armRegI;
                } else {
                    armRegRet = armRegI->getNewNum() < armRegRet->getNewNum() ? armRegI : armRegRet;
                }
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
    armRegRet->setNewNum(++newestNum);
    return armRegRet;
}

ArmReg *ArmRegManager::getArmRegByArmVar(Arm7Var *arm7Var, std::vector<ArmStmt *> *armStmts) {
//    for (auto *armReg:*armRegs) {
//        /// 这种比较方法确认
//        if (armReg->getArm7Var() == arm7Var)
//            return armReg;
//    }
    auto *armRegRet = getFreeArmReg(armStmts);
    auto *armLdr = new ArmStmt(ARM_STMT_LDR, armRegRet->getRegName().c_str(),
                               ("[fp, #" + std::to_string(arm7Var->getMemoryLoc()) + "]").c_str());
    armStmts->emplace_back(armLdr);
    return armRegRet;
}


ArmReg *ArmRegManager::getArmRegByNamePos(const char *name, int memoryLoc, std::vector<ArmStmt *> *armStmts) {
//    for (auto *armReg:*armRegs) {
//        /// 这种比较方法确认
//        if (armReg->getArm7Var() != nullptr && armReg->getArm7Var()->getIdent() == name &&
//            armReg->getArm7Var()->getMemoryLoc() == memoryLoc)
//            return armReg;
//    }
    auto *armRegRet = getFreeArmReg(armStmts);
    auto *armLdr = new ArmStmt(ARM_STMT_LDR, armRegRet->getRegName().c_str(),
                               ("[fp, #" + std::to_string(memoryLoc) + "]").c_str());
    armStmts->emplace_back(armLdr);
    return armRegRet;
}

ArmReg *ArmRegManager::getArmRegByLVal(LVal *lVal, std::vector<ArmStmt *> *armStmts) {
//    for (auto *armReg:*armRegs) {
//        /// 这种比较方法确认
//        if (armReg->getArm7Var() != nullptr && armReg->getArm7Var()->getIdent() == lVal->getIdent() &&
//            armReg->getArm7Var()->getMemoryLoc() == lVal->getIntPos())
//            return armReg;
//    }
    return nullptr;
}

//int ArmRegManager::getNewestNum() {
//    return newestNum;
//}
//
//void ArmRegManager::setNewestNum(int newestNum_) {
//    ArmRegManager::newestNum = newestNum_;
//}
//
//void ArmRegManager::addNewestNum() {
//    ArmRegManager::newestNum += 1;
//}

std::vector<ArmReg *> *ArmRegManager::getArmRegs() const {
    return armRegs;
}

//void ArmRegManager::setArmRegs(std::vector<ArmReg *> *armRegs_) {
//    ArmRegManager::armRegs = armRegs_;
//}

void ArmRegManager::freeAllArmReg(std::vector<ArmStmt *> *ArmStmts) {
    for (auto *armReg: *armRegs) {
        if (armReg->getArm7Var() != nullptr) {
            auto *armSTRStmt =
                    new ArmStmt(ARM_STMT_STR, armReg->getRegName().c_str(),
                                ("[fp, #" + std::to_string(armReg->getArm7Var()->getMemoryLoc()) + "]").c_str());
            ArmStmts->emplace_back(armSTRStmt);
            armReg->setArm7Var(nullptr);
        }
        armReg->setNewNum(0);
    }
    newestNum = 0;
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

void ArmRegManager::pushAllArmReg(std::vector<ArmStmt *> *ArmStmts) {
    for (int i = 0; i < armRegs->size(); i++) {
        auto *pushRXStmt = new ArmStmt(ARM_STMT_PUSH, ("{r" + std::to_string(i) + " }").c_str());
        ArmStmts->emplace_back(pushRXStmt);
    }
}

void ArmRegManager::popAllArmReg(std::vector<ArmStmt *> *ArmStmts) {
    for (int i = 0; i < armRegs->size(); i++) {
        auto *popRXStmt = new ArmStmt(ARM_STMT_POP, ("{r" + std::to_string(i) + " }").c_str());
        ArmStmts->emplace_back(popRXStmt);
    }
}

ArmReg *ArmRegManager::getOneArmReg(int i, std::vector<ArmStmt *> *ArmStmts) {
    if (armRegs->at(i)->getArm7Var() != nullptr) {
        auto *armSTRStmt =
                new ArmStmt(ARM_STMT_STR, armRegs->at(i)->getRegName().c_str(),
                            ("[fp, #" + std::to_string(armRegs->at(i)->getArm7Var()->getMemoryLoc()) + "]").c_str());
        ArmStmts->emplace_back(armSTRStmt);
        armRegs->at(i)->setArm7Var(nullptr);
    }

    return armRegs->at(i);
}

