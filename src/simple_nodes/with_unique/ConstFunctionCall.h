//
// Created by chris on 30/8/21.
//

#ifndef XLANG_CONSTFUNCTIONCALL_H
#define XLANG_CONSTFUNCTIONCALL_H

#include <vector>
#include "../sem.h"
#include "../common/include/SNode.h"
#include "../../ast/Path.h"

class sem::ConstFunctionCall : public sem::SNode {
public:
    Path path;
    std::vector<UExp> args;

    ConstFunctionCall(Path path, std::vector<UExp> args);

    bool equals(const SNode& o) const override;

};

#endif //XLANG_CONSTFUNCTIONCALL_H
