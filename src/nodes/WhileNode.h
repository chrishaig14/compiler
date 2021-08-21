//
// Created by chris on 16/8/20.
//

#ifndef WHILENODE_H
#define WHILENODE_H


#include "Node.h"
#include "BlockNode.h"

class WhileNode : public Node {
public:
    std::unique_ptr<BlockNode> body;
    UNode condition;

    WhileNode(UNode& condition, std::unique_ptr<BlockNode>& body, TextPosition start, TextPosition end);

    bool equal(const Node& other) const override;

    ~WhileNode() override;
    nlohmann::json to_json() const override;

};


#endif //WHILENODE_H
