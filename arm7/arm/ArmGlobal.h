#ifndef COMPILER_ARMGLOBAL_H
#define COMPILER_ARMGLOBAL_H

#include <vector>
#include <string>

/// 全局变量类
class Arm7GlobalVar;

/// 函数类
class Arm7GlobalFunc;

/// Arm汇编全局类
class ArmGlobal;

/// ARM树
class ArmTree {
private:
    std::vector<ArmGlobal *> *armGlobals;

public:
    /**
     * ARM树类
     * @param armGlobals 全局类型成员
     */
    explicit ArmTree(std::vector<ArmGlobal *> *armGlobals) : armGlobals(armGlobals) {};

    [[nodiscard]] std::vector<ArmGlobal *> *getArmGlobals() const {
        return armGlobals;
    }
};

class ArmGlobal {
private:
    Arm7GlobalVar *arm7GlobalVar;

    Arm7GlobalFunc *arm7GlobalFunc;

public:
    /**
     *
     * @param arm7GlobalVar 全局变量 可为null
     * @param arm7GlobalFunc 函数 可为null
     */
    ArmGlobal(Arm7GlobalVar *arm7GlobalVar, Arm7GlobalFunc *arm7GlobalFunc) :
            arm7GlobalVar(arm7GlobalVar), arm7GlobalFunc(arm7GlobalFunc) {};

    [[nodiscard]] Arm7GlobalVar *getArm7GlobalVar() const {
        return arm7GlobalVar;
    }

    [[nodiscard]] Arm7GlobalFunc *getArm7GlobalFunc() const {
        return arm7GlobalFunc;
    }
};

class Arm7GlobalVar {
private:
    std::string ident;

    int varType;

    int ifConst;

    std::vector<int> *subs;

    std::vector<int> *value;

public:
    /**
     *
     * @param ident 变量名
     * @param ifConst 0:变量;1:常量
     * @param subs size=0 表非数组
     * @param value 仅全局变量、局部常量有效
     */
    Arm7GlobalVar(const char *ident, int ifConst, std::vector<int> *subs, std::vector<int> *value) :
            ident(ident), ifConst(ifConst), subs(subs), value(value), varType() {};

    [[nodiscard]] const std::string &getIdent() const {
        return ident;
    }

    void setIdent(const char *ident_) {
        Arm7GlobalVar::ident = ident_;
    }

    [[nodiscard]] int getVarType() const {
        return varType;
    }

    void setVarType(int varType_) {
        Arm7GlobalVar::varType = varType_;
    }

    [[nodiscard]] int getIfConst() const {
        return ifConst;
    }

    void setIfConst(int ifConst_) {
        Arm7GlobalVar::ifConst = ifConst_;
    }

    [[nodiscard]] std::vector<int> *getSubs() const {
        return subs;
    }

    void setSubs(std::vector<int> *subs_) {
        Arm7GlobalVar::subs = subs_;
    }

    [[nodiscard]] std::vector<int> *getValue() const {
        return value;
    }

    void setValue(std::vector<int> *value_) {
        Arm7GlobalVar::value = value_;
    }
};


#endif //COMPILER_ARMGLOBAL_H
