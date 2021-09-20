//
// Created by chris on 1/8/20.
//

#include "Call.h"
#include "../../json/json.hpp"

using namespace ast;

Call::Call(ast::UExpNode function, ast::VectorOfExpNodesU arguments, TextPosition start, TextPosition end) : ast::CommonNode(CommonNodeType::CALL,
                                                                                                                             start,
                                                                                                                             end),
                                                                                             _function(std::move(
                                                                                                     function)),
                                                                                             _arguments(std::move(
                                                                                                     arguments)),
                                                                                             function(*_function) {
    for (auto& a: this->_arguments) {
        this->arguments.push_back(*a);
    }
}

bool Call::equal(const ast::CommonNode& x) const {
    const auto& other = (Call&) x;
    if (this->arguments.size() != other.arguments.size()) {
        return false;
    }
    for (size_t i = 0; i < this->arguments.size(); ++i) {
        if (this->arguments[i].get() != other.arguments[i].get()) {
            return false;
        }
    }
    return this->function == other.function;
}

Call::~Call() {
    // delete this->function;
    // for (auto* a: this->arguments) {
    //     delete a;
    // }
    // for (auto* at: this->arg_types) {
    //     delete at;
    // }
    // for (auto* ft: this->ftype) {
    //     delete ft;
    // }
}

nlohmann::json Call::to_json() const {
    nlohmann::json j;
    j["type"] = "call";
    j["call"]["function"] = this->function.to_json();
    std::vector<nlohmann::json> v;
    for (auto& e: this->arguments) {
        v.push_back(e.get().to_json());
    }
    j["call"]["arguments"] = v;
    return j;
}
