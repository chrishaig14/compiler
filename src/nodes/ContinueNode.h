//
// Created by chris on 7/11/20.
//

#ifndef CONTINUENODE_H
#define CONTINUENODE_H


#include "Node.h"



class ContinueNode : public Node{
public:
    bool equal(const Node& other) const override;
};


#endif //CONTINUENODE_H
