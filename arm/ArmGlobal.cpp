#include "ArmGlobal.h"


std::string Arm7Tree::genString() {
    std::string re = ".arch armv7-a\n";
    for (auto armGlobal: *armGlobals) {
        re += armGlobal->genString();
    }
    return re;
}

std::string ArmGlobal::genString() {
    // todo ArmGlobal 的输出方法
    if (arm7GlobalVar != nullptr) {
        return arm7GlobalVar->genString();
    }
    if (arm7GlobalFunc != nullptr) {
        return arm7GlobalFunc->genString();
    }
    return "";
}


std::string Arm7GlobalVar::genString() {
    // todo Arm7GlobalVar的输出方法
    std::string re = ".text\n";
    re += ".global " + ident + "\n";
    re += ".data\n";
    re += ".align 2\n";
    re += ".type " + ident + ", %object\n";
    /// TODO
    /// 这里的 .size 肯定有误;
    /// 1、ArmGlobal.h Line:80 Arm7GlobalVar 构造函数说明有歧义（已完善）
    ///   见 SymbolTable.h Line:160 Arm7Var 构造函数各个变量说明, subs仅在 ifArray 为 ARRAY_TRUE 时有效（不为null）
    /// 2、subs 是各个维度的长度，不是总元素个数
    if (subs == nullptr) {
        re += ".size " + ident + "," + std::to_string(4) + "\n";
    } else {
        int len = 1;
        for (int sub:*subs) {
            len *= sub;
        }
        re += ".size " + ident + "," + std::to_string(len * 4) + "\n";
    }
    re += ident + ":\n";
    for (auto it:*value) {
        re += ".word " + std::to_string(value->at(0))+"\n";
    }
    /// TODO 有警告
    /// warning: no return statement in function returning non-void [-Wreturn-type]
    return re;
}


std::string Arm7GlobalFunc::genString() {
    // todo Arm7GlobalFunc 的输出方法
    std::string re = ".text\n";
    re += ".align 2\n";
    re += ".global " + funcName + "\n";
    re += ".syntax unified\n";
    re += ".arm\n";
    re += ".fpu vfp\n";
    re += ".type " + funcName + ", %function\n";
    re += funcName + ":\n";
    for (auto armBlock:*armBlocks) {
        re += armBlock->genString();
    }

    re += "sub sp, fp, #8\n";
    re += "@ sp needed\n";
    re += "pop {r4,fp, pc}\n";
    return re;
}

int Arm7GlobalFunc::getPushLen() const {
    return pushLen;
}

void Arm7GlobalFunc::setPushLen(int pushLen_) {
    Arm7GlobalFunc::pushLen = pushLen_;
}

std::vector<FuncInnerBlockAux *> *Arm7GlobalFunc::getBlockAuxs() const {
    return blockAuxs;
}

