//
// Created by chris on 6/4/21.
//

#ifndef XLANG_PACKAGE_H
#define XLANG_PACKAGE_H

#include "Module.h"

class Package {
public:
    std::string name;
    std::string abs_path;
    std::string rel_path;
    std::map<std::string, Unit> units;

    Package(std::string name, std::string abs_path, std::string rel_path);
    Path path;

    Flirpin get(Path p) {
        VectorOfStrings pt = p.as_vec();
        Unit u = this->units[pt[0]];
        if (u.type == U_TYPE::MODULE) {
            return u.module->get(Path(VectorOfStrings(pt.begin() + 1, pt.end())));
        } else if (u.type == U_TYPE::PACKAGE) {
            return u.package->get(Path(VectorOfStrings(pt.begin() + 1, pt.end())));
        }
    }
};


#endif //XLANG_PACKAGE_H
