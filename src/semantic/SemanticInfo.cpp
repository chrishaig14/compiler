//
// Created by chris on 2/8/20.
//

#include "SemanticInfo.h"

SemanticInfo::SemanticInfo() {
    this->is_function = false;
    this->is_method = false;
    this->class_info = nullptr;
    this->is_class_method = false;
    this->is_error = false;
    this->_type = nullptr;
    this->is_constant = false;
    this->is_module = false;
    this->is_package = false;
    this->snode = nullptr;
}

void SemanticInfo::set_type(const TypeNode& typ) {
    this->_type = typ.clone();
}

const TypeNode& SemanticInfo::type() {
    if (_type == nullptr) {
        throw std::runtime_error("SymbolInfo has no TypeNode");
    }
    return *this->_type;
}

SemanticInfo& SemanticInfo::operator=(const SemanticInfo& other) {
    this->_type = other._type->clone();
    return *this;
}

SemanticInfo::SemanticInfo(const SemanticInfo& other) {
    this->is_error = other.is_error;
    this->is_function = other.is_function;
    this->is_method = other.is_method;
    this->class_info = other.class_info;
    this->is_class_method = other.is_class_method;
    this->is_constant = other.is_constant;
    this->is_module = other.is_module;
    this->is_package = other.is_package;
    this->module = other.module;
    this->package = other.package;
    this->snode = other.snode;

    if (other._type != nullptr) {
        this->_type = other._type->clone();
    } else {
        this->_type = nullptr;
    }
    this->is_error = is_error;
}

SemanticInfo::~SemanticInfo() {
    if (this->_type != nullptr) {
        delete this->_type;
    }
}

ErrorStub::ErrorStub() {
    this->is_error = true;
    auto x = UnknownTypeNode();
    this->set_type(x);
}
