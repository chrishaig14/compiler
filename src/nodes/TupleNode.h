//
// Created by chris on 23/11/20.
//

#ifndef UNTITLED1_TUPLENODE_H
#define UNTITLED1_TUPLENODE_H


#include "Node.h"
#include "../types.h"

class TupleNode : public Node {
public:
    VectorOfNodes values;
    bool equal(const Node& other) const override;

    TupleNode(const VectorOfNodes& values);
    TupleNode& tuple() override;
    const TupleNode& tuple() const override;

};


#endif //UNTITLED1_TUPLENODE_H
