//
// Created by chris on 26/4/21.
//

#ifndef XLANG_BREAK_H
#define XLANG_BREAK_H


#include <vector>
#include <string>
#include "Common.h"

class sem::Break : public sem::Common {
public:
    std::vector<std::string> reachables;
    Break();

    bool equals(const Common& o) const override;
    std::unique_ptr<Common> clone() const override;
};


#endif //XLANG_BREAK_H
