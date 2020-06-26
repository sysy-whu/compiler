#ifndef COMPILER_SIMBOLTABLE_H
#define COMPILER_SIMBOLTABLE_H

#include <string>
#include <utility>
#include <vector>

class Symbol;

/**
 * 全局的/某一代码块中的符号表
 */
class SymbolTable {
private:
    /// 符号表类型 global, local, extern...
    std::string type;

    std::vector<Symbol *> *symbols;

public:
    /**
     *
     * @param type
     * @param symbol
     */
    SymbolTable(const char *type, std::vector<Symbol *> *symbols) : type(type), symbols(symbols) {};

    [[nodiscard]] std::string getType() const {
        return type;
    }

    [[nodiscard]] const std::vector<Symbol *> *getSymbols() const {
        return symbols;
    }

    void setSymbols(std::vector<Symbol *> *symbol) {
        SymbolTable::symbols = symbol;
    }

    void addSymbol(Symbol *symbol) {
        symbols->emplace_back(symbol);
    }

};

/**
 * 变量符号VarSymbol
 */
class VarSymbol {
private:
    std::string ident;
    /// 目前仅int
    std::string varType;

    int value, hasInited;

    unsigned int row, column;

public:
    /**
     *
     * @param ident
     * @param varType
     * @param value
     * @param hasInited
     * @param row
     * @param column
     */
    VarSymbol(const char *ident, const char *varType, int value, int hasInited, unsigned int row, unsigned int column) :
            ident(ident), varType(varType), value(value), hasInited(hasInited), row(row),
            column(column) {};

    [[nodiscard]] std::string getIdent() const {
        return ident;
    }

    [[nodiscard]] std::string getVarType() const {
        return varType;
    }

    [[nodiscard]] int getValue() const {
        return value;
    }

    void setValue(int value_) {
        VarSymbol::value = value_;
    }

    [[nodiscard]] int getHasInited() const {
        return hasInited;
    }

    /**
     * 维护是否已经初始化
     * @param hasInited
     */
    void setHasInited(int hasInited_) {
        VarSymbol::hasInited = hasInited_;
    }

    [[nodiscard]] unsigned int getRow() const {
        return row;
    }

    [[nodiscard]] unsigned int getColumn() const {
        return column;
    }

};

/**
 * 常量符号ConstVarSymbol
 */
class ConstVarSymbol {
private:
    std::string ident;

    std::string varType;

    int value;

    unsigned int row, column;

public:
    /**
     *
     * @param ident
     * @param varType
     * @param value
     * @param row
     * @param column
     */
    ConstVarSymbol(const char *ident, const char *varType, int value, unsigned int row, unsigned int column) :
            ident(ident), varType(varType), value(value), row(row), column(column) {};

    [[nodiscard]] std::string getIdent() const {
        return ident;
    }

    [[nodiscard]] std::string getVarType() const {
        return varType;
    }

    [[nodiscard]] int getValue() const {
        return value;
    }

    [[nodiscard]] unsigned int getRow() const {
        return row;
    }

    [[nodiscard]] unsigned int getColumn() const {
        return column;
    }

};

/**
 * 数组变量符号VarArrSymbol
 */
class VarArrSymbol {
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
    /**
     *
     * @param ident
     * @param varType
     * @param value
     * @param subs
     * @param hasInited
     * @param row
     * @param column
     */
    VarArrSymbol(std::string ident, std::string varType, std::vector<int> value, std::vector<int> subs, int hasInited,
                 unsigned int row, unsigned int column) :
            ident(std::move(ident)), varType(std::move(varType)), value(std::move(value)), subs(std::move(subs)),
            hasInited(hasInited), row(row), column(column) {};

    [[nodiscard]] const std::string &getIdent() const {
        return ident;
    }

    [[nodiscard]] const std::string &getVarType() const {
        return varType;
    }

    void setVarType(const std::string &varType_) {
        VarArrSymbol::varType = varType_;
    }

    [[nodiscard]] const std::vector<int> &getValue() const {
        return value;
    }

    void setValue(const std::vector<int> &value_) {
        VarArrSymbol::value = value_;
    }

    [[nodiscard]] const std::vector<int> &getSubs() const {
        return subs;
    }

    void setSubs(const std::vector<int> &subs_) {
        VarArrSymbol::subs = subs_;
    }

    [[nodiscard]] int getHasInited() const {
        return hasInited;
    }

    void setHasInited(int hasInited_) {
        VarArrSymbol::hasInited = hasInited_;
    }

    [[nodiscard]] unsigned int getRow() const {
        return row;
    }

    void setRow(unsigned int row_) {
        VarArrSymbol::row = row_;
    }

    [[nodiscard]] unsigned int getColumn() const {
        return column;
    }

    void setColumn(unsigned int column_) {
        VarArrSymbol::column = column_;
    }

};

/**
 * 数组常量符号ConstVarArrSymbol
 */
class ConstVarArrSymbol {
private:
    std::string ident;

    std::string varType;
    /// 目前仅int
    std::vector<int> value;
    /// 维度们
    std::vector<int> subs;

    unsigned int row, column;
public:
    /**
     *
     * @param ident
     * @param varType
     * @param value
     * @param subs
     * @param row
     * @param column
     */
    ConstVarArrSymbol(const char *ident, const char *varType, std::vector<int> value, std::vector<int> subs,
                      unsigned int row, unsigned int column) :
            ident(ident), varType(varType), value(std::move(value)), subs(std::move(subs)), row(row), column(column) {};

    [[nodiscard]] const std::string &getIdent() const {
        return ident;
    }

    [[nodiscard]] const std::string &getVarType() const {
        return varType;
    }

    [[nodiscard]] const std::vector<int> &getValue() const {
        return value;
    }

    [[nodiscard]] const std::vector<int> &getSubs() const {
        return subs;
    }

    [[nodiscard]] unsigned int getRow() const {
        return row;
    }

    [[nodiscard]] unsigned int getColumn() const {
        return column;
    }

};

/**
 * 函数符号FuncSymbol
 */
class FuncSymbol {
private:
    std::string ident;

    std::string retType;

    std::vector<Symbol *> *params;

    unsigned int row, column;

public:
    /**
     *
     * @param ident
     * @param retType
     * @param paramsType
     * @param paramsName
     * @param row
     * @param column
     */
    FuncSymbol(const char *ident, const char *retType, std::vector<Symbol *> *params, unsigned int row,
               unsigned int column) :
            ident(ident), retType(retType), params(params), row(row), column(column) {};

    [[nodiscard]] std::vector<Symbol *> *getParams() const {
        return params;
    }

    [[nodiscard]] const std::string &getIdent() const {
        return ident;
    }

    [[nodiscard]] const std::string &getRetType() const {
        return retType;
    }

    [[nodiscard]] unsigned int getRow() const {
        return row;
    }

    [[nodiscard]] unsigned int getColumn() const {
        return column;
    }
};

class Symbol {
private:
    VarSymbol *varSymbol;
    VarArrSymbol *varArrSymbol;
    ConstVarSymbol *constVarSymbol;
    ConstVarArrSymbol *constVarArrSymbol;
    FuncSymbol *funcSymbol;

public:
    Symbol(VarSymbol *varSymbol1, VarArrSymbol *varArrSymbol1, ConstVarSymbol *constVarSymbol1,
           ConstVarArrSymbol *constVarArrSymbol1, FuncSymbol *funcSymbol1) :
            varArrSymbol(varArrSymbol1), varSymbol(varSymbol1), constVarArrSymbol(constVarArrSymbol1),
            constVarSymbol(constVarSymbol1), funcSymbol(funcSymbol1) {};

    [[nodiscard]] VarSymbol *getVarSymbol() const {
        return varSymbol;
    }

    [[nodiscard]] VarArrSymbol *getVarArrSymbol() const {
        return varArrSymbol;
    }

    [[nodiscard]] ConstVarSymbol *getConstVarSymbol() const {
        return constVarSymbol;
    }

    [[nodiscard]] ConstVarArrSymbol *getConstVarArrSymbol() const {
        return constVarArrSymbol;
    }

    [[nodiscard]] FuncSymbol *getFuncSymbol() const {
        return funcSymbol;
    }
};

#endif //COMPILER_SIMBOLTABLE_H
