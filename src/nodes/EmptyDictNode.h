//
// Created by chris on 31/1/21.
//

#ifndef XLANG_EMPTYDICTNODE_H
#define XLANG_EMPTYDICTNODE_H


#include "Node.h"
#include "TypeNode.h"

class EmptyDictNode : public Node {
public:
    EmptyDictNode(TypeNode* key_type, TypeNode* value_type, TextPosition start, TextPosition end)
            : Node(NodeType::EMPTYDICT, start, end) {
        this->key_type = key_type;
        this->value_type = value_type;
    }

    bool equal(const Node& other) const override {
        const auto& o = (EmptyDictNode&) other;
        return *o.key_type == *this->key_type && *o.value_type == *this->value_type;
    }

    nlohmann::json to_json() override {
        return {{"type",       "empty_dict"},
                {"empty_dict", {{"key_type", this->key_type->to_json()}, {"value_type", this->value_type->to_json()}}}};
    }

    TypeNode* key_type;
    TypeNode* value_type;
};


#endif //XLANG_EMPTYDICTNODE_H
