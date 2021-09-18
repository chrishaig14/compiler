//
// Created by chris on 18/9/21.
//

#ifndef XLANG_OBJECTCONSTRUCTORCALL_H
#define XLANG_OBJECTCONSTRUCTORCALL_H


#include "../../common/include/SNode.h"
#include "../../../ast/Path.h"

class sem::ObjectConstructorCall : public sem::SNode {
public:
    Path class_path;
    std::vector<USNode> arguments;
    ObjectConstructorCall(Path class_path, std::vector<USNode> arguments);
    bool equals(const SNode& o) const override;
};


#endif //XLANG_OBJECTCONSTRUCTORCALL_H
