#include "ArmGlobal.h"


std::string Arm7Tree::genString() {
    std::string re = ".arch armv7-a\n";
    re += "\t.text\n";
    re += "\t.global	__aeabi_idiv\n";
    re += "\t.global	__aeabi_idivmod\n";
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
        std::string re;
        re += arm7GlobalFunc->genString();
        return re;
    }
    return "";
}


std::string Arm7GlobalVar::genString() {
    // todo Arm7GlobalVar的输出方法
    std::string re = "\t.text\n";
    re += "\t.global " + ident + "\n";
    re += "\t.data\n";
    re += "\t.align 2\n";
    re += "\t.type " + ident + ", %object\n";
    /// TODO
    /// 这里的 .size 肯定有误;
    /// 1、ArmGlobal.h Line:80 Arm7GlobalVar 构造函数说明有歧义（已完善）
    ///   见 SymbolTable.h Line:160 Arm7Var 构造函数各个变量说明, subs仅在 ifArray 为 ARRAY_TRUE 时有效（不为null）
    /// 2、subs 是各个维度的长度，不是总元素个数
    if (subs == nullptr) {
        re += "\t.size " + ident + "," + std::to_string(4) + "\n";
    } else {
        int len = 1;
        for (int sub:*subs) {
            len *= sub;
        }
        re += "\t.size " + ident + "," + std::to_string(len * 4) + "\n";
    }
    re += ident + ":\n";
    for (auto it:*value) {
        re += "\t.word " + std::to_string(it) + "\n";
    }
    /// TODO 有警告
    /// warning: no return statement in function returning non-void [-Wreturn-type]
    return re;
}


std::string Arm7GlobalFunc::genString() {
    std::string re;
    // todo 遍历 blockAuxs 输出字符串常量
    for (auto funcInnerBlockAux:*blockAuxs) {
        re += funcInnerBlockAux->getBlockName() + ":\n";
        for (auto value: *funcInnerBlockAux->getValues()) {
            re += ".ascii \"" + value + "\"\n";
        }
        re += "\t.align 2\n";
    }

    re += "\t.text\n";
    re += "\t.align 2\n";
    re += "\t.global " + funcName + "\n";
    re += "\t.syntax unified\n";
    re += "\t.arm\n";
    re += "\t.fpu vfp\n";
    re += "\t.type " + funcName + ", %function\n";
    re += funcName + ":\n";
    for (auto armBlock:*armBlocks) {
        re += armBlock->genString();
    }

    re += "\tsub sp, fp, #8\n";
    re += "\t@ sp needed\n";
    re += "\tpop {r4,fp, pc}\n";
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

void Arm7GlobalFunc::setBlockAuxs(std::vector<FuncInnerBlockAux *> *blockAuxs_) {
    Arm7GlobalFunc::blockAuxs = blockAuxs_;
}

