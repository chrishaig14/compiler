//
// Created by chris on 2/8/20.
//

#include "SemanticInfo.h"

SemanticInfo::SemanticInfo() {
    this->class_info = nullptr;
    this->is_class_method = false;
    this->is_error = false;
    this->_type = nullptr;
    this->is_constant = false;
    this->this_arg = nullptr;
    this->snode = nullptr;
}

SemanticInfo& SemanticInfo::operator=(const SemanticInfo& other) {
    this->entity = other.entity;
    this->is_error = other.is_error;
    this->class_info = other.class_info;
    this->is_class_method = other.is_class_method;
    this->is_constant = other.is_constant;
    this->snode = other.snode;
    this->method_name = other.method_name;
    this->this_arg = other.this_arg;
    if (other._type != nullptr) {
        this->_type = other._type->clone();
    } else {
        this->_type = nullptr;
    }
    return *this;
}

SemanticInfo::SemanticInfo(const SemanticInfo& other) {
    this->entity = other.entity;
    this->is_error = other.is_error;
    this->this_arg = other.this_arg;
    this->class_info = other.class_info;
    this->is_class_method = other.is_class_method;
    this->is_constant = other.is_constant;
    this->snode = other.snode;
    this->method_name = other.method_name;

    if (other._type != nullptr) {
        this->_type = other._type->clone();
    } else {
        this->_type = nullptr;
    }
}

SemanticInfo::~SemanticInfo() {
    if (this->_type != nullptr) {
        delete this->_type;
    }
}

ErrorStub::ErrorStub() {
    this->is_error = true;
    auto x = UnknownTypeNode();
    this->entity = Entity{.type=E_TYPE::ERROR};
}
