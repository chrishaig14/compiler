//
// Created by chris on 2/8/20.
//

#ifndef SYMBOLINFO_H
#define SYMBOLINFO_H

#include <string>
#include <vector>
#include "../nodes/TypeNode.h"
#include "ClassInfo.h"
#include "../units.h"

class SemanticInfo {
    const TypeNode* _type;
public:
    bool is_function;
    bool is_method;
    ClassInfo* class_info;
    bool is_class_method;
    bool is_error;

    SemanticInfo& operator=(const SemanticInfo& other);

    SemanticInfo(const SemanticInfo& other);

    ~SemanticInfo();

    SemanticInfo();

    void set_type(const TypeNode& typ);

    const TypeNode& type();
    bool is_constant;
    bool is_imported;
    bool is_imported_package;
    Package* imported_package;
    bool is_module;
    Module* module;
    bool is_package;
    Package* package;
};

class ErrorStub : public SemanticInfo {
public:
    ErrorStub();
};


#endif //SYMBOLINFO_H
