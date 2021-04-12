//
// Created by chris on 6/4/21.
//

#ifndef XLANG_PACKAGE_H
#define XLANG_PACKAGE_H

#include "Module.h"

class Package : public Entity {
public:
    std::string name;
    std::string abs_path;
    std::string rel_path;
    std::map<std::string, Unit> units;

    Package(std::string name, std::string abs_path, std::string rel_path);
    std::string full_path;
};


#endif //XLANG_PACKAGE_H
