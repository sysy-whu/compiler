#ifndef COMPILER_IRLOCALSTMT_H
#define COMPILER_IRLOCALSTMT_H


#include <string>
#include <vector>

/**
 * IRLocalStmt语句类
 */
class IRLocalStmt {
private:
    std::string opt;
    std::string opd1;
    std::string opd2;
    std::string opd3;

    unsigned int line;
    unsigned int row;
    unsigned int col;

public:
    /**
     * IRLocalStmt语句类构造方法，只有一个操作符，一个操作数的情况
     * @param opt 操作符类型:
     * br               opd1;
     * ret              opd1;
     * alloca_i32       opd1;
     * con_alloca_i32   opd1;
     *
     * @param opd1 操作数
     * @Param line 行号
     * @param row  源代码位置行号
     * @param col  源代码位置列号
     */
    IRLocalStmt(std::string opt, std::string opd1, unsigned int line, unsigned int row, unsigned int col) :
            opt(std::move(opt)), opd1(std::move(opd1)), line(line), row(row), col(col) {};

    /**
     *  IRLocalStmt语句类构造方法，只有一个操作符，两个操作数的情况
     * @param opt 操作符类型:
     * load               opd2(被取)       opd1;
     * store              opd2(被存)       opd1;
     * alloca_i32*        opd2(维数)       opd1;
     * con_alloca_i32*    opd2(维数)       opd1;
     * dso_global_i32     opd2(initValue) opd1;
     * con_dso_global_i32 opd2(initValue) opd1;
     *
     * @param opd1
     * @param opd2
     * @Param line 行号
     * @param row  源代码位置行号
     * @param col  源代码位置列号
     */
    IRLocalStmt(std::string opt, std::string opd1, std::string opd2, unsigned int line, unsigned int row,
                unsigned int col) : opt(std::move(opt)), opd1(std::move(opd1)), opd2(std::move(opd2)), line(line),
                                    row(row), col(col) {};

    /**
     *  IRLocalStmt语句类构造方法，只有一个操作符，三个操作数的情况
     * @param opt 操作符类型
     * dso_global_i32*     opd2(维数)    opd3(initValue)    opd1;
     * con_dso_global_i32* opd2(维数)    opd3(initValue)    opd1;
     *
     * opType(算术关系逻辑)  opd2(操作数)   opd3(操作数)        opd1;
     * br(条件跳转)         opd1(条件)    opd2(opd1=1)       opd3;
     * @Param line 行号
     * @param row  源代码位置行号
     * @param col  源代码位置列号
     */
    IRLocalStmt(std::string opt, std::string opd1, std::string opd2, std::string opd3, unsigned int line,
                unsigned int row, unsigned int col) :
            opt(std::move(opt)), opd1(std::move(opd1)), opd2(std::move(opd2)),
            opd3(std::move(opd3)), line(line), row(row), col(col) {};

    // 操作符&操作数
    std::string getOpt() { return opt; }

    std::string getOpd1() { return opd1; }

    std::string getOpd2() { return opd2; }

    std::string getOpd3() { return opd3; }

//    void setOpt(std::string opt) { this->opt = std::move(opt); }

//    void setOpd1(std::string opd1) { this->opd1 = std::move(opd1); }

//    void setOpd2(std::string opd2) { this->opd2 = std::move(opd2); }

//    void setOpd3(std::string opd3) { this->opd3 = std::move(opd3); }

    // 行列号
    unsigned int getLine() const { return line; }

    unsigned int getRow() const { return row; }

    unsigned int getColumn() const { return col; }

    void setLine(unsigned int line) { this->line = line; }

//    void setRow(unsigned int row) { this->row = row; }

//    void setColumn(unsigned int column) { this->col = column; }

};

#endif //COMPILER_IRLOCALSTMT_H
