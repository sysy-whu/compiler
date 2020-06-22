#ifndef COMPILER_SIMBOLTABLE_H
#define COMPILER_SIMBOLTABLE_H


#include <string>
#include <utility>
#include <vector>


class Symbol {

};

/**
 * 全局的/某一代码块中的符号表
 */
class SymbolTable {
private:
    /// 符号表类型 global, local, extern...
    std::string type;

    std::vector<Symbol> symbols;

public:
    SymbolTable(std::string type, std::vector<Symbol> symbol) : type(std::move(type)), symbols(std::move(symbol)) {};

    const std::string &getType() const {
        return type;
    }

    const std::vector<Symbol> &getSymbols() const {
        return symbols;
    }

    void setSymbols(const std::vector<Symbol> &symbol) {
        SymbolTable::symbols = symbol;
    }

    void addSymbol(const Symbol symbol) {
        SymbolTable::symbols.push_back(symbol);
    }

};

/**
 * 变量符号VarSymbol
 */
class VarSymbol : public Symbol {
private:
    std::string ident;
    /// 目前仅int
    std::string varType;

    int value, hasInited;

    unsigned int row, column;

public:
    VarSymbol(std::string ident, std::string varType, int value, int hasInited, unsigned int row, unsigned int column) :
            ident(std::move(ident)), varType(std::move(varType)), value(value), hasInited(hasInited), row(row),
            column(column) {};

    const std::string &getIdent() const {
        return ident;
    }

    const std::string &getVarType() const {
        return varType;
    }

    int getValue() const {
        return value;
    }

    void setValue(int value) {
        VarSymbol::value = value;
    }

    int getHasInited() const {
        return hasInited;
    }

    /**
     * 维护是否已经初始化
     * @param hasInited
     */
    void setHasInited(int hasInited) {
        VarSymbol::hasInited = hasInited;
    }

    unsigned int getRow() const {
        return row;
    }

    unsigned int getColumn() const {
        return column;
    }

};

/**
 * 常量符号ConstVarSymbol
 */
class ConstVarSymbol : public Symbol {
private:
    std::string ident;

    std::string varType;

    int value;

    unsigned int row, column;
public:
    ConstVarSymbol(std::string ident, std::string varType, int value, unsigned int row, unsigned int column) :
            ident(std::move(ident)), varType(std::move(varType)), value(value), row(row), column(column) {};

    const std::string &getIdent() const {
        return ident;
    }

    const std::string &getVarType() const {
        return varType;
    }

    int getValue() const {
        return value;
    }

    unsigned int getRow() const {
        return row;
    }

    unsigned int getColumn() const {
        return column;
    }

};

/**
 * 数组变量符号VarArrSymbol
 */
class VarArrSymbol : public Symbol {
private:
    std::string ident;

    std::string varType;
    /// 目前仅int
    std::vector<int> value;
    /// 维度们
    std::vector<int> subs;

    int hasInited;

    unsigned int row, column;
public:
    VarArrSymbol(std::string ident, std::string varType, std::vector<int> value, std::vector<int> subs, int hasInited,
                 unsigned int row, unsigned int column) :
            ident(std::move(ident)), varType(std::move(varType)), value(std::move(value)), subs(std::move(subs)),
            hasInited(hasInited), row(row), column(column) {};

    const std::string &getIdent() const {
        return ident;
    }

    const std::string &getVarType() const {
        return varType;
    }

    const std::vector<int> &getValue() const {
        return value;
    }

    void setValue(const std::vector<int> &value) {
        VarArrSymbol::value = value;
    }

    const std::vector<int> &getSubs() const {
        return subs;
    }

    void setSubs(const std::vector<int> &subs) {
        VarArrSymbol::subs = subs;
    }

    /**
     * 维护是否已经初始化
     * @param hasInited
     */
    int getHasInited() const {
        return hasInited;
    }

    void setHasInited(int hasInited) {
        VarArrSymbol::hasInited = hasInited;
    }

    unsigned int getRow() const {
        return row;
    }

    unsigned int getColumn() const {
        return column;
    }

};

/**
 * 数组常量符号ConstVarArrSymbol
 */
class ConstVarArrSymbol : public Symbol {
private:
    std::string ident;

    std::string varType;
    /// 目前仅int
    std::vector<int> value;
    /// 维度们
    std::vector<int> subs;

    unsigned int row, column;
public:
    ConstVarArrSymbol(std::string ident, std::string varType, std::vector<int> value, std::vector<int> subs,
                      unsigned int row, unsigned int column) :
            ident(std::move(ident)), varType(std::move(varType)), value(std::move(value)), subs(std::move(subs)),
            row(row), column(column) {};

    const std::string &getIdent() const {
        return ident;
    }

    const std::string &getVarType() const {
        return varType;
    }

    const std::vector<int> &getValue() const {
        return value;
    }

    const std::vector<int> &getSubs() const {
        return subs;
    }

    unsigned int getRow() const {
        return row;
    }

    unsigned int getColumn() const {
        return column;
    }

};

/**
 * 函数符号FuncSymbol
 */
class FuncSymbol : public Symbol {
private:
    std::string ident;

    std::string retType;

    std::vector<std::string> paramsType;

    std::vector<std::string> paramsName;

    unsigned int row, column;

public:
    FuncSymbol(std::string ident, std::string retType, std::vector<std::string> paramsType,
               std::vector<std::string> paramsName, unsigned int row, unsigned int column) :
            ident(std::move(ident)), retType(std::move(retType)), paramsName(std::move(paramsName)),
            paramsType(std::move(paramsType)), row(row), column(column) {};


    const std::string &getIdent() const {
        return ident;
    }

    const std::string &getRetType() const {
        return retType;
    }

    const std::vector<std::string> &getParamsType() const {
        return paramsType;
    }

    const std::vector<std::string> &getParamsName() const {
        return paramsName;
    }

    unsigned int getRow() const {
        return row;
    }

    unsigned int getColumn() const {
        return column;
    }
};


#endif //COMPILER_SIMBOLTABLE_H
