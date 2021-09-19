//
// Created by chris on 26/4/21.
//

#ifndef XLANG_CONTINUE_H
#define XLANG_CONTINUE_H


#include "Common.h"
#include <vector>
#include <string>

class sem::Continue : public sem::Common {
public:
    std::vector<std::string> reachables;
    Continue();

    bool equals(const Common& o) const override;
};


#endif //XLANG_CONTINUE_H
