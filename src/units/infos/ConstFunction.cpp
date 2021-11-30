//
// Created by chris on 6/4/21.
//
#include "ConstFunction.h"

ConstFunction::ConstFunction(Path path, const sem::TypeFunction& ft) : const_function_ft(ft), path(path) {
}

ConstFunction::~ConstFunction() {
    // std::cout << "Destroyinh CONST FUnction!" << std::endl;
}
