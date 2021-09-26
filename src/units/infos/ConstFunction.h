//
// Created by chris on 6/4/21.
//
#ifndef CONST_FUNCTION_H
#define CONST_FUNCTION_H

#include "../entities/Entity.h"
#include "../../ast/general/Type.h"
#include "../../util/Path.h"
#include "../../ast/general/FunctionType.h"
#include "../../ast/top/Function.h"
#include "../../simple_nodes/sem.h"
#include "../../simple_nodes/common/src/TypeFunction.h"

class ConstFunction {
private:
    sem::UTypeFunction const_function_ft_p;
public:

    ConstFunction(Path path, sem::UTypeFunction ft);

    // ConstFunction(const ConstFunction& other);

    ~ConstFunction();
    Implicit* implicit;
    sem::TypeFunction& const_function_ft;
    Path path;
};

#endif