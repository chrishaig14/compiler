//
// Created by chris on 18/9/21.
//

#ifndef XLANG_EXP_H
#define XLANG_EXP_H

#include "../../sem.h"

class sem::Exp {
public:
    ExpType type;

    explicit Exp(ExpType type);

    virtual bool equals(const Exp& o) const = 0;

    bool operator!=(const Exp& other) const;

    bool operator==(const Exp& other) const;

    virtual ~Exp() = default;
};

#endif //XLANG_EXP_H
