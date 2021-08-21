//
// Created by chris on 31/1/21.
//

#ifndef XLANG_DICTNODE_H
#define XLANG_DICTNODE_H


#include "Node.h"
#include <vector>

class DictNode : public Node {
    bool equal(const Node& other) const override;
public:
    DictNode(std::vector<std::pair<std::unique_ptr<Node>, std::unique_ptr<Node>>>& items, TextPosition start,
             TextPosition end);

    nlohmann::json to_json() const override;

    std::vector<std::pair<std::unique_ptr<Node>, std::unique_ptr<Node>>> items;
};


#endif //XLANG_DICTNODE_H
