//
// Created by chris on 2/8/20.
//

#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <unordered_map>
#include <algorithm>
#include "types.h"
#include <unistd.h>
#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <sstream>
#include <fstream>

std::string mangle_path(std::string path);

std::string mangle_function_name(std::string module_name, std::string name);

std::string mangle_class_name(std::string module_name, std::string name);

std::string mangle_method_name(std::string module_name, std::string class_name, std::string method_name);

class VariableLocation {
public:
    int depth;
    int index;

    VariableLocation();

    VariableLocation(int depth, int index);
    VariableLocation(const VariableLocation& other);
};

template<typename T>
bool item_in_vec(T item, std::vector<T> vec) {
    return std::find(vec.begin(), vec.end(), item) != vec.end();
}

std::string file_to_string(const std::string& path);

std::string module_from_path(const std::string& path);

bool is_file(const std::string& path);

bool file_exists(const std::string& path);

std::string path_join(const std::string& base, const std::string& p);

std::string u_basename(const std::string& path);

std::string u_dirname(const std::string& path);

#endif //UTILS_H
