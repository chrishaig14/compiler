//
// Created by chris on 24/3/21.
//

#ifndef XLANG_CAST_H
#define XLANG_CAST_H

#include "../common/CommonNode.h"
#include "../optypes.h"
#include "../general/Type.h"
#include "../ast.h"

class ast::Cast : public ast::CommonNode {
public:
    CommonNode* exp;
    std::string as_type;


    Cast(CommonNode* exp, std::string as_type, TextPosition start, TextPosition end);

    bool equal(const ast::CommonNode& x) const override;

    ~Cast();

    ast::Type* exp_type;
    ast::Type* rtype;
};


#endif //XLANG_CAST_H
