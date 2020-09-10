//
// Created by chris on 30/8/20.
//

#ifndef UNTITLED1_NONENODE_H
#define UNTITLED1_NONENODE_H


#include "Node.h"
#include "Visitor.h"

class NoneNode : public Node {
public:
    void accept(Visitor& visitor) override;

    bool equal(Node* other) const override;

};


#endif //UNTITLED1_NONENODE_H
