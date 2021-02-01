//
// Created by chris on 31/1/21.
//

#ifndef XLANG_EMPTYDICTNODE_H
#define XLANG_EMPTYDICTNODE_H


#include "Node.h"
#include "TypeNode.h"

class EmptyDictNode : public Node {
public:
    EmptyDictNode(TypeNode* key_type, TypeNode* value_type) {
        this->ntype = EMPTYDICT;
        this->key_type = key_type;
        this->value_type = value_type;
    }

    bool equal(const Node& other) const override {
        auto& o = other.emptydict();
        return *o.key_type == *this->key_type && *o.value_type == *this->value_type;
    }

    EmptyDictNode& emptydict() override {
        return *this;
    }

    const EmptyDictNode& emptydict() const override {
        return *this;
    }

    TypeNode* key_type;
    TypeNode* value_type;
};


#endif //XLANG_EMPTYDICTNODE_H
