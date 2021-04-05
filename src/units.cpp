//
// Created by chris on 4/4/21.
//

#include "units.h"

Unit::Unit(std::string name, std::string path, UnitType type) {
    this->name = name;
    this->path = path;
    this->type = type;
}

Package::Package(std::string name, std::string path) : Unit(name, path, UnitType::PACKAGE) {
}

Module::Module(std::string name, std::string path) : Unit(name, path, UnitType::MODULE) {
}
