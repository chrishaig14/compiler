//
// Created by chris on 31/1/21.
//

#ifndef XLANG_EMPTYDICT_H
#define XLANG_EMPTYDICT_H


#include "Node.h"
#include "TypeNode.h"
#include "ast.h"

class ast::EmptyDict : public ast::Node {
public:
    EmptyDict(UTypeNode& key_type, UTypeNode& value_type, TextPosition start, TextPosition end)
            : ast::Node(NodeType::EMPTYDICT, start, end),

              key_type(std::move(key_type)), value_type(std::move(value_type)) {
    }

    bool equal(const ast::Node& other) const override {
        const auto& o = (EmptyDict&) other;
        return *o.key_type == *this->key_type && *o.value_type == *this->value_type;
    }

    nlohmann::json to_json() const override {
        return {{"type",       "empty_dict"},
                {"empty_dict", {{"key_type", this->key_type->to_json()}, {"value_type", this->value_type->to_json()}}}};
    }

    UTypeNode key_type;
    UTypeNode value_type;
};


#endif //XLANG_EMPTYDICT_H
