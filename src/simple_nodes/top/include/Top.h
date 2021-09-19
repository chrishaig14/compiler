//
// Created by chris on 4/4/21.
//

#ifndef XLANG_TOP_H
#define XLANG_TOP_H

#include <memory>
#include "../../sem.h"


class sem::Top {
public:
    TopType type;
    explicit Top(TopType type);

    virtual bool equals(const Top& o) const = 0;

    bool operator!=(const Top& other) const;

    bool operator==(const Top& other) const;

    virtual ~Top() = default;
};

#endif //XLANG_TOP_H
