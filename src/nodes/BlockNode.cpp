//
// Created by chris on 2/8/20.
//

#include "BlockNode.h"

BlockNode& BlockNode::block() {
    return *this;
}

const BlockNode& BlockNode::block() const {
    return *this;
}

json BlockNode::to_json() {
    json j;
    j["node"] = "block";
    j["block"] = {};
    for (auto n: this->nodes) {
//            j["block"].push_back(n->to_json());
    }
    return j;
}

BlockNode::~BlockNode() {
    for (auto p: this->nodes) {
        delete p;
    }
    for(auto l: this->local_vars){
        delete l.second;
    }
}

bool BlockNode::equal(const Node& p) const {
    auto& other = p.block();
    if (this->nodes.size() != other.nodes.size()) {
        return false;
    }
    for (int i = 0; i < this->nodes.size(); i++) {
        if (*this->nodes[i] != *other.nodes[i]) {
            return false;
        }
    };
    return true;
}

BlockNode::BlockNode(VectorOfNodes nodes) : nodes(nodes) { this->ntype = BLOCK; }

