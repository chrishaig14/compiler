//
// Created by chris on 6/4/21.
//

#ifndef XLANG_PACKAGE_H
#define XLANG_PACKAGE_H

#include "Module.h"
#include <common/Path.h>
#include "../packageunits/package_units.h"

class Package {
public:
    const std::string name;
    const std::string abs_path;
    const bool is_lib;
    const Path path;

    std::map<std::string, std::unique_ptr<Unit>> units;

    std::string rel_path;
    Package(Path path, std::string abs_path, bool is_lib);

    ModuleMember* get(Path p);
    std::vector<std::unique_ptr<Module>> modules;
    std::vector<std::unique_ptr<Package>> subpackages;
};


#endif //XLANG_PACKAGE_H
