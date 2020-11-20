//
// Created by chris on 1/8/20.
//

#ifndef CALLNODE_H
#define CALLNODE_H


#include <vector>
#include "Node.h"

#include "Node.h"
#include "../types.h"

class CallNode : public Node {
public:
    Node* function;
    VectorOfNodes arguments;

    CallNode(Node* function, const VectorOfNodes& arguments);

    bool equal(const Node& other) const override;

    ~CallNode();

    json to_json() const override;

    CallNode& call() override;
    const CallNode& call() const override;

};


#endif //CALLNODE_H
