//
// Created by chris on 1/8/20.
//

#ifndef NUMBERNODE_H
#define NUMBERNODE_H


#include "ExpNode.h"
#include "../../ast.h"

enum class NumberType {
    INTEGER, FLOAT, DOUBLE
};

class ast::Number : public ast::ExpNode {
public:
    std::string str;

    Number(NumberType num_type, std::string str, TextPosition start, TextPosition end);

    static UNumber make(NumberType num_type, std::string str, TextPosition start, TextPosition end) {
        return std::make_unique<Number>(num_type, str, start, end);
    }

    bool equal(const ast::ExpNode& x) const override;

    NumberType num_type;

    nlohmann::json to_json() const override;
};


#endif //NUMBERNODE_H
