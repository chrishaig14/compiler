//
// Created by chris on 6/4/21.
//

#ifndef XLANG_PACKAGE_H
#define XLANG_PACKAGE_H

#include "Module.h"
#include "../ast/Path.h"

class Package {
public:
    const std::string name;
    const std::string abs_path;
    const bool is_lib;
    const Path path;

    std::string rel_path;
    std::string full_header_path;
    std::string header_parent_path;

    std::map<std::string, Unit> units;
    Package(Path path, std::string abs_path, std::string rel_path, bool is_lib, std::string full_header_path,
            std::string header_parent_path);

    Flirpin get(Path p);
};


#endif //XLANG_PACKAGE_H
