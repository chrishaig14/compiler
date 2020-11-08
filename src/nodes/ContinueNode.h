//
// Created by chris on 7/11/20.
//

#ifndef UNTITLED1_CONTINUENODE_H
#define UNTITLED1_CONTINUENODE_H


#include "Node.h"
#include "Visitor.h"


class ContinueNode : public Node{
public:
    void accept(Visitor& visitor) override;

    bool equal(Node* other) const override;

};


#endif //UNTITLED1_CONTINUENODE_H
