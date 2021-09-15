//
// Created by chris on 6/4/21.
//

#include <cassert>
#include "Package.h"

Package::Package(Path path, std::string abs_path, bool is_lib)
        : name(path.as_vec().back()), abs_path(abs_path), is_lib(is_lib), path(path) {
    // this->path = path;
    // this->name = path.as_vec().back();
    // this->abs_path = abs_path;
    // this->rel_path = rel_path;
    // this->is_lib = is_lib;
}

ModuleMember Package::get(Path p) {
    if (p.as_str() == "core.core.Union") {
        return ModuleMember{.type=ModuleMemberType::CLASS, .clazz=new Class("Union", p)};
    }
    VectorOfStrings pt = p.as_vec();
    assert(this->units.count(pt[0]) == 1);
    Unit u = this->units.at(pt[0]);
    switch (u.type) {
        case U_TYPE::MODULE:
            return u.module->get(Path(VectorOfStrings(pt.begin() + 1, pt.end())));
        case U_TYPE::PACKAGE:
            return u.package->get(Path(VectorOfStrings(pt.begin() + 1, pt.end())));
    }
    return ModuleMember{};
}
