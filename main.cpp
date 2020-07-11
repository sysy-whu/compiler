#include <iostream>
#include <vector>
#include "util/Util.h"
#include "util/MyConstants.h"
#include "syTestFiles/LexTest.h"
#include "parser/Parse.h"
#include "semantic/Semantic.h"

std::string test(std::vector<std::string> &v) {
  std::string a = "xzc";
  v.push_back(a);
  return "zxc";
}

// Run->Edit Configurations->Program arguments
// -S -o <OutputFilepath> <InputFilepath> [-O1]
int main(int argc, char **argv) {
  // if (Util::handleParams(argc, argv) != PARAMS_PASS) {
  //     std::cout << "Error Program arguments" << std::endl;
  //     return -1;
  // }
  // Semantic semantic;
  // semantic.startSemantic();

  // // 移步 semantic 构造方法
  // //    Parse parse;
  // //    parse.parseAST();

  // // 移步 parse 构造方法
  // //    Lex lex;
  // //    test(lex);
  std::vector<std::string> v;
  test(v);
  for (auto &it:v) {
    std::cout << it;
  }

}
