//
// Created by chris on 31/8/21.
//

#ifndef XLANG_CONSTFUNCTION_H
#define XLANG_CONSTFUNCTION_H

#include "../../sem.h"
#include "Exp.h"
#include "../../../ast/Path.h"

class sem::ConstFunction : public sem::Exp {
public:
    Path path;

    ConstFunction(Path path);

    bool equals(const Exp& o) const override;

};

#endif //XLANG_CONSTFUNCTION_H
