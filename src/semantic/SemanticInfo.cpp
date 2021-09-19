//
// Created by chris on 2/8/20.
//

#include "SemanticInfo.h"
#include "../simple_nodes/expressions/include/Exp.h"

EntityNothing nothing;
EntityError error;

SemanticInfo::SemanticInfo() : entity(nothing) {
    this->is_constant = false;
    this->this_arg = nullptr;
    this->snode = nullptr;
    this->is_tuple_member = false;
}

SemanticInfo::~SemanticInfo() {
}

bool SemanticInfo::is_error() {
    return this->entity.get().type == E_TYPE::ERROR;
}

ErrorStub::ErrorStub() {
    this->entity = error;
}
