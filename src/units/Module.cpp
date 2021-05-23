//
// Created by chris on 6/4/21.
//

#include "Module.h"

Module::Module(Path path, std::string abs_path, std::string rel_path) {
    this->path = path;
    this->name = path.as_vec().back();
    this->abs_path = abs_path;
    this->rel_path = rel_path;
}