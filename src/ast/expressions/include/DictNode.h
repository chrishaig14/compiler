//
// Created by chris on 31/1/21.
//

#ifndef XLANG_DICTNODE_H
#define XLANG_DICTNODE_H


#include "ExpNode.h"
#include <vector>

class ast::DictNode : public ast::ExpNode {
    bool equal(const ast::ExpNode& other) const override;
    std::vector<std::pair<ast::UExpNode, ast::UExpNode>> _items;
public:
    std::vector<std::pair<RExpNode, RExpNode>> items;
    DictNode(std::vector<std::pair<ast::UExpNode, ast::UExpNode>> items, TextPosition start, TextPosition end);

    nlohmann::json to_json() const override;
};


#endif //XLANG_DICTNODE_H
