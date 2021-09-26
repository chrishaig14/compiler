//
// Created by chris on 25/9/21.
//

#include "ExpressionInfo.h"
#include "../simple_nodes/expressions/include/Exp.h"
#include "../units/entities/EntityNothing.h"
#include "../units/entities/EntityError.h"

EntityNothing __nothing;
EntityError __error;

ExpressionInfo::ExpressionInfo() : entity(__nothing) {
    this->is_constant = false;
    this->this_arg = nullptr;
    this->exp_snode = nullptr;
    this->is_tuple_member = false;
}

ExpressionInfo::~ExpressionInfo() {
}

bool ExpressionInfo::is_error() {
    return this->entity.get().is_error();
}

ExpErrorStub::ExpErrorStub() {
    this->entity = __error;
}
