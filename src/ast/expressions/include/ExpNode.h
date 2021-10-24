//
// Created by chris on 1/8/20.
//

#ifndef EXP_NODE_H
#define EXP_NODE_H

#include "../../node_types.h"
#include <string>
#include <json/json_fwd.hpp>
#include <json/json.hpp>
#include <iostream>
#include <util/types.h>
#include "../../ast.h"
#include <common/TextPosition.h>

class ast::ExpNode {
public:
    const ExpNodeType ntype;

    TextPosition start;
    TextPosition end;

    ExpNode(ExpNodeType ntype, TextPosition start, TextPosition end);

    virtual ~ExpNode();

    virtual bool equal(const ast::ExpNode& other) const;

    bool operator==(const ast::ExpNode& other) const;
    bool operator!=(const ast::ExpNode& other) const;

    virtual nlohmann::json to_json() const = 0;
};

#endif //EXP_NODE_H
