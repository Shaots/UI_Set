#include "../include/ILogger.h"
#include "myLogger.h"

ILogger* ILogger::createLogger() {
    myLogger* my = new myLogger();
    return my;
}

ILogger* ILogger::createLogger(const char* const& filename, bool overwrite) {
    ofstream* ofs;
    if (overwrite) {
        ofs = new ofstream(filename);
    } else {
        ofs = new ofstream (filename, ofstream::ate);
    }
    myLogger* my = new myLogger(ofs);
    return my;
}

ILogger::~ILogger() = default;
