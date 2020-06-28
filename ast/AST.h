#ifndef COMPILER_AST_H
#define COMPILER_AST_H

#include <vector>
#include <string>
#include "SourceLocation.h"

/**
 * 声明 Decl -> ConstDecl | VarDecl | FuncDef
 */
class Decl;

class VarDecl;

class ConstDecl;

/**
 * 表达式 Exp → AddExp 注：SysY 表达式是 int 型表达式
 */
class Exp;

/**
 * 语句类 Stmt
 */
class Stmt;

/**
 * 函数实参表 FuncRParams → Exp { ',' Exp } | '"' ident '"' { ',' Exp }
 */
class FuncRParams {
private:
    std::string strOut;

    std::vector<Exp *> *exps;

public:
    /**
     * 函数实参表 FuncRParams
     * @param strOut 字节流
     * @param exps 实参表
     */
    FuncRParams(const char *strOut, std::vector<Exp *> *exps) : strOut(strOut), exps(exps) {}

    [[nodiscard]] std::vector<Exp *> *getExps() const {
        return exps;
    }

    [[nodiscard]] const std::string &getStrOut() const {
        return strOut;
    }
};


/**
 * 左值表达式 LVal → Ident {'[' Exp ']'}
 */
class LVal : public Locs {
private:
    std::string ident;

    std::vector<Exp *> *exps;

public:
    /**
     * 左值表达式 LVal
     * @param ident
     * @param exps 数组各维度 可以为null
     * @param sourLocs _0: identLoc; [_Loc, ]_Loc, ... 数组各维度
     */
    LVal(const char *ident, std::vector<Exp *> *exps, std::vector<SourceLocation *> *sourLocs) :
            ident(ident), exps(exps), Locs(sourLocs) {};

    [[nodiscard]] const std::string &getIdent() const {
        return ident;
    }

    [[nodiscard]] std::vector<Exp *> *getExps() const {
        return exps;
    }
};

/**
 * 基本表达式 PrimaryExp → '(' Exp ')' | LVal | IntConst
 */
class PrimaryExp : public Locs {
private:
    Exp *exp;

    LVal *lVal;

    int number;

public:
    /**
     * 基本表达式 PrimaryExp
     * @param exp
     * @param lVal
     * @param number
     * @param sourLocs 带括号时: _0: (_Loc, _1: )_Loc
     */
    PrimaryExp(Exp *exp, LVal *lVal, int number, std::vector<SourceLocation *> *sourLocs) :
            exp(exp), lVal(lVal), number(number), Locs(sourLocs) {};

    [[nodiscard]] Exp *getExp() const {
        return exp;
    }

    [[nodiscard]] LVal *getLVal() const {
        return lVal;
    }

    [[nodiscard]] int getNumber() const {
        return number;
    }
};

/**
 * 一元表达式 UnaryExp → PrimaryExp
 *                   | Ident '(' [FuncRParams] ')'
 *                   | ('+' | '-' | '!') UnaryExp 注：'!'仅出现在条件表达式中
 */
class UnaryExp : public Locs {
private:
    PrimaryExp *primaryExp;

    std::string ident;

    FuncRParams *funcRParams;

    int opType;

    UnaryExp *unaryExp;

public:
    /**
     * 一元表达式 UnaryExp
     * @param primaryExp
     * @param ident
     * @param funcRParams
     * @param unaryExp
     * @param sourLocs PrimaryExp: null | 函数调用: _0: identLoc, _1: (_Loc, _2: )_Loc
     *                            | 一元式: _0: opTypeLoc
     *
     */
    UnaryExp(PrimaryExp *primaryExp, const char *ident, FuncRParams *funcRParams, int opType, UnaryExp *unaryExp,
             std::vector<SourceLocation *> *sourLocs) :
            primaryExp(primaryExp), ident(ident), funcRParams(funcRParams), opType(opType), unaryExp(unaryExp),
            Locs(sourLocs) {};

    [[nodiscard]] PrimaryExp *getPrimaryExp() const {
        return primaryExp;
    }

    [[nodiscard]] const std::string &getIdent() const {
        return ident;
    }

    [[nodiscard]] FuncRParams *getFuncRParams() const {
        return funcRParams;
    }

    [[nodiscard]] UnaryExp *getUnaryExp() const {
        return unaryExp;
    }

    [[nodiscard]] int getOpType() const {
        return opType;
    }
};

/**
 * 乘除模表达式 MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
 */
class MulExp : public Locs {
private:
    MulExp *mulExp;

    UnaryExp *unaryExp;

    int opType;

public:
    /**
     * 乘除模表达式 MulExp
     * @param mulExp 可以为null
     * @param unaryExp
     * @param opType
     * @param sourLocs opType 位置
     */
    MulExp(MulExp *mulExp, UnaryExp *unaryExp, int opType, std::vector<SourceLocation *> *sourLocs) :
            mulExp(mulExp), unaryExp(unaryExp), opType(opType), Locs(sourLocs) {};

    [[nodiscard]] MulExp *getMulExp() const {
        return mulExp;
    }

    [[nodiscard]] UnaryExp *getUnaryExp() const {
        return unaryExp;
    }

    [[nodiscard]] int getOpType() const {
        return opType;
    }
};

/**
 * 加减表达式 AddExp → MulExp | AddExp ('+' | '−') MulExp
 */
class AddExp : public Locs {
private:
    AddExp *addExp;

    MulExp *mulExp;

    int opType;

public:
    /**
     * 加减表达式 AddExp
     * @param addExp 可以为null
     * @param mulExp
     * @param opType
     * @param sourLocs opType位置
     */
    AddExp(AddExp *addExp, MulExp *mulExp, int opType, std::vector<SourceLocation *> *sourLocs) :
            addExp(addExp), mulExp(mulExp), opType(opType), Locs(sourLocs) {};

    [[nodiscard]] AddExp *getAddExp() const {
        return addExp;
    }

    [[nodiscard]] MulExp *getMulExp() const {
        return mulExp;
    }

    [[nodiscard]] int getOpType() const {
        return opType;
    }
};

/**
 * 关系表达式 RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp
 */
class RelExp : public Locs {
private:
    RelExp *relExp;

    AddExp *addExp;

    int opType;

public:
    /**
     * 关系表达式 RelExp
     * @param relExp 可以为null
     * @param addExp
     * @param opType
     * @param sourLocs opType位置
     */
    RelExp(RelExp *relExp, AddExp *addExp, int opType, std::vector<SourceLocation *> *sourLocs) :
            relExp(relExp), addExp(addExp), opType(opType), Locs(sourLocs) {};

    [[nodiscard]] RelExp *getRelExp() const {
        return relExp;
    }

    [[nodiscard]] AddExp *getAddExp() const {
        return addExp;
    }

    [[nodiscard]] int getOpType() const {
        return opType;
    }
};

/**
 * 相等性表达式 EqExp → RelExp | EqExp ('==' | '!=') RelExp
 */
class EqExp : public Locs {
private:
    EqExp *eqExp;

    RelExp *relExp;

    int opType;

public:
    /**
     * 相等性表达式 EqExp
     * @param eqExp 可以为null
     * @param relExp
     * @param opType
     * @param sourLocs opType 位置
     */
    EqExp(EqExp *eqExp, RelExp *relExp, int opType, std::vector<SourceLocation *> *sourLocs) :
            eqExp(eqExp), relExp(relExp), opType(opType), Locs(sourLocs) {};

    [[nodiscard]] EqExp *getEqExp() const {
        return eqExp;
    }

    [[nodiscard]] RelExp *getRelExp() const {
        return relExp;
    }

    [[nodiscard]] int getOpType() const {
        return opType;
    }
};

/**
 * 逻辑与表达式 LAndExp → EqExp | LAndExp '&&' EqExp
 */
class LAndExp : public Locs {
private:
    LAndExp *lAndExp;

    EqExp *eqExp;

public:
    /**
     * 逻辑与表达式 LAndExp
     * @param lAndExp 可以为null
     * @param eqExp
     * @param sourLocs && 位置
     */
    LAndExp(LAndExp *lAndExp, EqExp *eqExp, std::vector<SourceLocation *> *sourLocs) :
            lAndExp(lAndExp), eqExp(eqExp), Locs(sourLocs) {};

    [[nodiscard]] LAndExp *getLAndExp() const {
        return lAndExp;
    }

    [[nodiscard]] EqExp *getEqExp() const {
        return eqExp;
    }
};

/**
 * 逻辑或表达式 LOrExp → LAndExp | LOrExp '||' LAndExp
 */
class LOrExp : public Locs {
private:
    LOrExp *lOrExp;

    LAndExp *lAndExp;

public:
    /**
     * 逻辑或表达式 LOrExp 构造函数
     * @param lOrExp 可为null
     * @param lAndExp
     * @param sourLocs || 位置
     */
    LOrExp(LOrExp *lOrExp, LAndExp *lAndExp, std::vector<SourceLocation *> *sourLocs) :
            lOrExp(lOrExp), lAndExp(lAndExp), Locs(sourLocs) {};

    [[nodiscard]] LOrExp *getLOrExp() const {
        return lOrExp;
    }

    [[nodiscard]] LAndExp *getLAndExp() const {
        return lAndExp;
    }
};

/**
 * 条件表达式 Cond → LOrExp
 */
class Cond {
private:
    LOrExp *lOrExp;

public:
    /**
     * 条件表达式 Cond 构造函数
     * @param lOrExp 条件表达式
     */
    explicit Cond(LOrExp *lOrExp) : lOrExp(lOrExp) {};

    [[nodiscard]] LOrExp *getLOrExp() const {
        return lOrExp;
    }
};

/**
 * 表达式 Exp → AddExp 注：SysY 表达式是 int 型表达式
 */
class Exp {
private:
    AddExp *addExp;

public:
    /**
     * 表达式 Exp → AddExp 注：SysY 表达式是 int 型表达式
     * @param addExp
     */
    explicit Exp(AddExp *addExp) : addExp(addExp) {};

    [[nodiscard]] AddExp *getAddExp() const {
        return addExp;
    }
};


/**
 * 常量表达式 ConstExp → AddExp 注：使用的 Ident 必须是常量
 */
class ConstExp {
private:
    AddExp *addExp;

public:
    /**
     * 常量表达式 ConstExp → AddExp 注：SysY 表达式是 int 型表达式
     * @param addExp
     */
    explicit ConstExp(AddExp *addExp) : addExp(addExp) {};

    [[nodiscard]] AddExp *getAddExp() const {
        return addExp;
    }
};

/**
 * 语句块项 BlockItem → VarDecl | ConstDecl | Stmt
 */
class BlockItem {
private:
    VarDecl *varDecl;

    ConstDecl *constDecl;

    Stmt *stmt;

public:
    /**
     * 语句块项 BlockItem
     * @param constDecl
     * @param varDecl
     * @param stmt 语句 Stmt
     */
    BlockItem(ConstDecl *constDecl, VarDecl *varDecl, Stmt *stmt) :
            constDecl(constDecl), varDecl(varDecl), stmt(stmt) {};

    [[nodiscard]] VarDecl *getVarDecl() const {
        return varDecl;
    }

    [[nodiscard]] ConstDecl *getConstDecl() const {
        return constDecl;
    }

    [[nodiscard]] Stmt *getStmt() const {
        return stmt;
    }
};

/**
 * 语句块 Block → '{' { BlockItem } '}'
 */
class Block : public Locs {
private:
    std::vector<BlockItem *> *blockItems;

public:
    /**
     * 语句块 Block
     * @param blockItems 代码块一个个语句
     * @param sourLocs _0: {_Loc, _1: }_Loc
     */
    Block(std::vector<BlockItem *> *blockItems, std::vector<SourceLocation *> *sourLocs) :
            blockItems(blockItems), Locs(sourLocs) {};

    [[nodiscard]] const std::vector<BlockItem *> *getBlockItems() const {
        return blockItems;
    }
};

/**
 * 语句 Stmt → LVal '=' Exp ';'
 *          | [Exp] ';'
 *          | Block
 *          | 'if' '( Cond ')' Stmt [ 'else' Stmt ]
 *          | 'while' '(' Cond ')' Stmt
 *          | 'break' ';'
 *          | 'continue' ';'
 *          | 'return' [Exp] ';'
 */
class Stmt : public Locs {
private:
    int stmtType;

    LVal *lVal;

    Exp *exp;

    Block *block;

    Cond *cond;

    Stmt *stmtBrBody;

    Stmt *elseBody;

public:
    /**
     * 语句 Stmt
     * @param stmtType
     * @param lVal
     * @param block
     * @param exp
     * @param cond
     * @param stmtBrBody
     * @param stmtBody
     */
    Stmt(int stmtType, LVal *lVal, Exp *exp, Block *block, Cond *cond, Stmt *stmtBrBody, Stmt *elseBody,
         std::vector<SourceLocation *> *sourLocs) :
            stmtType(stmtType), lVal(lVal), exp(exp), block(block), cond(cond), stmtBrBody(stmtBrBody),
            elseBody(elseBody),
            Locs(sourLocs) {};

    [[nodiscard]] Block *getBlock() const {
        return block;
    }

    [[nodiscard]] int getStmtType() const {
        return stmtType;
    }

    [[nodiscard]] LVal *getLVal() const {
        return lVal;
    }

    [[nodiscard]] Exp *getExp() const {
        return exp;
    }

    [[nodiscard]] Cond *getCond() const {
        return cond;
    }

    [[nodiscard]] Stmt *getStmtBrBody() const {
        return stmtBrBody;
    }

    [[nodiscard]] Stmt *getElseBody() const {
        return elseBody;
    }
};

/**
 * 函数形参 FuncFParam → BType Ident ['[' ']' { '[' Exp ']' }]
 */
class FuncFParam : public Locs {
private:
    int BType;

    std::string ident;

    std::vector<Exp *> *exps;

public:
    /**
     * 函数形参 FuncFParam
     * @param BType 参数类型
     * @param ident 参数名
     * @param exps 数组参数维度，不记录恒为空的的第一维
     * @param sourLocs _0:identLoc; _1:assignLoc; [_Loc、 ]_Loc、...
     */
    FuncFParam(int BType, const char *ident, std::vector<Exp *> *exps, std::vector<SourceLocation *> *sourLocs) :
            BType(BType), ident(ident), exps(exps), Locs(sourLocs) {};

    [[nodiscard]] int getBType() const {
        return BType;
    }

    [[nodiscard]] const std::string &getIdent() const {
        return ident;
    }

    [[nodiscard]] const std::vector<Exp *> *getExps() const {
        return exps;
    }
};

/**
 * 函数形参表 FuncFParams → FuncFParam { ',' FuncFParam }
 */
class FuncFParams {
private:
    std::vector<FuncFParam *> *funcFParams;

public:
    /**
     * 函数形参表 FuncFParams
     * @param funcFParams 形参
     */
    explicit FuncFParams(std::vector<FuncFParam *> *funcFParams) : funcFParams(funcFParams) {};

    [[nodiscard]] const std::vector<FuncFParam *> *getFuncFParams() const {
        return funcFParams;
    }
};

/**
 * 函数定义 FuncDef → funcType Ident '(' [FuncFParams] ')' Block
 */
class FuncDef : public Locs {
private:
    int funcType;

    std::string ident;

    FuncFParams *funcFParams;

    Block *block;

public:
    /**
     * 函数定义 FuncDef
     * @param funcType 返回类型
     * @param ident 函数名
     * @param funcFParams 函数形参表
     * @param block 函数代码块
     * @param sourLocs _0: funcTypeLoc, _1: identLoc, _2: (_Loc, _3: )_Loc
     */
    FuncDef(int funcType, const char *ident, FuncFParams *funcFParams, Block *block,
            std::vector<SourceLocation *> *sourLocs) :
            funcType(funcType), ident(ident), funcFParams(funcFParams), block(block), Locs(sourLocs) {};

    [[nodiscard]] int getFuncType() const {
        return funcType;
    }

    [[nodiscard]] const std::string &getIdent() const {
        return ident;
    }

    [[nodiscard]] const FuncFParams *getFuncFParams() const {
        return funcFParams;
    }

    [[nodiscard]] Block *getBlock() const {
        return block;
    }
};

/**
 * 变量初值 InitVal → Exp
 *                | '{' [ InitVal { ',' InitVal } ] '}'
 */
class InitVal : public Locs {
private:
    Exp *exp;

    std::vector<InitVal *> *initVals;

public:
    /**
     * 变量初值 InitVal 构造方法
     * @param Exp 常量值
     * @param initVals 子维
     * @param sourLocs 当前维度的 _0:'{', _1:'}' 位置
     */
    InitVal(Exp *exp, std::vector<InitVal *> *initVals, std::vector<SourceLocation *> *sourLocs) :
            exp(exp), initVals(initVals), Locs(sourLocs) {};

    [[nodiscard]] const Exp *getExp() const {
        return exp;
    }

    [[nodiscard]] const std::vector<InitVal *> *getInitVals() const {
        return initVals;
    }
};

/**
 * 变量定义 VarDef → Ident { '[' ConstExp ']' }
 *               | Ident { '[' ConstExp ']' } '=' InitVal
 */
class VarDef : public Locs {
private:
    std::string ident;

    std::vector<ConstExp *> *constExps;

    InitVal *initVal;

public:
    /**
     * 变量定义 VarDef 构造方法
     * @param ident 变量名
     * @param constExps 变量数组的各维度数，可为空
     * @param constInitVal 常量值，可为空
     * @param sourLocs _0:identLoc (; _1:assignLoc 可无)
     */
    VarDef(const char *ident, std::vector<ConstExp *> *constExps, InitVal *initVal,
           std::vector<SourceLocation *> *sourLocs) :
            ident(ident), constExps(constExps), initVal(initVal), Locs(sourLocs) {};

    [[nodiscard]] const std::string &getIdent() const {
        return ident;
    }

    [[nodiscard]] const std::vector<ConstExp *> *getConstExps() const {
        return constExps;
    }

    [[nodiscard]] InitVal *getInitVal() const {
        return initVal;
    }
};

/**
 * 变量声明 VarDecl → BType VarDef { ',' VarDef } ';'
 */
class VarDecl {
private:
    int BType;

    std::vector<VarDef *> *varDefs;

public:
    /**
     * 变量声明 VarDecl 构造函数
     * @param BType 变量基本数据类型
     * @param constDefs 变量定义，可为多个
     */
    VarDecl(int BType, std::vector<VarDef *> *varDefs) : BType(BType), varDefs(varDefs) {};

    [[nodiscard]] int getBType() const {
        return BType;
    }

    [[nodiscard]] const std::vector<VarDef *> *getVarDefs() const {
        return varDefs;
    }
};

/**
 * 常量初值 ConstInitVal → ConstExp
|                       '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
 */
class ConstInitVal : public Locs {
private:
    ConstExp *constExp;

    std::vector<ConstInitVal *> *constInitVals;

public:
    /**
     * 常量初值 ConstInitVal构造方法
     * @param constExp 常量值
     * @param constInitVals 子维
     * @param sourLocs 当前维度的 _0: '{', _1: '}' 位置
     */
    ConstInitVal(ConstExp *constExp, std::vector<ConstInitVal *> *constInitVals,
                 std::vector<SourceLocation *> *sourLocs) :
            constExp(constExp), constInitVals(constInitVals), Locs(sourLocs) {};

    [[nodiscard]] ConstExp *getConstExp() const {
        return constExp;
    }

    [[nodiscard]] const std::vector<ConstInitVal *> *getConstInitVals() const {
        return constInitVals;
    }
};

/**
 * 常数定义 ConstDef → Ident { '[' ConstExp ']' } '=' ConstInitVal
 */
class ConstDef : public Locs {
private:
    std::string ident;

    std::vector<ConstExp *> *constExps;

    ConstInitVal *constInitVal;

public:
    /**
     * 常数定义 ConstDef 构造方法
     * @param ident 常量名
     * @param constExps 数组常量的各维度数，可为空
     * @param constInitVal 常量值，不可为空
     * @param sourLocs _0:identLoc; _1:assignLoc; [_Loc、 ]_Loc、...
     */
    ConstDef(const char *ident, std::vector<ConstExp *> *constExps, ConstInitVal *constInitVal,
             std::vector<SourceLocation *> *sourLocs) :
            ident(ident), constExps(constExps), constInitVal(constInitVal), Locs(sourLocs) {};

    [[nodiscard]] const std::string &getIdent() const {
        return ident;
    }

    [[nodiscard]] const std::vector<ConstExp *> *getConstExps() const {
        return constExps;
    }

    [[nodiscard]]  ConstInitVal *getConstInitVal() const {
        return constInitVal;
    }
};

/**
 * 常量声明 ConstDecl -> 'const' BType ConstDef { ',' ConstDef } ';'
 */
class ConstDecl : public Locs {
private:
    int BType;

    std::vector<ConstDef *> *constDefs;

public:
    /**
     * 常量声明 ConstDecl构造函数
     * @param BType 常量基本数据类型
     * @param constDefs 常量定义，可为多个
     * @param sourLocs _0: constLoc; _1: typeLoc
     */
    ConstDecl(int BType, std::vector<ConstDef *> *constDefs, std::vector<SourceLocation *> *sourLocs) :
            BType(BType), constDefs(constDefs), Locs(sourLocs) {};

    [[nodiscard]] int getBType() const {
        return BType;
    }

    [[nodiscard]] const std::vector<ConstDef *> *getConstDefs() const {
        return constDefs;
    }
};

/**
 * 声明 Decl -> ConstDecl | VarDecl | FuncDef
 */
class Decl {
private:
    ConstDecl *constDecl;
    VarDecl *varDecl;
    FuncDef *funcDef;

public:
    Decl(ConstDecl *constDecl, VarDecl *varDecl, FuncDef *funcDef) :
            constDecl(constDecl), varDecl(varDecl), funcDef(funcDef) {};

    [[nodiscard]] ConstDecl *getConstDecl() const {
        return constDecl;
    }

    [[nodiscard]] VarDecl *getVarDecl() const {
        return varDecl;
    }

    [[nodiscard]] FuncDef *getFuncDef() const {
        return funcDef;
    }
};

/**
 * 语法分析抽象语法树 AST -> {Decl}
 */
class AST {
private:
    std::vector<Decl *> *decls;

public:
    /**
     * 语法分析抽象语法树 AST -> {Decl}
     * @param decls 全局变量/函数
     */
    explicit AST(std::vector<Decl *> *decls) : decls(decls) {};

    [[nodiscard]] const std::vector<Decl *> *getDecls() const {
        return decls;
    }
};


#endif //COMPILER_AST_H
