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
    std::vector<std::pair<UNode, UNode>> _items;
public:
    std::vector<std::pair<std::reference_wrapper<Node>, std::reference_wrapper<Node>>> items;
    DictNode(std::vector<std::pair<UNode, UNode>> items, TextPosition start, TextPosition end);

    nlohmann::json to_json() const override;
};


#endif //XLANG_DICTNODE_H
