//
// Created by chris on 1/8/20.
//

#include "CallExp.h"
#include "../Call.h"
#include "../../json/json.hpp"

using namespace ast;

CallExp::CallExp(ast::UExpNode function, ast::VectorOfExpNodesU arguments, TextPosition start, TextPosition end)
        : ast::ExpNode(ExpNodeType::CALL, start, end), _function(std::move(function)), _arguments(std::move(arguments)),
          function(*_function) {
    for (auto& a: this->_arguments) {
        this->arguments.push_back(*a);
    }
}

bool CallExp::equal(const ast::ExpNode& x) const {
    const auto& other = (CallExp&) x;
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

CallExp::~CallExp() {
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

nlohmann::json CallExp::to_json() const {
    nlohmann::json j;
    j["type"] = "CallExp";
    j["CallExp"]["function"] = this->function.to_json();
    std::vector<nlohmann::json> v;
    for (auto& e: this->arguments) {
        v.push_back(e.get().to_json());
    }
    j["CallExp"]["arguments"] = v;
    return j;
}

std::unique_ptr<ast::Call> CallExp::to_call() {
    return std::make_unique<ast::Call>(std::move(this->_function), std::move(this->_arguments), this->start, this->end);
}
