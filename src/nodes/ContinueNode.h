//
// Created by chris on 7/11/20.
//

#ifndef CONTINUENODE_H
#define CONTINUENODE_H


#include "Node.h"
#include "Visitor.h"


class ContinueNode : public Node{
public:
    void accept(Visitor& visitor) override;

    bool equal(NodeContainer other) const override;

};


#endif //CONTINUENODE_H
