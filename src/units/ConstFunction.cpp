//
// Created by chris on 6/4/21.
//
#include "ConstFunction.h"

ConstFunction::ConstFunction(Path path, sem::TypeFunction* ft) : const_function_ft(*ft) {
    this->path = path;
    this->const_function_ft_p = ft;
    this->implicit = nullptr;
}

ConstFunction::ConstFunction(const ConstFunction& other)
        : const_function_ft_p(other.const_function_ft.clone()), const_function_ft(*const_function_ft_p) {
    this->implicit = other.implicit;
}

ConstFunction::~ConstFunction() {
    std::cout << "Destroyinh CONST FUnction!" << std::endl;
}
