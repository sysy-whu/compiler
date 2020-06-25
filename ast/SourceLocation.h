#ifndef COMPILER_SOURCELOCATION_H
#define COMPILER_SOURCELOCATION_H

/**
 * 单个位置类 SourceLocation
 */
class SourceLocation {
    unsigned int line;
    unsigned int col;
public:
    /**
     * 单个位置类 构造函数
     * @param line 行
     * @param col 列
     */
    SourceLocation(const unsigned int line, const unsigned int col) : line(line), col(col) {};

    unsigned int getLine() const {
        return line;
    }

    unsigned int getCol() const {
        return col;
    }
};

/**
 * 位置类 Locs 多<vector>类型成员变量方式
 */
class Locs {
protected:
    std::vector<SourceLocation *> *locs;

public:
    /**
     * 位置类 Locs
     * @param locs 行列位置
     */
    explicit Locs(std::vector<SourceLocation *> *locs) : locs(locs) {};

    std::vector<SourceLocation *> *getLocs() { return locs; }

};


#endif //COMPILER_SOURCELOCATION_H
