#include "ArmRegManager.h"

ArmRegManager::ArmRegManager() {
    ArmRegManager::newestNum = 0;
    ArmRegManager::armRegs = new std::vector<ArmReg *>();
    for(int i=0;i<4;i++){
        auto *armReg = new ArmReg(("R"+std::to_string(i)).c_str(), nullptr, 0);
        armRegs->emplace_back(armReg);
    }

    for(int i=5;i<10;i++){
        auto *armReg = new ArmReg(("R"+std::to_string(i)).c_str(), nullptr, 0);
        armRegs->emplace_back(armReg);
    }
}

/// 按照 最先分配最先释放 来分配新寄存器
ArmReg *ArmRegManager::getFreeArmReg() {
    ArmReg *armRegRet = ArmRegManager::armRegs->at(0);
    for(auto & armRegI : *ArmRegManager::armRegs){
        if(armRegI->getArm7Var() == nullptr){
            return armRegI;
        }else{
            if(armRegI->getNewNum() < armRegRet->getNewNum()){
                armRegRet = armRegI;
            }
        }
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

