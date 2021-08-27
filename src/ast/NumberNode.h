//
// Created by chris on 1/8/20.
//

#ifndef NUMBERNODE_H
#define NUMBERNODE_H


#include "Node.h"
#include "ast.h"

enum class NumberType {
    INTEGER, FLOAT, DOUBLE
};

class ast::NumberNode : public ast::Node {
public:
    std::string str;

    NumberNode(NumberType num_type, std::string str, TextPosition start, TextPosition end);

    static UNumberNode make(NumberType num_type, std::string str, TextPosition start, TextPosition end) {
        return std::make_unique<NumberNode>(num_type, str, start, end);
    }

    bool equal(const ast::Node& x) const override;

    NumberType num_type;

    nlohmann::json to_json() const override;
};


#endif //NUMBERNODE_H
