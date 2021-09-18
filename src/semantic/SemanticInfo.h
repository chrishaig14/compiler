//
// Created by chris on 2/8/20.
//

#ifndef SYMBOLINFO_H
#define SYMBOLINFO_H

#include <string>
#include <vector>
#include "../ast/Type.h"
#include "../units/Class.h"
#include "../simple_nodes/common/include/SNode.h"
#include "../units/Module.h"
#include "../units/Package.h"

class SemanticInfo {
public:
    std::reference_wrapper<Entity> entity;
    std::unique_ptr<Entity> _entity;

    void set_entity(Entity* e) {
        this->_entity = std::unique_ptr<Entity>(e);
        this->entity = *e;
    }

    void set_entity(std::unique_ptr<Entity> e) {
        this->_entity = std::move(e);
        this->entity = *this->_entity;
    }

    USNode snode;
    bool is_tuple_member;
    ~SemanticInfo();

    SemanticInfo();

    bool is_constant;
    sem::SNode* this_arg;

    bool is_error();
};

class SemanticInfoBlock {
public:
    std::unique_ptr<sem::Block> snode;
};

class ErrorStub : public SemanticInfo {
public:
    ErrorStub();
};


#endif //SYMBOLINFO_H
