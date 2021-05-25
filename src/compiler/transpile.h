//
// Created by chris on 25/5/21.
//

#ifndef XLANG_TRANSPILE_H
#define XLANG_TRANSPILE_H


#include "../units/Package.h"
#include "../utils.h"
#include "../transpiler/STranspiler.h"

void transpile_all_modules(Package& package, const std::string& output_dir, bool is_top, const std::string& global_name,
                           std::string static_initializations, std::string& static_cleanups);
void transpile_one_module(Module& module, std::string& package_header, const std::string& output_package_dir,
                          Package& package, std::string static_initializations, std::string& static_cleanups);

#endif //XLANG_TRANSPILE_H
