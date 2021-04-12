//
// Created by chris on 6/4/21.
//

#include "Package.h"

Package::Package(std::string name, std::string abs_path, std::string rel_path) {
    this->name = name;
    this->abs_path = abs_path;
    this->rel_path = rel_path;
}
