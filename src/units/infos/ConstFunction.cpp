//
// Created by chris on 6/4/21.
//
#include "ConstFunction.h"

ConstFunction::ConstFunction(Path path, sem::UTypeFunction ft)
        : const_function_ft_p(std::move(ft)), const_function_ft(*const_function_ft_p), path(path) {
}

ConstFunction::~ConstFunction() {
    // std::cout << "Destroyinh CONST FUnction!" << std::endl;
}
