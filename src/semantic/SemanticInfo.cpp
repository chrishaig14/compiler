//
// Created by chris on 2/8/20.
//

#include "SemanticInfo.h"

SemanticInfo::SemanticInfo() {
    this->is_constant = false;
    this->this_arg = nullptr;
    this->snode = nullptr;
    this->is_tuple_member = false;
}

SemanticInfo& SemanticInfo::operator=(const SemanticInfo& other) {
    this->entity = other.entity;
    this->is_constant = other.is_constant;
    this->snode = other.snode;
    this->this_arg = other.this_arg;
    this->is_tuple_member = other.is_tuple_member;
    return *this;
}

SemanticInfo::SemanticInfo(const SemanticInfo& other) {
    this->entity = other.entity;
    this->this_arg = other.this_arg;
    this->is_constant = other.is_constant;
    this->is_tuple_member = other.is_tuple_member;
    this->snode = other.snode;
}

SemanticInfo::~SemanticInfo() {
}

ErrorStub::ErrorStub() {
    this->entity = Entity(E_TYPE::ERROR);
}
