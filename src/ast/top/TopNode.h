//
// Created by chris on 1/8/20.
//

#ifndef TOP_NODE_H
#define TOP_NODE_H

#include "../node_types.h"
#include <string>
#include "../../scanner/TextPosition.h"
#include "../../json/json_fwd.hpp"
#include "../../json/json.hpp"
#include <iostream>
#include "../../util/types.h"
#include "../ast.h"

class ast::TopNode {
public:
    const TopNodeType ntype;

    TextPosition start;
    TextPosition end;

    TopNode(TopNodeType ntype, TextPosition start, TextPosition end);

    virtual ~TopNode();

    virtual bool equal(const ast::TopNode& other) const;

    bool operator==(const ast::TopNode& other) const;
    bool operator!=(const ast::TopNode& other) const;

    virtual nlohmann::json to_json() const = 0;
};

#endif //TOP_NODE_H
