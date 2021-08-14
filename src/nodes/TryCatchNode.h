//
// Created by chris on 19/6/21.
//

#ifndef XLANG_TRYCATCHNODE_H
#define XLANG_TRYCATCHNODE_H


#include "Node.h"
#include "TypeNode.h"
#include "BlockNode.h"

class TryCatchNode : public Node {
public:
    TryCatchNode(BlockNode* body, std::vector<std::pair<std::string, ObjectType*>> catches, VectorOfNodes catches_bodies,
                 TextPosition start, TextPosition end);

    bool equal(const Node& other) const override;

    BlockNode* body;
    VectorOfNodes catches_bodies;
    std::vector<std::pair<std::string, ObjectType*>> catches;
};


#endif //XLANG_TRYCATCHNODE_H
