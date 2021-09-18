//
// Created by chris on 30/8/21.
//

#ifndef XLANG_CONSTFUNCTIONCALL_H
#define XLANG_CONSTFUNCTIONCALL_H

#include <vector>
#include "../common/include/sem.h"
#include "../common/include/SNode.h"
#include "../../ast/Path.h"

class sem::ConstFunctionCall : public sem::SNode {
public:
    Path path;
    std::vector<USNode> args;

    ConstFunctionCall(Path path, std::vector<USNode> args)
            : SNode(SNodeType::CONST_FUNCTION_CALL), path(path), args(std::move(args)) {
    }

    bool equals(const SNode& o) const override {
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

};

#endif //XLANG_CONSTFUNCTIONCALL_H
