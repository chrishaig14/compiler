//
// Created by chris on 1/8/20.
//

#include <cassert>
#include "If.h"

using namespace ast;

bool If::equal(const ast::Node& x) const {
    const auto& other = (If&) x;
    if (*this->condition != *other.condition) {
        return false;
    }
    if (*this->then != *other.then) {
        return false;
    }
    if (this->elifs.size() != other.elifs.size()) {
        return false;
    }
    for (size_t i = 0; i < this->elifs.size(); i++) {
        if (*this->elifs[i].first != *other.elifs[i].first) {
            return false;
        }
        if (*this->elifs[i].second != *other.elifs[i].second) {
            return false;
        }
    }
    if ((this->selse != nullptr && other.selse == nullptr) || (this->selse == nullptr && other.selse != nullptr)) {
        return false;
    }
    return (this->selse == nullptr && other.selse == nullptr) || *this->selse == *other.selse;

}

If::If(UNode& condition, std::unique_ptr<ast::Block>& then, std::vector<std::pair<Node*, ast::Block*>> elifs,
               std::unique_ptr<ast::Block>& selse, TextPosition start, TextPosition end) : ast::Node(NodeType::IFF,
                                                                                                start,
                                                                                                end),
                                                                                           condition(std::move(condition)),
                                                                                           then(std::move(then)),
                                                                                           selse(std::move(selse)),
                                                                                           elifs(elifs) {
    // assert(condition != nullptr);
    // assert(then != nullptr);
}

If::~If() {
    // delete this->condition;
    // delete this->then;
    //
    // delete this->selse;
    //
    // for (auto p: this->elifs) {
    //     delete p.first;
    //     delete p.second;
    // }
}

nlohmann::json If::to_json() const {
    std::vector<nlohmann::json> elifs;
    for (auto e: this->elifs) {
        elifs.push_back({{"condition", e.first->to_json()},
                         {"then",      e.second->to_json()}});
    }
    nlohmann::json j = {{"type", "if"}};
    j["if"]["condition"] = this->condition->to_json();
    j["if"]["then"] = this->then->to_json();
    j["if"]["elifs"] = elifs;
    j["if"]["else"] = this->selse != nullptr ? this->selse->to_json() : nlohmann::json();
    return j;
}

