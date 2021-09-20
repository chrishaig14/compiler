//
// Created by chris on 2/5/21.
//

#ifndef XLANG_CHECKERUTILS_H
#define XLANG_CHECKERUTILS_H

#include "../units/Entity.h"
#include "../units/FunctionValue.h"
#include "../units/ObjectValue.h"
#include "../ast/top/Function.h"
#include "../simple_nodes/common/include/Common.h"
#include "../simple_nodes/top/include/FunctionDef.h"
#include "../simple_nodes/expressions/include/NewObject.h"
#include "../simple_nodes/expressions/include/Integer.h"
#include "../simple_nodes/expressions/include/Id.h"
#include "../simple_nodes/common/include/If.h"
#include "../simple_nodes/common/include/Call.h"
#include "../simple_nodes/common/include/Return.h"
#include "../ast/nodes.h"
#include "SemanticInfo.h"
#include "../ast/ObjectType.h"
//
// Created by chris on 2/5/21.
//

#include "CheckerUtils.h"

std::string binoptype_to_str(OpType op);

int target_union_type(const sem::TypeObject& target, const sem::Type& source);

Entity* entity_from_type(const ast::Type& type);

ast::Function* generate_eq_method(std::string class_name, ast::VectorOfTypes tp, VectorOfStrings members_ordered);

ast::Function* generate_str_method(std::string class_name);

sem::FunctionDef* make_class_default_init(const std::string& class_path, const VectorOfStrings& members);

ast::Type* get_entity_type(Entity& e);

void mangle_generic_names(ast::Type& t);
void mangle_generic_names(ast::FunctionType& t);
void mangle_generic_names(ast::ObjectType& t);

void make_not_generic(ast::FunctionType& ft);
void make_not_generic(ast::ObjectType& ot);
void make_not_generic(ast::Type& t);

sem::UExp make_union_wrapper(int type_index, sem::UExp expression);


sem::UExp make_boolop_snode(ConstFunction* operator_fun, SemanticInfo& left_info, SemanticInfo& right_info);


#endif //XLANG_CHECKERUTILS_H
