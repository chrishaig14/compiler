//
// Created by chris on 1/8/20.
//

#ifndef CALLNODE_H
#define CALLNODE_H


#include <vector>
#include "Node.h"

#include "Node.h"
#include "../types.h"
#include "TypeNode.h"

class CallNode : public Node {
public:
    Node* function;
    VectorOfNodes arguments;

    CallNode(Node* function, const VectorOfNodes& arguments, TextPosition start, TextPosition end);

    bool equal(const Node& other) const override;

    ~CallNode();

    CallNode& call() override;
    const CallNode& call() const override;

    VectorOfTypes ftype;
    VectorOfTypes arg_types;
};


#endif //CALLNODE_H
