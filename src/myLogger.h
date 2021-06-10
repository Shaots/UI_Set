#ifndef UI_VECTOR_MYLOGGER_H
#define UI_VECTOR_MYLOGGER_H
#include <iostream>
#include <fstream>
#include "../include/ILogger.h"
using namespace std;
class myLogger : public ILogger {
public:
    myLogger();
    myLogger(ofstream* ofs);

public:
    RC log(RC code, Level level, const char* const& srcfile, const char* const& function, int line) override;

    RC log(RC code, Level level) override;

    static const char* GetErrorString(RC code);

    static const char* GetLevelString(Level level);

    ~myLogger() override;

private:
    ofstream* ofs;
};


#endif //UI_VECTOR_MYLOGGER_H
