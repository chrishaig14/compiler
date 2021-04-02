//
// Created by chris on 30/8/20.
//

#ifndef NONENODE_H
#define NONENODE_H


#include "Node.h"


class NoneNode : public Node {
public:
    NoneNode(TextPosition start, TextPosition end);
    bool equal(const Node& other) const override;
    NoneNode& none() override;
    const NoneNode& none() const override;

};


#endif //NONENODE_H
