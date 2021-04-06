//
// Created by chris on 5/9/20.
//

#include "utils.h"
#include <iostream>

std::string u_dirname(const std::string& path) {
    char* p = new char[path.size() + 1];
    strcpy(p, path.c_str());
    std::string r = dirname(p);
    delete[] p;
    return r;
}

std::string mangle_function_name(std::string module_name, std::string name) {
    return "m_" + module_name + "_f_" + name;
}

std::string mangle_class_name(std::string module_name, std::string name) {
    return "m_" + module_name + "_c_" + name;
}

std::string mangle_method_name(std::string module_name, std::string class_name, std::string method_name) {
    return "m_" + module_name + "_c_" + class_name + "_f_" + method_name;
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

std::string mangle_path(std::string path) {
    std::string out;
    for (int i = 0; i < path.size(); i++) {
        if (path[i] == '.') {
            out += "_D_";
        }else{
            out += path[i];
        }
    }
    return out;
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
