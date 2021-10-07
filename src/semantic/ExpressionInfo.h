//
// Created by chris on 25/9/21.
//

#ifndef XLANG_EXPRESSIONINFO_H
#define XLANG_EXPRESSIONINFO_H

#include <string>
#include <vector>
#include "../ast/general/Type.h"
#include "../units/infos/ConcreteClass.h"
#include "../simple_nodes/common/include/Common.h"
#include "../units/infos/Module.h"
#include "../units/infos/Package.h"

class ExpressionInfo {
public:

    std::reference_wrapper<Entity> entity;
    std::unique_ptr<Entity> _entity;
    
    void set_entity(std::unique_ptr<Entity> e) {
        this->_entity = std::move(e);
        this->entity = *this->_entity;
    }

    bool is_tuple_member;
    ~ExpressionInfo();

    ExpressionInfo();

    bool is_constant;
    sem::Common* this_arg;

    bool is_error();
    sem::UExp exp_snode;
};

class ExpErrorStub : public ExpressionInfo {
public:
    ExpErrorStub();
};

typedef std::unique_ptr<ExpressionInfo> UExpressionInfo;



#endif //XLANG_EXPRESSIONINFO_H
