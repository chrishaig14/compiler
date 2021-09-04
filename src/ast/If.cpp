//
// Created by chris on 1/8/20.
//

#include <cassert>
#include "If.h"

using namespace ast;

bool If::equal(const ast::Node& x) const {
    const auto& other = (If&) x;
    if (this->condition != other.condition) {
        return false;
    }
    if (this->then != other.then) {
        return false;
    }
    if (this->elifs.size() != other.elifs.size()) {
        return false;
    }
    for (size_t i = 0; i < this->elifs.size(); i++) {
        if (this->elifs[i].first.get() != other.elifs[i].first.get()) {
            return false;
        }
        if (this->elifs[i].second.get() != other.elifs[i].second.get()) {
            return false;
        }
    }
    if ((this->selse != nullptr && other.selse == nullptr) || (this->selse == nullptr && other.selse != nullptr)) {
        return false;
    }
    return (this->selse == nullptr && other.selse == nullptr) || *this->selse == *other.selse;

}

If::If(UNode condition, std::unique_ptr<ast::Block> then, std::vector<std::pair<UNode, UBlock>> elifs,
       std::unique_ptr<ast::Block> selse, TextPosition start, TextPosition end) : ast::Node(NodeType::IFF, start, end),
                                                                                   _condition(std::move(condition)),
                                                                                   _then(std::move(then)),
                                                                                   _elifs(std::move(elifs)),
                                                                                   selse(std::move(selse)),
                                                                                   then(*_then),
                                                                                   condition(*_condition) {
    for (auto& e: this->_elifs) {
        this->elifs.emplace_back(*e.first, *e.second);
    }
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
    std::vector<nlohmann::json> elifs_j;
    for (auto& e: this->elifs) {
        elifs_j.push_back({{"condition", e.first.get().to_json()},
                         {"then",      e.second.get().to_json()}});
    }
    nlohmann::json j = {{"type", "if"}};
    j["if"]["condition"] = this->condition.to_json();
    j["if"]["then"] = this->then.to_json();
    j["if"]["elifs"] = elifs_j;
    j["if"]["else"] = this->selse != nullptr ? this->selse->to_json() : nlohmann::json();
    return j;
}

