//
// Created by chris on 2/8/20.
//

#ifndef SYMBOLINFO_H
#define SYMBOLINFO_H

#include <string>
#include <vector>
#include "../nodes/TypeNode.h"
#include "../units/Class.h"
#include "../simple_nodes/SNode.h"
#include "../units/Module.h"
#include "../units/Package.h"

class SemanticInfo {
public:
    Entity entity;
    bool is_class_method;
    SNode* snode;

    SemanticInfo& operator=(const SemanticInfo& other);

    SemanticInfo(const SemanticInfo& other);

    ~SemanticInfo();

    SemanticInfo();

    bool is_constant;
    SNode* this_arg;
};

class ErrorStub : public SemanticInfo {
public:
    ErrorStub();
};


#endif //SYMBOLINFO_H
