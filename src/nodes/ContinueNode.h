//
// Created by chris on 7/11/20.
//

#ifndef CONTINUENODE_H
#define CONTINUENODE_H


#include "Node.h"


class ContinueNode : public Node {
public:
    ContinueNode(TextPosition start, TextPosition end);
    bool equal(const Node& other) const override;
    ContinueNode& cntinue() override;
    const ContinueNode& cntinue() const override;
};


#endif //CONTINUENODE_H
