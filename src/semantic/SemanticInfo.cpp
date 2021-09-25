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
    return this->entity.get().is_error();
}

ExpressionInfo::ExpressionInfo() : entity(nothing) {
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

ErrorStub::ErrorStub() {
    this->entity = error;
}

ExpErrorStub::ExpErrorStub() {
    this->entity = error;
}
