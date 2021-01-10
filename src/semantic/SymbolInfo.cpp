//
// Created by chris on 2/8/20.
//

#include "SymbolInfo.h"

SymbolInfo::SymbolInfo() {
    this->is_function = false;
    this->is_method = false;
    this->class_info = nullptr;
    this->is_class_method = false;
    this->is_error = false;
    this->_type = nullptr;
}

void SymbolInfo::set_type(const TypeNode& typ) {
    this->_type = typ.clone();
}

const TypeNode& SymbolInfo::type() {
    if (_type == nullptr) {
        throw std::runtime_error("SymbolInfo has no TypeNode");
    }
    return *this->_type;
}

ErrorStub::ErrorStub() {
    this->is_error = true;
    auto x = UnknownTypeNode();
    this->set_type(x);
}
