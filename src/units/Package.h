//
// Created by chris on 6/4/21.
//

#ifndef XLANG_PACKAGE_H
#define XLANG_PACKAGE_H

#include "Module.h"

class Package : public Entity {
public:
    std::string name;
    std::string path;
    std::map<std::string, Unit> units;

    Package(std::string name, std::string path);
};


#endif //XLANG_PACKAGE_H
