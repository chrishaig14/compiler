//
// Created by chris on 26/9/21.
//

#ifndef XLANG_FOR_H
#define XLANG_FOR_H

#include "Common.h"
#include "Block.h"

class sem::For : public sem::Common {
public:
    std::string varname;
    UExp expression;
    Block body;

    For(const std::string& varname, UExp expression, const Block& body);

    For(const For& other);

    bool equals(const Common& o) const override;
    std::unique_ptr<Common> clone() const override;

};

#endif //XLANG_FOR_H
