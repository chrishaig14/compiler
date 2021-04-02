//
// Created by chris on 2/4/21.
//

#ifndef XLANG_DEFAULTCONSTRUCTORNODE_H
#define XLANG_DEFAULTCONSTRUCTORNODE_H


#include "Node.h"

class DefaultConstructorNode : public Node {
public:
    DefaultConstructorNode(std::string name) {
        this->ntype = DEF_CONST;
        this->name = name;
    }

    bool equal(const Node& other) const override {
        return false;
    }

    DefaultConstructorNode& defconst() override {
        return *this;

    }

    const DefaultConstructorNode& defconst() const override {
        return *this;
    }

    std::string name;
};


#endif //XLANG_DEFAULTCONSTRUCTORNODE_H
