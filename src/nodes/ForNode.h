//
// Created by chris on 14/8/20.
//

#ifndef FORNODE_H
#define FORNODE_H


#include "Node.h"
#include "BlockNode.h"
#include "../utils.h"
class ForNode : public Node {
public:
    std::string var;
    Node* exp;
    BlockNode* body;
    bool equal(const Node& other) const override ;

    ForNode(const std::string& var, Node* exp, BlockNode* body);
    ~ForNode();
    ForNode& forloop() override;
    const ForNode& forloop() const override;

};


#endif //FORNODE_H
