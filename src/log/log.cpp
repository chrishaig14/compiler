//
// Created by chris on 30/10/21.
//

#include "log.h"

Logger& Logger::get(const std::string& name) {
    auto it = Logger::instances.find(name);
    if (it != Logger::instances.end()) {
        return *it->second;
    }
    Logger::instances[name] = std::unique_ptr<Logger>(new Logger());
    return *Logger::instances.at(name);
}

void Logger::info(const std::string& msg) {
    if (this->log_info) {
        std::cout << this->name << " [info] " << msg << std::endl;
    }
}

void Logger::debug(const std::string& msg) {
    if (this->log_debug) {
        std::cout << this->name << " [debug] " << msg << std::endl;
    }
}

Logger::Logger() {
    this->log_info = true;
    this->log_debug = false;
}

std::map<std::string, std::unique_ptr<Logger>> Logger::instances;