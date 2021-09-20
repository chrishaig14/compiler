//
// Created by chris on 31/1/21.
//

#include "DictNode.h"

using namespace ast;

bool DictNode::equal(const ast::ExpNode& other) const {
    const auto& o = (DictNode&) other;
    if (this->items.size() != o.items.size()) {
        return false;
    }
    for (size_t i = 0; i < this->items.size(); i++) {
        auto& p = this->items[i];
        auto& op = o.items[i];
        if (p.first.get() != op.first.get()) {
            return false;
        }
        if (p.second.get() != op.second.get()) {
            return false;
        }
    }
    return true;
}

nlohmann::json DictNode::to_json() const {
    nlohmann::json j;
    j["type"] = "dict";
    std::vector<nlohmann::json> v;
    for (auto& i: this->items) {
        v.push_back({{"key",   i.first.get().to_json()},
                     {"value", i.second.get().to_json()}});
    }
    j["dict"]["items"] = v;
    return j;
}

DictNode::DictNode(std::vector<std::pair<ast::UExpNode, ast::UExpNode>> items, TextPosition start, TextPosition end) : ast::ExpNode(
        ExpNodeType::DICT,
        start,
        end), _items(std::move(items)) {
    for (auto& i: this->_items) {
        this->items.emplace_back(*i.first, *i.second);
    }
}
