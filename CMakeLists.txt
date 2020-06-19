cmake_minimum_required(VERSION 3.15)
project(compiler)

set(CMAKE_CXX_STANDARD 17)

add_executable(compiler main.cpp util/Error.h util/MyConstants.h util/Util.cpp util/Util.h lexer/Token.h lexer/Lex.cpp lexer/Lex.h)