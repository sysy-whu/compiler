#include "Util.h"
//#include <io.h>
#include <fstream>
#include "Error.h"
//#include "MyConstants.h"

int Util::ifO2 = 0;

char *Util::inputUrl = nullptr;

char *Util::outputUrl = nullptr;

int Util::handleParams(int argc, char **argv) {
    if (argc != 5 && argc != 6) {
        Error::errorParam();
        return PARAMS_NUMBER_WRONG;
    }
    if (*argv[1] != *PARAMS_S || *argv[2] != *PARAMS_O) {
//        std::cout << argv[1] << "\t" << argv[2] << std::endl;
        Error::errorParam();
        return PARAMS_WORD_WRONG;
    }
    if (argc == 6) {
        if (*argv[5] == *PARAMS_O1) {
            setIfO2(O1_TRUE);
        } else {
            Error::errorParam();
            return PARAMS_WORD_WRONG;
        }
    }
    std::fstream _file;
    _file.open(argv[4], std::ios::in);
    if(!_file){
        Error::errorInputFile();
        return PARAMS_FILE_WRONG;
    }else{
        _file.close();
        std::cout << argv[3] << "\t" << argv[4] << std::endl;
        std::cout << outputUrl << "\t" << inputUrl << std::endl;
        setInputUrl(argv[4]);
        setOutputUrl(argv[3]);
        return PARAMS_PASS;
    }
    exit(-1);

    std::cout << argv[3] << "\t" << argv[4] << std::endl;
//    if (_access(argv[4], 0) == 0 && _access(argv[4], 2) == 0) {
//        setInputUrl(argv[4]);
//        setOutputUrl(argv[3]);

//        return PARAMS_PASS;
//    } else {
//        Error::errorInputFile();
//        return PARAMS_FILE_WRONG;
//    }
}
