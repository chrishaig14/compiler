//
// Created by chris on 1/8/20.
//

#ifndef IFNODE_H
#define IFNODE_H


#include <vector>
#include "Node.h"

#include "BlockNode.h"
#include "Node.h"

class IfNode : public Node {
public:
    std::unique_ptr<Node> condition;
    std::unique_ptr<BlockNode> then;
    std::unique_ptr<BlockNode> selse;
    std::vector<std::pair<Node*, BlockNode*>> elifs;
    IfNode(std::unique_ptr<Node>& condition, std::unique_ptr<BlockNode>& then,
           std::vector<std::pair<Node*, BlockNode*>> elifs, std::unique_ptr<BlockNode>& selse, TextPosition start,
           TextPosition end);

    bool equal(const Node& other) const override;

    nlohmann::json to_json() const override;

    ~IfNode() override;

};


#endif //IFNODE_H
