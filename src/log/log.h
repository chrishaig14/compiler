//
// Created by chris on 30/10/21.
//

#ifndef XLANG_LOG_H
#define XLANG_LOG_H

#include <string>
#include <iostream>
#include <map>
#include <memory>

#define STRINGIZING(x) #x
#define STR(x) STRINGIZING(x)
#define FILE_LINE __FILE__ ":" STR(__LINE__)

#define LOG_INFO(logger, msg) (Logger::get(logger)).info((msg))
#define LOG_DEBUG(logger, msg) (Logger::get(logger)).debug(std::string(FILE_LINE) + " " + (msg))


class Logger {

    static std::map<std::string, std::unique_ptr<Logger>> instances;

    Logger();

public:
    const std::string name;
    bool log_info;
    bool log_debug;

    static Logger& get(const std::string& name);

    void info(const std::string& msg);

    void debug(const std::string& msg);
};

#endif //XLANG_LOG_H
