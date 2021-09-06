//
// Created by chris on 6/4/21.
//
#include "ConstFunction.h"

ConstFunction::ConstFunction(Path path, ast::FunctionType* ft) {
    this->path = path;
    this->ft = ft;
    this->implicit = nullptr;
}

ConstFunction::ConstFunction(const ConstFunction& other) {
    this->ft = other.ft->clone();
    this->implicit = other.implicit;
}

ConstFunction::~ConstFunction() {
    std::cout << "Destroyinh CONST FUnction!" << std::endl;
}
