//
// Created by chris on 16/8/20.
//

#ifndef WHILENODE_H
#define WHILENODE_H


#include "Node.h"
#include "BlockNode.h"

class WhileNode : public Node {
public:
    BlockNode* body;
    Node* condition;

    WhileNode(Node* condition, BlockNode* body);

    void accept(Visitor& visitor) override;

    bool equal(const Node& other) const override ;

    WhileNode &whil() override;

    ~WhileNode();

    const WhileNode &whil() const override;

};


#endif //WHILENODE_H
