//
// Created by chris on 1/8/20.
//

#ifndef NODE_H
#define NODE_H

#include "../node_types.h"
#include <string>
#include "../../scanner/TextPosition.h"
#include "../../json/json_fwd.hpp"
#include "../../json/json.hpp"
#include <iostream>
#include "../../util/types.h"
#include "../ast.h"
#include "../exp/include/ExpNode.h"

class ast::CommonNode {
public:
    const CommonNodeType ntype;

    TextPosition start;
    TextPosition end;

    CommonNode(CommonNodeType ntype, TextPosition start, TextPosition end);

    virtual ~CommonNode();

    virtual bool equal(const ast::CommonNode& other) const;

    bool operator==(const ast::CommonNode& other) const;
    bool operator!=(const ast::CommonNode& other) const;

    virtual nlohmann::json to_json() const = 0;
};

#endif //NODE_H
