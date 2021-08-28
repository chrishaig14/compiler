//
// Created by chris on 31/1/21.
//

#ifndef XLANG_DICTNODE_H
#define XLANG_DICTNODE_H


#include "Node.h"
#include <vector>
#include "ast.h"

class ast::DictNode : public ast::Node {
    bool equal(const ast::Node& other) const override;
public:
    DictNode(std::vector<std::pair<UNode, UNode>>& items, TextPosition start, TextPosition end);

    nlohmann::json to_json() const override;

    std::vector<std::pair<UNode, UNode>> items;
};


#endif //XLANG_DICTNODE_H
