//
// Created by chris on 11/10/21.
//

#ifndef XLANG_PACKAGECHECKER_H
#define XLANG_PACKAGECHECKER_H

#include <string>
#include <fstream>
#include "../units/infos/Package.h"
#include "../units/infos/Module.h"
#include "../util/utils.h"
#include "../semantic/ModuleChecker.h"

std::unique_ptr<ModuleMember> find(Path path, Package& top_package);

void add_path_to_module(Module& module, Path path, Package& top_package);

void resolve_module_imports(Module& module, Package& top_package);

class PackageChecker {
    bool check_module(Module& module);

    bool check_unit(Unit& uvalue);

    Package& top_package;
public:
    explicit PackageChecker(Package& top_package);

    bool check_package(Package& package);
};


#endif //XLANG_PACKAGECHECKER_H
