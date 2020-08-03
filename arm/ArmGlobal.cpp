#include "ArmGlobal.h"


std::string Arm7Tree::genString() {
    std::string re = "";
    for(auto armGlobal: *armGlobals){
        re += armGlobal->genString();
    }
    return re;
}

std::string ArmGlobal::genString() {
    // todo ArmGlobal 的输出方法
    if (arm7GlobalVar != nullptr) {
        return "";
    }
    if (arm7GlobalFunc != nullptr) {
        return arm7GlobalFunc->genString();
    }
    return "";
}


std::string Arm7GlobalVar::genString() {
    // todo Arm7GlobalVar的输出方法
}


std::string Arm7GlobalFunc::genString() {
    // todo Arm7GlobalFunc 的输出方法
    std::string re = funcName + ":\n";
    for (auto armBlock:*armBlocks) {
        re += armBlock->genString();
    }
    return re;
}

int Arm7GlobalFunc::getPushLen() const {
    return pushLen;
}

void Arm7GlobalFunc::setPushLen(int pushLen_) {
    Arm7GlobalFunc::pushLen = pushLen_;
}

