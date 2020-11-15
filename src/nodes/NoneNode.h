//
// Created by chris on 30/8/20.
//

#ifndef NONENODE_H
#define NONENODE_H


#include "Node.h"
#include "Visitor.h"

class NoneNode : public Node {
public:
    void accept(Visitor& visitor) override;

    bool equal(NodeContainer other) const override;

    bool operator==(const NoneNode& other )const;
    bool operator!=(const NoneNode& other )const;

};


#endif //NONENODE_H
