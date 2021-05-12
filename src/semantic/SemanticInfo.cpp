//
// Created by chris on 2/8/20.
//

#include "SemanticInfo.h"

SemanticInfo::SemanticInfo() {
    this->is_class_method = false;
    this->is_constant = false;
    this->this_arg = nullptr;
    this->snode = nullptr;
}

SemanticInfo& SemanticInfo::operator=(const SemanticInfo& other) {
    this->entity = other.entity;
    this->is_class_method = other.is_class_method;
    this->is_constant = other.is_constant;
    this->snode = other.snode;
    this->this_arg = other.this_arg;
    return *this;
}

SemanticInfo::SemanticInfo(const SemanticInfo& other) {
    this->entity = other.entity;
    this->this_arg = other.this_arg;
    this->is_class_method = other.is_class_method;
    this->is_constant = other.is_constant;
    this->snode = other.snode;
}

SemanticInfo::~SemanticInfo() {
}

ErrorStub::ErrorStub() {
    auto x = UnknownTypeNode();
    this->entity = Entity{.type=E_TYPE::ERROR};
}
