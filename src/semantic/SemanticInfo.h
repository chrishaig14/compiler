//
// Created by chris on 2/8/20.
//

#ifndef SYMBOLINFO_H
#define SYMBOLINFO_H

#include <string>
#include <vector>
#include "../ast/TypeNode.h"
#include "../units/Class.h"
#include "../simple_nodes/SNode.h"
#include "../units/Module.h"
#include "../units/Package.h"

class SemanticInfo {
public:
    std::reference_wrapper<Entity> entity;
    sem::SNode* snode;
    bool is_tuple_member;
    SemanticInfo& operator=(const SemanticInfo& other);

    SemanticInfo(const SemanticInfo& other);

    ~SemanticInfo();

    SemanticInfo();

    bool is_constant;
    sem::SNode* this_arg;

    bool is_error();
};

class ErrorStub : public SemanticInfo {
public:
    ErrorStub();
};


#endif //SYMBOLINFO_H
