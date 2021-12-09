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

Module* Package::get_module(Path p) {
    const VectorOfStrings& p_vec = p.as_vec();
    if (p_vec.size() == 1) {
        if (this->units.contains(p.basname())) {
            return &this->units[p.basname()]->module();
        }
        return nullptr;
    }
    auto bbb = p.pathbase().as_vec()[0];
    if (this->units.contains(bbb)) {
        if (this->units[bbb]->is_module()) {
            return nullptr;
        }
        VectorOfStrings v = VectorOfStrings(p_vec.begin() + 1, p_vec.end());
        return this->units[bbb]->package().get_module(Path(v));
    }
    return nullptr;
}
