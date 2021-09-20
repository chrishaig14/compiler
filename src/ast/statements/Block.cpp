//
// Created by chris on 2/8/20.
//

#include "Block.h"

#include <utility>

using namespace ast;

Block::~Block() {
    // for (auto p: this->nodes) {
    //     delete p;
    // }
    // for (auto l: this->local_vars) {
    //     delete l.second;
    // }
}

bool Block::equal(const ast::Statement& p) const {
    auto& other = (Block&) p;
    if (this->nodes.size() != other.nodes.size()) {
        return false;
    }
    for (size_t i = 0; i < this->nodes.size(); i++) {
        if (*this->nodes[i] != *other.nodes[i]) {
            return false;
        }
    };
    return true;
}

Block::Block(ast::VectorOfNodesU nodes, TextPosition start, TextPosition end) : ast::Statement(StatementType::BLOCK, start, end),
                                                                           nodes(std::move(nodes)) {
    ;
}

nlohmann::json Block::to_json() const {
    std::vector<nlohmann::json> v;
    for (auto& s: this->nodes) {
        v.push_back(s->to_json());
    }
    return v;
}

