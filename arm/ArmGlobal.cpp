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
        std::string re = "\t.text\n"
                         "\t.align\t2\n"
                         "\t.global\tdiv_zt\n"
                         "\t.arch armv7-a\n"
                         "\t.syntax unified\n"
                         "\t.arm\n"
                         "\t.fpu vfp\n"
                         "\t.type\tdiv_zt, %function\n"
                         "div_zt:\n"
                         "\t@ args = 0, pretend = 0, frame = 16\n"
                         "\t@ frame_needed = 1, uses_anonymous_args = 0\n"
                         "\t@ link register save eliminated.\n"
                         "\tstr\tfp, [sp, #-4]!\n"
                         "\tadd\tfp, sp, #0\n"
                         "\tsub\tsp, sp, #20\n"
                         "\tstr\tr0, [fp, #-16]\n"
                         "\tstr\tr1, [fp, #-20]\n"
                         "\tmov\tr3, #0\n"
                         "\tstr\tr3, [fp, #-12]\n"
                         "\tmov\tr3, #0\n"
                         "\tstr\tr3, [fp, #-8]\n"
                         "\tldr\tr2, [fp, #-16]\n"
                         "\tldr\tr3, [fp, #-20]\n"
                         "\tcmp\tr2, r3\n"
                         "\tbge\t.L2\n"
                         "\tmov\tr3, #0\n"
                         "\tb\t.L3\n"
                         ".L2:\n"
                         "\tldr\tr3, [fp, #-20]\n"
                         "\tcmp\tr3, #0\n"
                         "\tbne\t.L4\n"
                         "\tmov\tr3, #0\n"
                         "\tb\t.L3\n"
                         ".L4:\n"
                         "\tldr\tr2, [fp, #-16]\n"
                         "\tldr\tr3, [fp, #-20]\n"
                         "\tsub\tr3, r2, r3\n"
                         "\tstr\tr3, [fp, #-16]\n"
                         "\tldr\tr3, [fp, #-8]\n"
                         "\tadd\tr3, r3, #1\n"
                         "\tstr\tr3, [fp, #-8]\n"
                         "\tldr\tr2, [fp, #-16]\n"
                         "\tldr\tr3, [fp, #-20]\n"
                         "\tcmp\tr2, r3\n"
                         "\tbge\t.L4\n"
                         "\tldr\tr3, [fp, #-8]\n"
                         ".L3:\n"
                         "\tmov\tr0, r3\n"
                         "\tadd\tsp, fp, #0\n"
                         "\t@ sp needed\n"
                         "\tldr\tfp, [sp], #4\n"
                         "\tbx\tlr\n"
                         "\t.size\tdiv_zt, .-div_zt\n"
                         "\t.align\t2\n"
                         "\t.global\tmod_zt\n"
                         "\t.syntax unified\n"
                         "\t.arm\n"
                         "\t.fpu vfp\n"
                         "\t.type\tmod_zt, %function\n"
                         "mod_zt:\n"
                         "\t@ args = 0, pretend = 0, frame = 16\n"
                         "\t@ frame_needed = 1, uses_anonymous_args = 0\n"
                         "\t@ link register save eliminated.\n"
                         "\tstr\tfp, [sp, #-4]!\n"
                         "\tadd\tfp, sp, #0\n"
                         "\tsub\tsp, sp, #20\n"
                         "\tstr\tr0, [fp, #-16]\n"
                         "\tstr\tr1, [fp, #-20]\n"
                         "\tmov\tr3, #0\n"
                         "\tstr\tr3, [fp, #-12]\n"
                         "\tmov\tr3, #0\n"
                         "\tstr\tr3, [fp, #-8]\n"
                         "\tldr\tr2, [fp, #-16]\n"
                         "\tldr\tr3, [fp, #-20]\n"
                         "\tcmp\tr2, r3\n"
                         "\tbge\t.L6\n"
                         "\tmov\tr3, #0\n"
                         "\tb\t.L7\n"
                         ".L6:\n"
                         "\tldr\tr3, [fp, #-20]\n"
                         "\tcmp\tr3, #0\n"
                         "\tbne\t.L8\n"
                         "\tmov\tr3, #0\n"
                         "\tb\t.L7\n"
                         ".L8:\n"
                         "\tldr\tr2, [fp, #-16]\n"
                         "\tldr\tr3, [fp, #-20]\n"
                         "\tsub\tr3, r2, r3\n"
                         "\tstr\tr3, [fp, #-16]\n"
                         "\tldr\tr3, [fp, #-8]\n"
                         "\tadd\tr3, r3, #1\n"
                         "\tstr\tr3, [fp, #-8]\n"
                         "\tldr\tr2, [fp, #-16]\n"
                         "\tldr\tr3, [fp, #-20]\n"
                         "\tcmp\tr2, r3\n"
                         "\tbge\t.L8\n"
                         "\tldr\tr3, [fp, #-16]\n"
                         ".L7:\n"
                         "\tmov\tr0, r3\n"
                         "\tadd\tsp, fp, #0\n"
                         "\t@ sp needed\n"
                         "\tldr\tfp, [sp], #4\n"
                         "\tbx\tlr\n";
        re += arm7GlobalFunc->genString();
        return re;
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
        re += ".word " + std::to_string(it) + "\n";
    }
    /// TODO 有警告
    /// warning: no return statement in function returning non-void [-Wreturn-type]
    return re;
}


std::string Arm7GlobalFunc::genString() {
    std::string re;
    // todo 遍历 blockAuxs 输出字符串常量
    for (auto funcInnerBlockAux:*blockAuxs) {
        re += funcName + ":\n";
        for (auto value: *funcInnerBlockAux->getValues()) {
            re += ".ascii \"" + value + "\"\n";
        }
        re += ".align 2\n";
    }

    re += ".text\n";
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

