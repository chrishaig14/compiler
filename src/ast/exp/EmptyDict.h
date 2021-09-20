//
// Created by chris on 31/1/21.
//

#ifndef XLANG_EMPTYDICT_H
#define XLANG_EMPTYDICT_H


#include "ExpNode.h"
#include "../Type.h"
#include "../ast.h"

class ast::EmptyDict : public ast::ExpNode {
public:
    EmptyDict(ast::UTypeNode& key_type, ast::UTypeNode& value_type, TextPosition start, TextPosition end)
            : ast::ExpNode(ExpNodeType::EMPTYDICT, start, end),

              key_type(std::move(key_type)), value_type(std::move(value_type)) {
    }

    bool equal(const ast::ExpNode& other) const override {
        const auto& o = (EmptyDict&) other;
        return *o.key_type == *this->key_type && *o.value_type == *this->value_type;
    }

    nlohmann::json to_json() const override {
        return {{"type",       "empty_dict"},
                {"empty_dict", {{"key_type", this->key_type->to_json()}, {"value_type", this->value_type->to_json()}}}};
    }

    ast::UTypeNode key_type;
    ast::UTypeNode value_type;
};


#endif //XLANG_EMPTYDICT_H
