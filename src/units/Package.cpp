//
// Created by chris on 6/4/21.
//

#include "Package.h"

Package::Package(Path path, std::string abs_path, std::string rel_path, bool is_lib, std::string full_header_path,
                 std::string header_parent_path)
        : full_header_path(full_header_path), header_parent_path(header_parent_path) {
    this->path = path;
    this->name = path.as_vec().back();
    this->abs_path = abs_path;
    this->rel_path = rel_path;
    this->is_lib = is_lib;
}

Flirpin Package::get(Path p) {
    if (p.as_str() == "core.core.Union") {
        return Flirpin{.type=F_TYPE::CLASS, .clazz=new Class()};
    }
    VectorOfStrings pt = p.as_vec();
    Unit u = this->units.at(pt[0]);
    if (u.type == U_TYPE::MODULE) {
        return u.module->get(Path(VectorOfStrings(pt.begin() + 1, pt.end())));
    } else if (u.type == U_TYPE::PACKAGE) {
        return u.package->get(Path(VectorOfStrings(pt.begin() + 1, pt.end())));
    }
}
