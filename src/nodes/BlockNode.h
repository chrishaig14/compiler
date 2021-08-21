//
// Created by chris on 2/8/20.
//

#ifndef BLOCKNODE_H
#define BLOCKNODE_H


#include <vector>
#include "Node.h"

#include "Node.h"
#include "../types.h"
#include "TypeNode.h"
#include "../units/Entity.h"

class BlockNode : public Node {
public:

    BlockNode(VectorOfNodesU nodes, TextPosition start, TextPosition end);

    static UBlockNode make(VectorOfNodesU nodes, TextPosition start, TextPosition end) {
        return std::make_unique<BlockNode>(std::move(nodes), start, end);
    }


    VectorOfNodesU nodes;

    bool equal(const Node& p) const override;


    ~BlockNode();
    nlohmann::json to_json() const override;
    std::vector<std::pair<std::string, TypeNode*>> local_vars;
};


#endif //BLOCKNODE_H
