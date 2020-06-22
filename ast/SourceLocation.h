#ifndef COMPILER_SOURCELOCATION_H
#define COMPILER_SOURCELOCATION_H


class SourceLocation {
    unsigned int line{};
    unsigned int col{};
public:
    SourceLocation(unsigned int line, unsigned int col) :
            line(line), col(col) {};

    SourceLocation() = default;;

    unsigned int getLine() const { return line; }

    unsigned int getCol() const { return col; }

};


#endif //COMPILER_SOURCELOCATION_H
