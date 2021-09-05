//
// Created by chris on 2/5/21.
//

#ifndef XLANG_CHECKERUTILS_H
#define XLANG_CHECKERUTILS_H

#include "../units/Entity.h"
#include "../units/FunctionValue.h"
#include "../units/ObjectValue.h"
#include "../ast/Function.h"
#include "../simple_nodes/SNode.h"
#include "../simple_nodes/with_unique/FunctionDef.h"
#include "../simple_nodes/NewObject.h"
#include "../simple_nodes/with_unique/Integer.h"
#include "../simple_nodes/with_unique/Id.h"
#include "../simple_nodes/with_unique/IfSNode.h"
#include "../simple_nodes/with_unique/Call.h"
#include "../simple_nodes/with_unique/Return.h"
#include "../ast/nodes.h"
#include "SemanticInfo.h"
#include "../ast/TypeObject.h"
//
// Created by chris on 2/5/21.
//

#include "CheckerUtils.h"

std::string binoptype_to_str(OpType op);

int target_union_type(const ast::ObjectType& target, const ast::Type& source);

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

USNode make_union_wrapper(int type_index, USNode expression);


USNode make_boolop_snode(ConstFunction* operator_fun, SemanticInfo& left_info, SemanticInfo& right_info);


#endif //XLANG_CHECKERUTILS_H
