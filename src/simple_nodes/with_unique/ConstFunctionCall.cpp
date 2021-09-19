//
// Created by chris on 30/8/21.
//

#include "ConstFunctionCall.h"
#include "../expressions/include/Exp.h"

bool sem::ConstFunctionCall::equals(const sem::SNode& o) const {
    auto& other = (const ConstFunctionCall&) o;
    bool path_ok = this->path.as_str() == other.path.as_str();
    bool args_ok = true;
    if (this->args.size() != other.args.size()) {
        return false;
    }
    for (size_t i = 0; i < this->args.size(); i++) {
        if (*this->args[i] != *other.args[i]) {
            return false;
        }
    }
    return path_ok and args_ok;
}

sem::ConstFunctionCall::ConstFunctionCall(Path path, std::vector<UExp> args)
        : SNode(SNodeType::CONST_FUNCTION_CALL), path(path), args(std::move(args)) {
}
