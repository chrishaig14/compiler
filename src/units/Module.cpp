//
// Created by chris on 6/4/21.
//

#include "Module.h"

Module::Module(Path path, std::string abs_path, std::string rel_path, bool is_lib, std::string full_header_path) {
    this->path = path;
    this->name = path.as_vec().back();
    this->abs_path = abs_path;
    this->rel_path = rel_path;
    this->is_lib = is_lib;
    this->full_header_path = full_header_path;
}

Flirpin Module::get(Path p) {
    VectorOfStrings pt = p.as_vec();
    return this->flirpins[pt.back()];
}
