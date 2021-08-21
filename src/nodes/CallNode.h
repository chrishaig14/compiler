//
// Created by chris on 1/8/20.
//

#ifndef CALLNODE_H
#define CALLNODE_H


#include <vector>
#include "Node.h"
#include "../types.h"
#include "TypeNode.h"

class CallNode : public Node {
public:
    UNode function;
    VectorOfNodesU arguments;

    CallNode(UNode& function, VectorOfNodesU& arguments, TextPosition start, TextPosition end);

    bool equal(const Node& other) const override;

    ~CallNode();
    nlohmann::json to_json() const override;

    VectorOfTypes ftype;
    VectorOfTypes arg_types;
};


#endif //CALLNODE_H
