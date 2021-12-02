//
// Created by chris on 4/4/21.
//

#ifndef XLANG_CALL_H
#define XLANG_CALL_H


#include <vector>
#include "Common.h"

class sem::Call : public sem::Common {
public:
    UExp function;
    std::vector<UExp> arguments;

    Call(const Call& other);

    Call(UExp function, std::vector<UExp> arguments);

    bool equals(const Common& o) const override;
    std::unique_ptr<Common> clone() const override;
};


#endif //XLANG_CALL_H
