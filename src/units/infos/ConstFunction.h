//
// Created by chris on 6/4/21.
//
#ifndef CONST_FUNCTION_H
#define CONST_FUNCTION_H

#include <ast/general/Type.h>
#include <common/Path.h>
#include <ast/general/TypeFunction.h>
#include <ast/top/Function.h>
#include <simple_nodes/sem.h>
#include <simple_nodes/common/include/TypeFunction.h>
#include <unordered_map>

class ConstFunction {
private:
public:
    sem::TypeFunction const_function_ft;
    std::unordered_map<std::string, std::set<std::string>> constraints;

    ConstFunction(Path path, const sem::TypeFunction& ft);

    ConstFunction(const ConstFunction& other) = default;

    ConstFunction& operator=(const ConstFunction& other) = default;

    // ConstFunction(const ConstFunction& other);

    ~ConstFunction();
    Path path;
};

#endif