cmake_minimum_required(VERSION 3.15)
project(compiler)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES
        *.h
        *.cpp
        util/*.h
        util/*.cpp
        lexer/*.h
        lexer/*.cpp
        ast/*.h
        ast/*.cpp
        parser/*.h
        parser/*.cpp
        semantic/*.h
        semantic/*.cpp
        ir/*.h
        ir/*.cpp
        syTestFiles/*.h
        syTestFiles/*.cpp
        )

add_executable(compiler main.cpp ${SOURCE_FILES} ${SOURCES} arm7/semantic/SymbolTable.h arm7/semantic/Semantic.cpp arm7/semantic/Semantic.h arm7/arm/ArmGlobal.h arm7/arm/ArmStmt.h arm7/arm/Arm7Gen.cpp arm7/arm/Arm7Gen.h arm7/arm/ArmRegManager.h arm7/arm/ArmRegManager.cpp)


