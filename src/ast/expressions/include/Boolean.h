//
// Created by chris on 17/8/20.
//

#ifndef BOOLEANNODE_H
#define BOOLEANNODE_H


#include "ExpNode.h"
#include <json/json.hpp>
#include "../../ast.h"

class ast::Boolean : public ast::ExpNode {
public:
    bool value;

    Boolean(bool value, TextPosition start, TextPosition end);

    bool operator!=(const Boolean& other) const;

    bool equal(const ast::ExpNode& x) const override;

    nlohmann::json to_json() const override;
};


#endif //BOOLEANNODE_H
