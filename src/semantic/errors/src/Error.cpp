//
// Created by chris on 8/8/21.
//

#include "../include/Error.h"
#include "../../../units/entities/Entity.h"
#include "../../../units/entities/EntityPackage.h"
#include "../../../units/entities/EntityModule.h"
#include "../../../units/entities/EntityClass.h"
#include "../../../units/entities/EntityValue.h"
#include "../../../units/entities/EntityConstFunction.h"
#include "../../../units/entities/EntityEnum.h"
#include "../../../units/entities/EntityNothing.h"
#include "../../../units/entities/EntityNotFound.h"

std::string entity_to_string(const Entity& entity) {
    std::string out;
    switch (entity.e_type) {
        case E_TYPE::PACKAGE:
            return "package " + E_HLT( entity.get_package().package.name);
        case E_TYPE::MODULE:
            return "module " + E_HLT(entity.get_module().module.name);
        case E_TYPE::CLASS:
            return "class " + E_HLT(entity.get_class().clazz.class_name);
        case E_TYPE::VALUE:
            return E_HLT(entity.get_value().type.to_string());
        case E_TYPE::CONST_FUNCTION:
            return E_HLT(entity.get_constfun().const_function.const_function_ft.to_string());
            // return "function " + E_HLT(entity.const_function->path.as_vec().back());
        case E_TYPE::ERROR:
            return "ERROR";
        case E_TYPE::NOT_FOUND:
            return "NOT FOUND";
        case E_TYPE::ENUM:
            return "enum " + E_HLT(entity.get_enum().enumm.enumm_name);
        case E_TYPE::NOTHING:
            return "NOTHING";
        case E_TYPE::NONE:
            return "NONE";
            break;
        case E_TYPE::TYPECLASS:
            return "typeclass";
            break;
        case E_TYPE::TEMPLATE_CLASS:
            return "template class";
            break;
    }
    return out;
}

bool Error::operator==(const Error& other) const {
    if (typeid(*this) != typeid(other)) {
        std::cout << "Types are different!" << std::endl;
        return false;
    }
    return this->equal(other);
}
