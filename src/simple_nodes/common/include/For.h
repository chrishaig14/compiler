//
// Created by chris on 26/9/21.
//

#ifndef XLANG_FOR_H
#define XLANG_FOR_H

#include "Common.h"

class sem::For : public sem::Common {
public:
    std::string varname;
    UExp expression;
    std::unique_ptr<Block> body;

    For(const std::string& varname, UExp expression, std::unique_ptr<Block> body);

    bool equals(const Common& o) const override;

};

#endif //XLANG_FOR_H
