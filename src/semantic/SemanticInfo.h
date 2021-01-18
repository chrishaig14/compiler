//
// Created by chris on 2/8/20.
//

#ifndef SYMBOLINFO_H
#define SYMBOLINFO_H

#include "FunctionInfo.h"
#include "ObjectInfo.h"
#include "ClassInfo.h"

class SemanticInfo {
    const TypeNode* _type;
public:
    bool is_function;
    bool is_method;
    ClassInfo* class_info;
    bool is_class_method;
    bool is_error;

    SemanticInfo& operator=(const SemanticInfo& other) {
        this->_type = other._type->clone();
        return *this;
    }

    SemanticInfo(const SemanticInfo& other) {
        this->is_error = other.is_error;
        this->is_function = other.is_function;
        this->is_method = other.is_method;
        this->class_info = other.class_info;
        this->is_class_method = other.is_class_method;
        if (other._type != nullptr) {
            this->_type = other._type->clone();
        } else {
            this->_type = nullptr;
        }
        this->is_error = is_error;
    }

    ~SemanticInfo() {
        if (this->_type != nullptr) {
            delete this->_type;
        }
    }

    SemanticInfo();

    void set_type(const TypeNode& typ);

    const TypeNode& type();
};

class ErrorStub : public SemanticInfo {
public:
    ErrorStub();
};


#endif //SYMBOLINFO_H
