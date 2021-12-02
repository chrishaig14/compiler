//
// Created by chris on 19/6/21.
//

#ifndef XLANG_THROW_H
#define XLANG_THROW_H


#include "Common.h"
#include <vector>
#include <string>

class sem::Throw : public sem::Common {
public:
    Common* expression;
    std::vector<std::string> reachables;
    Throw(Common* expression);
    bool equals(const Common& o) const override;
    std::unique_ptr<Common> clone() const override;
};


#endif //XLANG_THROW_H
