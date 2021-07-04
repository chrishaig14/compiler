//
// Created by chris on 6/9/20.
//

#ifndef EMPTYLISTNODE_H
#define EMPTYLISTNODE_H


#include "Node.h"
#include "TypeNode.h"

class EmptyListNode : public Node {
public:
    EmptyListNode(TypeNode* type, TextPosition start, TextPosition end);

    bool equal(const Node& other) const override;

    ~EmptyListNode() override;
    nlohmann::json to_json() override;

    TypeNode* type;
};


#endif //EMPTYLISTNODE_H
