//
// Created by chris on 6/4/21.
//

#include "Module.h"

Module::Module(std::string name, std::string path) : Entity(E_TYPE::MODULE) {
    this->name = name;
    this->path = path;
}