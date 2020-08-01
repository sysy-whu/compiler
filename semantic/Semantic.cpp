#include <iostream>
#include "Semantic.h"

Semantic::Semantic() {
    Parse parse;
    parse.parseAST();
    ast = parse.getAST();
    std::cout << ast->getDecls()->size() << std::endl;
    symbolTables = new std::vector<SymbolTable *>();
}

void Semantic::startSemantic() {
    /// 在这里无脑设置 sylib.h 声明函数
    setSyLib_H();
    std::cout << "after setSyLib_H; begin semantic" << std::endl;


}

///===-----------------------------------------------------------------------===///
/// 静态库文件里面的函数
///===-----------------------------------------------------------------------===///
void Semantic::setSyLib_H() {
    auto *symbolsLib = new std::vector<Symbol *>();
    std::string avoidNULL = "nullptr";

    std::string getIntStr = "getint";
    // int getint()
    auto *funcGetint = new Arm7Func(getIntStr.c_str(), TYPE_INT, nullptr);
    auto *symbolGetInt = new Symbol(nullptr, funcGetint);
    symbolsLib->emplace_back(symbolGetInt);

    std::string getChStr = "getch";
    // int getch()
    auto *funcGetCh = new Arm7Func(getChStr.c_str(), TYPE_INT, nullptr);
    auto *symbolGetCh = new Symbol(nullptr, funcGetCh);
    symbolsLib->emplace_back(symbolGetCh);

    std::string getArrayStr = "getarray";
    auto *getArrayParams = new std::vector<Arm7Var *>();
    // int[]
    auto *getArrayVarArraySubs = new std::vector<int>();
    auto *getArrayVarArrayParam = new Arm7Var(avoidNULL.c_str(), getArrayStr.c_str(), TYPE_INT, 1, 0, 1,
                                              getArrayVarArraySubs,
                                              nullptr);
    getArrayParams->emplace_back(getArrayVarArrayParam);
    // int getarray(int[])
    auto *funcGetArray = new Arm7Func(getArrayStr.c_str(), TYPE_INT, getArrayParams);
    auto *symbolGetArray = new Symbol(nullptr, funcGetArray);
    symbolsLib->emplace_back(symbolGetArray);

    std::string putIntStr = "putint";
    auto *putIntParams = new std::vector<Arm7Var *>();
    // int
    auto *putIntVarParam = new Arm7Var(avoidNULL.c_str(), putIntStr.c_str(), TYPE_INT, 1, 0, 0, nullptr, nullptr);
    putIntParams->emplace_back(putIntVarParam);
    // void putint(int)
    auto *funcPutInt = new Arm7Func(putIntStr.c_str(), TYPE_VOID, putIntParams);
    auto *symbolPutInt = new Symbol(nullptr, funcPutInt);
    symbolsLib->emplace_back(symbolPutInt);

    std::string putChStr = "putch";
    auto *putChParams = new std::vector<Arm7Var *>();
    // int
    auto *putChVarParam = new Arm7Var(avoidNULL.c_str(), putChStr.c_str(), TYPE_INT, 1, 0, 0, nullptr, nullptr);
    putChParams->emplace_back(putChVarParam);
    // void putch(int)
    auto *funcPutCh = new Arm7Func(putChStr.c_str(), TYPE_VOID, putChParams);
    auto *symbolPutCh = new Symbol(nullptr, funcPutCh);
    symbolsLib->emplace_back(symbolPutCh);

    std::string putArrayStr = "putarray";
    auto *putArrayParams = new std::vector<Arm7Var *>();
    // int
    auto *putArrayVarParam = new Arm7Var(avoidNULL.c_str(), putArrayStr.c_str(), TYPE_INT, 1, 0, 0, nullptr, nullptr);
    putArrayParams->emplace_back(putArrayVarParam);
    // int[]
    auto *putArrayVarArraySubs = new std::vector<int>();
    auto *putArrayVarArrayParam = new Arm7Var(avoidNULL.c_str(), putArrayStr.c_str(), TYPE_INT, 1, 0, 1,
                                              putArrayVarArraySubs,
                                              nullptr);
    putArrayParams->emplace_back(putArrayVarArrayParam);
    // void putarray(int, int[])
    auto *funcPutArray = new Arm7Func(putArrayStr.c_str(), TYPE_VOID, putArrayParams);
    auto *symbolPutArray = new Symbol(nullptr, funcPutArray);
    symbolsLib->emplace_back(symbolPutArray);

    auto *putFParams = new std::vector<Arm7Var *>();
    // <格式串>
    auto *putFStrParam = new Arm7Var(avoidNULL.c_str(), putFStr.c_str(), TYPE_STR, 1, 0, 0, nullptr, nullptr);
    putFParams->emplace_back(putFStrParam);
    // int... jump -> 写死判断
    // void putf(<格式串>, int, …)
    auto *funcPutF = new Arm7Func(putFStr.c_str(), TYPE_VOID, putFParams);
    auto *symbolPutF = new Symbol(nullptr, funcPutF);
    symbolsLib->emplace_back(symbolPutF);

    std::string startTimeStr = "starttime";
    auto *startTimeParams = new std::vector<Arm7Var *>();
    // void starttime()
    auto *funcStartTime = new Arm7Func(startTimeStr.c_str(), TYPE_VOID, startTimeParams);
    auto *symbolStartTime = new Symbol(nullptr, funcStartTime);
    symbolsLib->emplace_back(symbolStartTime);

    std::string stopTimeStr = "stoptime";
    auto *stopTimeParams = new std::vector<Arm7Var *>();
    // void stoptime()
    auto *funcStopTime = new Arm7Func(stopTimeStr.c_str(), TYPE_VOID, stopTimeParams);
    auto *symbolStopTime = new Symbol(nullptr, funcStopTime);
    symbolsLib->emplace_back(symbolStopTime);

    auto *symbolLibTable = new SymbolTable(SYMBOL_TABLE_EXTERN, symbolsLib);
    symbolTables->push_back(symbolLibTable);
}
