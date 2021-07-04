//
// Created by chris on 31/1/21.
//

#include "DictNode.h"

bool DictNode::equal(const Node& other) const {
    const auto& o = (DictNode&) other;
    if (this->items.size() != o.items.size()) {
        return false;
    }
    for (size_t i = 0; i < this->items.size(); i++) {
        auto p = this->items[i];
        auto op = o.items[i];
        if (*p.first != *op.first) {
            return false;
        }
        if (*p.second != *op.second) {
            return false;
        }
    }
    return true;
}

nlohmann::json DictNode::to_json() {
    nlohmann::json j;
    j["type"] = "dict";
    std::vector<nlohmann::json> v;
    for (auto i: this->items) {
        v.push_back({{"key",   i.first->to_json()},
                     {"value", i.second->to_json()}});
    }
    j["dict"]["items"] = v;
    return j;
}
