//
// Created by chris on 4/4/21.
//

#include "../include/Block.h"

using namespace sem;

Block::Block() : Common(CommonType::BLOCK) {
    this->unwrap = false;
}

bool Block::equals(const Common& o) const {
    auto& other = (const Block&) o;
    if (this->nodes.size() != other.nodes.size()) {
        return false;
    }
    for (size_t i = 0; i < this->nodes.size(); i++) {
        if (*this->nodes[i] != *other.nodes[i]) {
            return false;
        }
    }
    return true;
}

std::unique_ptr<Common> Block::clone() const {
    return std::make_unique<Block>(*this);
}

Block::Block(const Block& other) : sem::Common(CommonType::BLOCK) {
    this->unwrap = other.unwrap;
    for (auto& l: other.locals) {
        this->locals.emplace_back(l);
    }
    for (auto& n: other.nodes) {
        this->nodes.push_back(n->clone());
    }
}
