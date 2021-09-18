//
// Created by chris on 18/9/21.
//

#ifndef XLANG_OBJECTCONSTRUCTOR_H
#define XLANG_OBJECTCONSTRUCTOR_H


#include "../common/SNode.h"
#include "../../ast/Path.h"

class sem::ObjectConstructor : public sem::SNode {
public:
    Path class_path;

    explicit ObjectConstructor(Path class_path);
    [[nodiscard]] bool equals(const SNode& o) const override;
};


#endif //XLANG_OBJECTCONSTRUCTOR_H
