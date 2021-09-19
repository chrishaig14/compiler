//
// Created by chris on 4/4/21.
//

#ifndef XLANG_COMMON_H
#define XLANG_COMMON_H

#include <memory>
#include "../../sem.h"



class sem::Common {
public:
    CommonType type;
    explicit Common(CommonType type);

    virtual bool equals(const Common& o) const = 0;

    bool operator!=(const Common& other) const;

    bool operator==(const Common& other) const;

    virtual ~Common() = default;
};

#endif //XLANG_COMMON_H
