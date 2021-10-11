//
// Created by chris on 6/4/21.
//

#include <cassert>
#include "Package.h"

Package::Package(Path path, std::string abs_path, bool is_lib)
        : name(path.as_vec().back()), abs_path(abs_path), is_lib(is_lib), path(path) {
    // this->path = path;
    // this->name = path..basname();
    // this->abs_path = abs_path;
    // this->rel_path = rel_path;
    // this->is_lib = is_lib;
}

ModuleMember* Package::get(Path p) {
    if (p.as_str() == "libcore.libcore.Union") {
        return new ClassModuleMember(*new ConcreteClass("Union", p));
    }
    VectorOfStrings pt = p.as_vec();
    assert(this->units.count(pt[0]) == 1);
    Unit* u = this->units.at(pt[0]).get();
    if (u->is_module()) {
        return u->module().get(Path(VectorOfStrings(pt.begin() + 1, pt.end())));
    } else if (u->is_package()) {
        return u->package().get(Path(VectorOfStrings(pt.begin() + 1, pt.end())));
    }
    return nullptr;
}
