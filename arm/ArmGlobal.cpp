#include "ArmGlobal.h"


std::string Arm7Tree::genString() {
    std::string re = "";
    for (auto armGlobal: *armGlobals) {
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
    std::string re = ".global " + ident + "\n";
    re += ".align 2\n";
    re += ".type " + ident + ", %object";
    if (subs->size() == 0) {
        re += ".size " + ident + "," + std::to_string(4) + "\n";
    } else {
        re += ".size " + ident + "," + std::to_string(subs->size() * 4) + "\n";
    }
    re += "." + ident + "\n";
    for (auto it:*value) {
        re += ".word " + std::to_string(value->at(0));
    }
}


std::string Arm7GlobalFunc::genString() {
    // todo Arm7GlobalFunc 的输出方法
    std::string re = ".global " + funcName + "\n";
    re += ".global " + funcName + "\n";
    re += ".type " + funcName + ",%function\n";
    for (auto armBlock:*armBlocks) {
        re += armBlock->genString();
    }
    return re;
}


