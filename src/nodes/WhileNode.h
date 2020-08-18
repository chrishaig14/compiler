//
// Created by chris on 16/8/20.
//

#ifndef UNTITLED1_WHILENODE_H
#define UNTITLED1_WHILENODE_H


#include "Node.h"
#include "BlockNode.h"

class WhileNode : public Node {
public:
    BlockNode* body;
    Node* condition;


    WhileNode(Node* condition, BlockNode* body);

    void accept(Visitor& visitor) override;

    bool equal(Node* other) const override;

    bool operator==(const WhileNode& other) const;
};


#endif //UNTITLED1_WHILENODE_H
