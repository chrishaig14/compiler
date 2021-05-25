//
// Created by chris on 25/5/21.
//

#ifndef XLANG_ANALYZE_H
#define XLANG_ANALYZE_H

#include "../units/Module.h"
#include "../semantic/Checker.h"

void process_global_all_modules(Package& package);

void add_local_path_to_module(Module& module, Path path, Package* top_package);
void add_global_path_to_module(Module& module, Path path);
void add_path_to_module(Module& module, Path path, Package* top_package);
void add_path_with_alias_to_module(Module& module, const std::string& alias, Path path, Package* root_package);

void analyze_module(Module& module, Package* top_package);
void analyze_all_modules(Package& package, Package* top_package);

#endif //XLANG_ANALYZE_H
