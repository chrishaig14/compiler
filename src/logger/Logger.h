//
// Created by chris on 1/4/21.
//

#ifndef XLANG_LOGGER_H
#define XLANG_LOGGER_H


#include <string>
#include <iostream>

namespace Logger {
    void info(std::string s) {
        std::cout << "[INFO] " << s << std::endl;
    }
};


#endif //XLANG_LOGGER_H
