//
// Created by chris on 6/4/21.
//

#include "Module.h"

Module::Module(Path path, std::string abs_path, std::string rel_path, bool is_lib, std::string full_header_path) : name(
        path.as_vec().back()), abs_path(abs_path), is_lib(is_lib), full_header_path(full_header_path), path(path) {
}

Flirpin Module::get(Path p) {
    VectorOfStrings pt = p.as_vec();
    return this->flirpins[pt.back()];
}
