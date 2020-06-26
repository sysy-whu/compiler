#ifndef COMPILER_IRLOCALSTMT_H
#define COMPILER_IRLOCALSTMT_H


#include <string>
#include <vector>

/**
 * IRLocalStmt语句类
 */
class IRLocalStmt {
private:
    int opt;
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
     *
     * alloca_i32       opd1;
     * con_alloca_i32   opd1;
     * global_i32       opd1;
     * con_global_i32   opd1;
     *
     * @param opd1 操作数
     * @Param line 行号
     * @param row  源代码位置行号
     * @param col  源代码位置列号
     */
    IRLocalStmt(int opt, const char *opd1, unsigned int line, unsigned int row,
                unsigned int col) : opt(opt), opd1(opd1), line(line), row(row), col(col) {};

    /**
     *  IRLocalStmt语句类构造方法，只有一个操作符，两个操作数的情况
     * @param opt 操作符类型:
     * load                opd2(被取)       opd1;
     * store               opd2(被存)       opd1;
     *
     * alloca_i32*         opd2(维数)       opd1;
     * con_alloca_i32*     opd2(维数)       opd1;
     * global_i32*         opd2(维数)       opd1;
     * con_global_i32*     opd2(维数)       opd1;
     * getPtr              opd2(维数索引)    opd1;
     *
     *     维数示例： [4 x [2 x i32]] (变量)
     *              i32* | [2 x i32]* (参数)
     *     维数索引： [0][1][2]
     *
     * @param opd1
     * @param opd2
     * @Param line 行号
     * @param row  源代码位置行号
     * @param col  源代码位置列号
     */
    IRLocalStmt(int opt, const char *opd1, const char *opd2, unsigned int line,
                unsigned int row, unsigned int col) :
            opt(opt), opd1(opd1), opd2(opd2), line(line), row(row), col(col) {};

    /**
     *  IRLocalStmt语句类构造方法，只有一个操作符，三个操作数的情况
     * @param opt 操作符类型
     *
     * opType(算术关系逻辑)  opd2(操作数)   opd3(操作数)        opd1;
     * br(条件跳转)         opd1(条件)    opd2(opd1=1)       opd3;
     *
     * @Param line 行号
     * @param row  源代码位置行号
     * @param col  源代码位置列号
     */
    IRLocalStmt(int opt, const char *opd1, const char *opd2, const char *opd3,
                unsigned int line, unsigned int row, unsigned int col) :
            opt(opt), opd1(opd1), opd2(opd2), opd3(opd3), line(line), row(row), col(col) {};

    // 操作符&操作数
    [[nodiscard]] int getOpt() const { return opt; }

    std::string getOpd1() { return opd1; }

    std::string getOpd2() { return opd2; }

    std::string getOpd3() { return opd3; }

    // 行列号
    [[nodiscard]] unsigned int getLine() const { return line; }

    [[nodiscard]] unsigned int getRow() const { return row; }

    [[nodiscard]] unsigned int getColumn() const { return col; }

    void setLine(unsigned int line_) { this->line = line_; }
};

#endif //COMPILER_IRLOCALSTMT_H
