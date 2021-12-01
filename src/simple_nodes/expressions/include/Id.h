//
// Created by chris on 4/4/21.
//

#ifndef XLANG_ID_H
#define XLANG_ID_H

#include <string>
#include "Exp.h"

class sem::Id : public sem::Exp {
public:
    std::string identifier;

    explicit Id(const std::string& identifier);
    bool equals(const Exp& o) const override;
    UExp clone() const override;
};


#endif //XLANG_ID_H
