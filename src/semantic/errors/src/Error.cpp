//
// Created by chris on 8/8/21.
//

#include "../include/Error.h"
#include "../../../units/Entity.h"

std::string entity_to_string(const Entity& entity) {
    std::string out;
    switch (entity.type) {
        case E_TYPE::PACKAGE:
            return "package " + E_HLT(((EntityPackage&) entity).package.name);
        case E_TYPE::MODULE:
            return "module " + E_HLT(((EntityModule&) entity).module->name);
        case E_TYPE::CLASS:
            return "class " + E_HLT(((EntityClass&) entity).clazz->class_name);
        case E_TYPE::VALUE:
            return E_HLT(((Value&) entity).type.to_string());
        case E_TYPE::CONST_FUNCTION:
            return E_HLT(((EntityConstFunction&) entity).const_function.const_function_ft.to_string());
            // return "function " + E_HLT(entity.const_function->path.as_vec().back());
        case E_TYPE::ERROR:
            return "ERROR";
        case E_TYPE::NOT_FOUND:
            return "NOT FOUND";
        case E_TYPE::ENUM:
            return "enum " + E_HLT(((EntityEnum&) entity).enumm->enumm_name);
        case E_TYPE::NOTHING:
            return "NOTHING";
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
