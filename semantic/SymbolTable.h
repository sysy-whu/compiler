#ifndef COMPILER_SYMBOLTABLE_H
#define COMPILER_SYMBOLTABLE_H

#include <string>
#include <utility>
#include <vector>
#include "../util/MyConstants.h"
#include "../ast/AST.h"

/// 函数类
class Arm7Func;

/// 变量类
class Arm7Var;

/// 符号元素类
class Symbol;

/// 符号表类
class SymbolTable {
private:
    int tableType;

    std::string funcName;

    std::vector<Symbol *> *symbols;



public:
    /**
     * 符号表类SymbolTable 构造函数
     * @param tableType 符号表类型，global,local,extern...
     * @param symbols 符号表元素
     */
    SymbolTable(int tableType, const char *funcName, std::vector<Symbol *> *symbols) :
            tableType(tableType), funcName(funcName), symbols(symbols) {};

    [[nodiscard]] int getTableType() const {
        return tableType;
    }

    [[nodiscard]] std::vector<Symbol *> *getSymbols() const {
        return symbols;
    }

    [[nodiscard]] const std::string &getFuncName() const {
        return funcName;
    }

    Arm7Var *getGlobalVar(const char *varName);

};

class Symbol {
private:
    Arm7Var *arm7Var;

    Arm7Func *arm7Func;

public:
    /**
     * 符号元素类Symbol 构造函数
     * @param arm7Var 变量，可为 null
     * @param arm7Func 函数，可为 null
     */
    Symbol(Arm7Var *arm7Var, Arm7Func *arm7Func) : arm7Var(arm7Var), arm7Func(arm7Func) {};

    [[nodiscard]] Arm7Var *getArm7Var() const {
        return arm7Var;
    }

    void setArm7Var(Arm7Var *arm7Var_) {
        Symbol::arm7Var = arm7Var_;
    }

    [[nodiscard]] Arm7Func *getArm7Func() const {
        return arm7Func;
    }

    void setArm7Func(Arm7Func *arm7Func_) {
        Symbol::arm7Func = arm7Func_;
    }
};

class Arm7Func {
private:
    std::string ident;

    int retType;

    std::vector<Arm7Var *> *params;

    int capacity;

public:
    /**
     * 函数符号类Arm7Func 构造函数
     * @param ident 函数名
     * @param retType 返回值类型: 0:voidRet; 1:intRet
     * @param params 形参参数表
     * @param capacity 需要开辟的空间，此处即乘四按实际字节数算
     */
    Arm7Func(const char *ident, int retType, std::vector<Arm7Var *> *params) :
            ident(ident), retType(retType), params(params), capacity(4) {};

    [[nodiscard]] const std::string &getIdent() const {
        return ident;
    }

    [[nodiscard]] int getRetType() const {
        return retType;
    }

    [[nodiscard]] std::vector<Arm7Var *> *getParams() const {
        return params;
    }

    void setParams(std::vector<Arm7Var *> *params_) {
        Arm7Func::params = params_;
    }

    [[nodiscard]] int getCapacity() const {
        return capacity;
    }

    void addCapacityByOne() {
        Arm7Func::capacity += 4;
    }

    void addCapacityByNum(int num) {
        Arm7Func::capacity += 4 * num;
    }

};

class Arm7Var {
private:
    std::string ident;

    std::string funcName;

    int varType;

    int level;

    int ifConst;

    int ifArray;

    std::vector<int> *subs;

    std::vector<int> *value;

    InitVal *initVal;

    /// 如 8 表 [fp, #-8]
    int memoryLoc;

    /// 当前寄存器
    int registerNow;
    /// 是否锁定当前寄存器（局部使用量多，长留保留不反复store/load）
    int ifRegisterLock;

public:
    /**
     * 变量符号类 Arm7Var 构造函数
     * @param ident 变量名
     * @param level 变量代码块层次: 全局变量0; 实参参数1; 其余每遇到 { 则自增1; 每遇到 } 则自减1
     * @param ifConst 0:变量; 1:常量
     * @param ifArray 0:非数组; 1:数组
     * @param subs ifArray=1 时有效
     * @param value ifConst=1 时有效
     */
    Arm7Var(const char *ident, const char *funcName, int varType, int level, int ifConst, int ifArray,
            std::vector<int> *subs, std::vector<int> *value) :
            ident(ident), funcName(funcName), varType(varType), level(level), ifConst(ifConst), ifArray(ifArray),
            subs(subs), value(value), registerNow(-1), ifRegisterLock(0), memoryLoc(LOCAL_VAR_POS), initVal(nullptr) {};

    Arm7Var(const char *ident, const char *funcName, int varType, int level, int ifConst, int ifArray,
            std::vector<int> *subs, std::vector<int> *value, InitVal *initVal) :
            ident(ident), funcName(funcName), varType(varType), level(level), ifConst(ifConst), ifArray(ifArray),
            subs(subs), value(value), registerNow(-1), ifRegisterLock(0), memoryLoc(LOCAL_VAR_POS), initVal(initVal) {};

    [[nodiscard]] const std::string &getIdent() const {
        return ident;
    }

    [[nodiscard]] const std::string &getFuncName() const {
        return funcName;
    }

    void setIdent(const char *ident_) {
        Arm7Var::ident = ident_;
    }

    [[nodiscard]] int getLevel() const {
        return level;
    }

    [[nodiscard]] int getIfConst() const {
        return ifConst;
    }

    [[nodiscard]] int getIfArray() const {
        return ifArray;
    }

    [[nodiscard]] std::vector<int> *getSubs() const {
        return subs;
    }

    void setSubs(std::vector<int> *subs_) {
        Arm7Var::subs = subs_;
    }

    [[nodiscard]] std::vector<int> *getValue() const {
        return value;
    }

    void setValue(std::vector<int> *value_) {
        Arm7Var::value = value_;
    }

    [[nodiscard]] int getRegisterNow() const {
        return registerNow;
    }

    void setRegisterNow(int registerNow_) {
        Arm7Var::registerNow = registerNow_;
    }

    [[nodiscard]] int getIfRegisterLock() const {
        return ifRegisterLock;
    }

    void setIfRegisterLock(int ifRegisterLock_) {
        Arm7Var::ifRegisterLock = ifRegisterLock_;
    }

    [[nodiscard]] int getMemoryLoc() const {
        return memoryLoc;
    }

    void setMemoryLoc(int memoryLoc_) {
        Arm7Var::memoryLoc = memoryLoc_;
    }

    [[nodiscard]] int getVarType() const {
        return varType;
    }

    [[nodiscard]] InitVal *getInitVal() const {
        return initVal;
    }
};


#endif //COMPILER_SYMBOLTABLE_H
