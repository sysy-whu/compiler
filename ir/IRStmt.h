#ifndef COMPILER_IRSTMT_H
#define COMPILER_IRSTMT_H
/**
 * 详细说明见 SysY2020-WHU_中间码设计.docx
 */
#include <string>
#include <vector>

/**
 * IRLocalStmt语句类
 */
class IRStmt {
private:
    int opt;

    std::string opd1;

    std::string opd2;

    std::vector<std::string> *opd3;

public:
    IRStmt(int opt, const char *opd1) : opt(opt), opd1(opd1) {};

    IRStmt(int opt, const char *opd1, const char *opd2) : opt(opt), opd1(opd1), opd2(opd2) {};

    IRStmt(int opt, const char *opd1, const char *opd2, std::vector<std::string> *opd3) :
            opt(opt), opd1(opd1), opd2(opd2), opd3(opd3) {};

    // 操作符&操作数
    [[nodiscard]] int getOpt() const { return opt; }

    std::string getOpd1() { return opd1; }

    std::string getOpd2() { return opd2; }

    std::vector<std::string> *getOpd3() { return opd3; }
};

#endif //COMPILER_IRSTMT_H
