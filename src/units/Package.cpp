//
// Created by chris on 6/4/21.
//

#include "Package.h"

Package::Package(std::string name, std::string path) : Entity(E_TYPE::PACKAGE) {
    this->name = name;
    this->path = path;
}
