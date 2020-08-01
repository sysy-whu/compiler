//
// Created by 77350 on 2020/8/1.
//

#ifndef COMPILER_ARMSTMT_H
#define COMPILER_ARMSTMT_H
#include "../util/MyConstants.h"

class ArmStmt{
private:
    // 操作类型
    ARM_TYPE opType;

    // arm指令操作数
    std::string opNum1;
    std::string opNum2;
    std::string opNum3;
private:
    // 把opType转换为字符串
    std::string opType2String();
public:
    ArmStmt(ARM_TYPE opType, const std::string &opNum1, const std::string &opNum2, const std::string &opNum3);

    ArmStmt(ARM_TYPE opType, const std::string &opNum1, const std::string &opNum2);

    ArmStmt(ARM_TYPE opType, const std::string &opNum1);

    ArmStmt(ARM_TYPE opType);

    ARM_TYPE getOpType() const;

    const std::string &getOpNum1() const;

    const std::string &getOpNum2() const;

    const std::string &getOpNum3() const;

    void setOpType(ARM_TYPE opType);

    void setOpNum1(const std::string &opNum1);

    void setOpNum2(const std::string &opNum2);

    void setOpNum3(const std::string &opNum3);
    // 把该ArmStmt打印输出
    std::string genString();
};

#endif //COMPILER_ARMSTMT_H
