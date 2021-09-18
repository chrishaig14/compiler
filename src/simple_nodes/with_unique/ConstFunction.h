//
// Created by chris on 31/8/21.
//

#ifndef XLANG_CONSTFUNCTION_H
#define XLANG_CONSTFUNCTION_H

#include "../common/sem.h"
#include "../common/SNode.h"
#include "../../ast/Path.h"

class sem::ConstFunction : public sem::SNode {
public:
    Path path;

    ConstFunction(Path path) : SNode(SNodeType::CONST_FUNCTION), path(path) {
    }

    bool equals(const SNode& o) const override {
        auto& other = (const ConstFunction&) o;
        bool path_ok = this->path.as_str() == other.path.as_str();
        return path_ok;
    }

};

#endif //XLANG_CONSTFUNCTION_H
