//
// Created by chris on 30/4/21.
//

#ifndef XLANG_ENUMDEF_H
#define XLANG_ENUMDEF_H


#include <string>
#include "../../common/include/Common.h"
#include "../../../types.h"

class sem::EnumDef : public sem::Common {
public:
    EnumDef(const std::string& id, VectorOfStrings values);

    std::string id;
    VectorOfStrings values;

    bool equals(const Common& o) const override;
};


#endif //XLANG_ENUMDEF_H
