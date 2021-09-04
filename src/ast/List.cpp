//
// Created by chris on 1/8/20.
//

#include "List.h"
#include "../json/json.hpp"

using namespace ast;

List::List(ast::VectorOfNodesU elements, TextPosition start, TextPosition end) : ast::Node(NodeType::LST, start, end),
                                                                            _elements(std::move(elements)) {
    for (auto& e: this->_elements) {
        this->elements.push_back(*e);
    }
}

bool List::equal(const ast::Node& other) const {
    if (this->elements.size() != ((List&) other).elements.size()) {
        return false;
    }
    for (size_t i = 0; i < this->elements.size(); ++i) {
        if (this->elements[i].get() != ((List&) other).elements[i].get()) {
            return false;
        }
    }
    return true;
}

List::~List() {
    // for (auto* e: this->elements) {
    //     delete e;
    // }
}

nlohmann::json List::to_json() const {
    nlohmann::json j;
    j["type"] = "list";
    std::vector<nlohmann::json> v;
    for (auto& x: this->elements) {
        v.push_back(x.get().to_json());
    }
    j["list"]["elements"] = v;
    return j;
}

