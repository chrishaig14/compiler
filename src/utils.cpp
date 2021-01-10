//
// Created by chris on 5/9/20.
//

#include "utils.h"
#include <iostream>

std::ostream& operator<<(std::ostream& out, CodeLabel v) {
    for (auto i: v) {
        std::string spaces;
        for (int k = i.first.size(); k < 32; k++) {
            spaces += " ";
        }
        out << i.first + spaces + i.second->to_string() << std::endl;
    }
    return out;
}

std::string u_dirname(const std::string& path) {
    char* p = new char[path.size() + 1];
    strcpy(p, path.c_str());
    std::string r = dirname(p);
    delete[] p;
    return r;
}

std::string u_basename(const std::string& path) {
    char* p = new char[path.size() + 1];
    strcpy(p, path.c_str());
    std::string r = basename(p);
    delete[] p;
    return r;
}

std::string path_join(const std::string& base, const std::string& p) {
    return base + "/" + p;
}

bool file_exists(const std::string& path) {
    return access(path.c_str(), F_OK) == 0;
}

bool is_file(const std::string& path) {
    struct stat path_stat;
    stat(path.c_str(), &path_stat);
    return S_ISREG(path_stat.st_mode);
}

std::string file_to_string(const std::string& path) {
    std::string filename = path;
    std::ifstream file(filename);
    std::stringstream sstream;
    sstream << file.rdbuf();
    std::string text = sstream.str();
    return text;
}

std::string module_from_path(const std::string& path) {
    std::string s = u_basename(path);
    s = s.substr(0, s.size() - 3);
    return s;
}

bool operator==(CodeLabel a, CodeLabel b) {
    if (a.size() != b.size()) {
        int min = a.size() < b.size() ? a.size() : b.size();
        for (int i = 0; i < min; i++) {
            auto a_inst = a[i].second;
            auto b_inst = b[i].second;
            std::cerr << "comparing " << a_inst->to_string() << " == " << b_inst->to_string() << ": "
                      << (a_inst->equal(b_inst) ? "True" : "False") << std::endl;
        }
        return false;
    }
    for (int i = 0; i < a.size(); i++) {
        auto a_inst = a[i].second;
        auto b_inst = b[i].second;
        std::cerr << "comparing " << a_inst->to_string() << " == " << b_inst->to_string() << ": "
                  << (a_inst->equal(b_inst) ? "True" : "False") << std::endl;
        if (!a_inst->equal(b_inst)) {
            return false;
        }
    }
    return true;
}

VariableLocation::VariableLocation(int depth, int index) {
    this->depth = depth;
    this->index = index;
}

VariableLocation::VariableLocation(const VariableLocation& other) {
    this->depth = other.depth;
    this->index = other.index;
}

VariableLocation::VariableLocation() {
    this->depth = -1;
    this->index = -1;
}
