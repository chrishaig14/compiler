//
// Created by chris on 1/8/20.
//

#include "ListNode.h"
#include "../json/json.hpp"
using namespace ast;
ListNode::ListNode(VectorOfNodesU& elements, TextPosition start, TextPosition end) : ast::Node(NodeType::LST, start, end),
                                                                                     elements(std::move(elements)) {
}

bool ListNode::equal(const ast::Node& other) const {
    if (this->elements.size() != ((ListNode&) other).elements.size()) {
        return false;
    }
    for (size_t i = 0; i < this->elements.size(); ++i) {
        if (*this->elements[i] != *((ListNode&) other).elements[i]) {
            return false;
        }
    }
    return true;
}

ListNode::~ListNode() {
    // for (auto* e: this->elements) {
    //     delete e;
    // }
}

nlohmann::json ListNode::to_json() const {
    nlohmann::json j;
    j["type"] = "list";
    std::vector<nlohmann::json> v;
    for (auto& x: this->elements) {
        v.push_back(x->to_json());
    }
    j["list"]["elements"] = v;
    return j;
}

