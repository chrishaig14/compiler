//
// Created by chris on 2/8/20.
//

#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <unordered_map>
#include <algorithm>
#include "types.h"
#include "instructions/Instruction.h"
#include <unistd.h>
#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <sstream>
#include <fstream>

std::ostream& operator<<(std::ostream& out, const CodeLabel v);

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
