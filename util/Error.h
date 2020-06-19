/********************************************
* Error.h                              *
* Created by lugf027 on 2020/6/19.            *
* Github: https://github.com/lugf027        *
*********************************************/

#ifndef COMPILER_ERROR_H
#define COMPILER_ERROR_H

#include <iostream>

class Error {
public:
    static void errorParam() { std::cout << "invalid params input" << std::endl; }

    static void errorInputFile() { std::cout << "can't access target input file" << std::endl; }

    static void errorOpenFile() { std::cout << "Error when open target file" << std::endl; }

    static void errorOpenFile(char *filename) { std::cout << "Error when open:" << filename << std::endl; }

    static void errorChar(char ch, int row, int column) {
        std::cout << "Unreadable char:" << ch << " at line: " << row << " column: " << column << std::endl;
    }
};

#endif //COMPILER_ERROR_H
