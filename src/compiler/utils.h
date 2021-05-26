//
// Created by chris on 25/5/21.
//

#ifndef XLANG_UTILS_H
#define XLANG_UTILS_H

#include <string>
#include <map>

std::map<std::string, std::string> read_requirements(const std::string& filepath);

void parse_args(int argc, char* argv[], bool& is_lib, std::string& project_dir, std::string& project_output_dir,
                std::string& global_name);


void init(int argc, char** argv, bool& is_lib, std::string& project_dir, std::string& project_output_dir,
          std::string& global_name);
#endif //XLANG_UTILS_H
