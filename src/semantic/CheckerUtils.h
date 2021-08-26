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
#include "../simple_nodes/FunctionSNode.h"
#include "../simple_nodes/NewObjectSNode.h"
#include "../simple_nodes/IntegerSNode.h"
#include "../simple_nodes/IdSNode.h"
#include "../simple_nodes/IfSNode.h"
#include "../simple_nodes/CallSNode.h"
#include "../simple_nodes/ReturnSNode.h"
#include "../ast/nodes.h"
#include "SemanticInfo.h"
#include "../ast/ObjectType.h"
//
// Created by chris on 2/5/21.
//

#include "CheckerUtils.h"

std::string binoptype_to_str(OpType op);

int target_union_type(const ObjectType& target, const TypeNode& source);

Entity* entity_from_type(const TypeNode& type);

ast::Function* generate_eq_method(std::string class_name, VectorOfTypes tp, VectorOfStrings members_ordered);

ast::Function* generate_str_method(std::string class_name);

FunctionSNode* make_class_default_init(const std::string& class_path, const VectorOfStrings& members);

TypeNode* get_entity_type(Entity& e);

void mangle_generic_names(TypeNode* t);
void mangle_generic_names(FunctionType* t);
void mangle_generic_names(ObjectType* t);

void make_not_generic(FunctionType* ft);
void make_not_generic(ObjectType* ft);
void make_not_generic(TypeNode* ft);

std::string map_boolop_to_method_name(BoolOp op);
std::string map_binop_to_method_name(OpType op);


SNode* make_union_wrapper(int type_index, SNode* expression);


SNode* make_boolop_snode(ConstFunction* operator_fun, SemanticInfo& left_info, SemanticInfo& right_info);


#endif //XLANG_CHECKERUTILS_H
