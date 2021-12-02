//
// Created by chris on 4/4/21.
//

#ifndef XLANG_BLOCK_H
#define XLANG_BLOCK_H

#include "Common.h"
#include <vector>
#include <string>

class sem::Block : public sem::Common {
public:
    bool unwrap;
    std::vector<std::string> locals;
    std::vector<UCommon> nodes;
    Block();
    Block(const Block& other);

    bool equals(const Common& o) const override;
    std::unique_ptr<Common> clone() const override;
};


#endif //XLANG_BLOCK_H
