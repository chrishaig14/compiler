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

    bool operator==(const ContinueNode& other) const {return true;}
    bool operator!=(const ContinueNode& other) const {return false;}
};


#endif //CONTINUENODE_H
