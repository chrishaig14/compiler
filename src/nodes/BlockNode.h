//
// Created by chris on 2/8/20.
//

#ifndef BLOCKNODE_H
#define BLOCKNODE_H


#include <vector>
#include "Node.h"
#include "Visitor.h"
#include "Node.h"
#include "../types.h"

class BlockNode : public Node {
public:

    BlockNode(VectorOfNodes nodes) : nodes(nodes) {}

    BlockNode();

    VectorOfNodes nodes;

    bool operator!=(const BlockNode& other) const {
        return !(*this == other);
    }

    bool operator==(const BlockNode& other) const {
        if (this->nodes.size() != other.nodes.size()) return false;
        for (int i = 0; i < this->nodes.size(); i++) {
//            if (!this->nodes[i]->equal(other.nodes[i])) return false;
        };
        return true;
    }

    void accept(Visitor& visitor) override;

    json to_json() {
        json j;
        j["node"] = "block";
        j["block"] = {};
        for (auto n: this->nodes) {
//            j["block"].push_back(n->to_json());
        }
        return j;
    }
};


#endif //BLOCKNODE_H
