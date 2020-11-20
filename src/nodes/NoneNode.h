//
// Created by chris on 30/8/20.
//

#ifndef NONENODE_H
#define NONENODE_H


#include "Node.h"
#include "Visitor.h"

class NoneNode : public Node {
public:
    bool equal(const Node& other) const override;

};


#endif //NONENODE_H
