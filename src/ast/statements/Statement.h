//
// Created by chris on 1/8/20.
//

#ifndef NODE_H
#define NODE_H

#include "../node_types.h"
#include <string>
#include <scanner/TextPosition.h>
#include <json/json_fwd.hpp>
#include <json/json.hpp>
#include <iostream>
#include <util/types.h>
#include "../ast.h"
#include "../expressions/include/ExpNode.h"

class ast::Statement {
public:
    const StatementType ntype;

    TextPosition start;
    TextPosition end;

    Statement(StatementType ntype, TextPosition start, TextPosition end);

    virtual ~Statement();

    virtual bool equal(const ast::Statement& other) const;

    bool operator==(const ast::Statement& other) const;
    bool operator!=(const ast::Statement& other) const;

    virtual nlohmann::json to_json() const = 0;
};

#endif //NODE_H
