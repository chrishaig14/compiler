//
// Created by chris on 4/4/21.
//

#ifndef XLANG_ASSIGNMENT_H
#define XLANG_ASSIGNMENT_H

#include "Common.h"
#include "../../expressions/include/Exp.h"

class sem::Assignment : public sem::Common {
public:
    UExp lvalue;
    UExp rvalue;
    Assignment(UExp lvalue, UExp rvalue);

    bool equals(const Common& o) const override;
};


#endif //XLANG_ASSIGNMENT_H
