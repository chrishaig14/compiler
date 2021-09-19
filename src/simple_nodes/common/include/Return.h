//
// Created by chris on 4/4/21.
//

#ifndef XLANG_RETURN_H
#define XLANG_RETURN_H


#include "Common.h"
#include <vector>
#include <string>

class sem::Return : public sem::Common {
public:
    UExp expression;
    std::vector<std::string> reachables;
    explicit Return(UExp expression);
    bool equals(const Common& o) const override;
};


#endif //XLANG_RETURN_H
