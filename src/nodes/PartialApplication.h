//
// Created by chris on 7/12/20.
//

#ifndef UNTITLED1_PARTIALAPPLICATION_H
#define UNTITLED1_PARTIALAPPLICATION_H

#include "../types.h"
#include "Node.h"
#include "TypeNode.h"

class PartialApplication : public Node {
public:
    Node* function;
    VectorOfNodes args;
    FunctionType* complete_type;
    PartialApplication(Node* function, VectorOfNodes args, TextPosition start, TextPosition end);

    bool equal(const Node& other) const override;
    PartialApplication& partial() override;
    const PartialApplication& partial() const override;

};


#endif //UNTITLED1_PARTIALAPPLICATION_H
