/********************************************
* Util.h                              *
* Created by lugf027 on 2020/6/19.            *
* Github: https://github.com/lugf027        *
*********************************************/

#ifndef COMPILER_UTIL_H
#define COMPILER_UTIL_H


class Util {
private:
    static char *inputUrl;

    static char *outputUrl;

    static int ifO2;

public:
    static char *getInputUrl() { return inputUrl; }

    static void setInputUrl(char *url) { inputUrl = url; }

    static char *getOutputUrl() { return outputUrl; }

    static void setOutputUrl(char *url) { outputUrl = url; }

    static int getIfO2() { return ifO2; }

    static void setIfO2(int o2) { ifO2 = o2; }

    /**
     * handle params from console
     * @param argc: number of params
     * @param argv: array type of params
     * @return some state in MyConstants.h
     */
    static int handleParams(int argc, char **argv);

};

#endif //COMPILER_UTIL_H
