//
// Created by chris on 28/6/20.
//

#ifndef CHECKER_H
#define CHECKER_H

#include <unordered_map>
#include "GlobalProcessor.h"
#include "../utils.h"
#include "../ast/nodes.h"
#include <set>
#include "../scanner/CodeLines.h"
#include <iostream>
#include <set>
#include "../macros.h"
#include "../logging/logging.h"
#include "../simple_nodes/Block.h"
#include "../units/ObjectValue.h"
#include "../units/FunctionValue.h"
#include "../ast/EnumNode.h"
#include "../simple_nodes/EnumDef.h"
#include "errors/ErrorReporter.h"
#include "util.h"
// #include "../logger/Logger.h"
#include "../simple_nodes/Block.h"
#include "../simple_nodes/Assignment.h"
#include "../simple_nodes/Return.h"
#include "../simple_nodes/Continue.h"
#include "../simple_nodes/Integer.h"
#include "../simple_nodes/FunctionDef.h"
#include "../simple_nodes/Declaration.h"
#include "../simple_nodes/Id.h"
#include "../simple_nodes/Call.h"
#include "../simple_nodes/String.h"
#include "../units/FunctionValue.h"
#include "../simple_nodes/Bool.h"
#include "../simple_nodes/Float.h"
#include "../simple_nodes/KlassDef.h"
#include "../simple_nodes/NewObject.h"
#include "../simple_nodes/While.h"
#include "../simple_nodes/Break.h"
#include "../simple_nodes/List.h"
#include "../simple_nodes/IfSNode.h"
#include "../simple_nodes/ObjectMember.h"
#include "../simple_nodes/Match.h"
#include "CheckerUtils.h"
#include "../ast/UnaryOp.h"
#include "../ast/ObjectType.h"

#define T_NONE ObjectType(".None")

typedef std::unique_ptr<SemanticInfo> USemanticInfo;

bool type_matches(TypeNode* a, TypeNode* b);
bool is_generic(const TypeNode& t);
MapStringType make_replacements(TypeNode* a, TypeNode* b);
UTypeNode make_type_from_object_pattern(const ObjectType& object_type, const MapStringType& replacements);
UTypeNode make_type_from_function_pattern(const FunctionType& ftn, const MapStringType& replacements);
UTypeNode make_type(const TypeNode& original, const MapStringType& replacements);
SemanticInfo match_arguments_to_generic_function(const FunctionType& function_type, VectorOfTypes arg_types);
USemanticInfo error_stub();
Entity* map_flirpin_to_entity(Flirpin flirpin);
Flirpin map_unit_to_flirpin(Unit u);
TextPosition add_one_col(TextPosition t);
bool function_is_generic(const FunctionType& ft);

sem::SNode* make_for_snode(ast::For& node, USemanticInfo& binfo, USemanticInfo& exp_info_p, std::string loop_list_var_id,
                           std::string loop_index_var_id, std::string loop_list_len_var_id, sem::SNode* update_loop_index_snode);

class Checker {
    int loop_count;
    bool add_this;
    std::map<std::string, std::unique_ptr<EntityValue>> entity_values_no_generic;
    std::unordered_map<std::string, SymbolTable*> scopes;
    SymbolTable* scope;
public:
    Module& module;
    bool is_call;
    ErrorReporter error_reporter;
    Entity* this_entity;
    Package& top_package;
    sem::SNode* update_loop_index_snode;

    Checker(Package& top_package, Module& module);
    ~Checker();

    bool is_immutable(const TypeNode& node);
    void enter_scope(const std::string& name);
    void leave_scope();
    bool assert_type_exists(const TypeNode& type, TextPosition pos);
    Class* instantiate_generic(Class* generic, const ObjectType& instance);
    bool is_variable(const ObjectType& a);
    std::pair<std::string, TypeNode*>*
    get_first_substitution_function(FunctionType& a, FunctionType& b, bool is_top_level_arg);
    std::pair<std::string, TypeNode*>*
    get_first_substitution_object(ObjectType& a, ObjectType& b, bool is_top_level_arg);
    std::pair<std::string, TypeNode*>* get_first_substitution(TypeNode& a, TypeNode& b, bool is_top_level_arg);
    UTypeNode substitute(const TypeNode& t, const std::string& var, const TypeNode& replacement);
    std::unique_ptr<FunctionType> unify_function_call(const FunctionType& fun, VectorOfTypes& args,
                                                      std::map<std::string, TypeNode*>& all_substitutions);
    std::unique_ptr<SemanticInfo> match_arguments_to_generic_function(const FunctionType& ft, VectorOfTypes arg_types,
                                                                      std::map<std::string, TypeNode*>& all_substitutions);
    void fail(std::string msg);

    USemanticInfo dispatch_rvalue(ast::Node& nod);

    USemanticInfo visit_assignment(ast::Assignment& n);
    USemanticInfo visit_binop(ast::BinaryOp& node);
    USemanticInfo visit_block(ast::Block& node);
    USemanticInfo visit_root(ast::Block& node);
    USemanticInfo visit_boolean(ast::Boolean& node);
    USemanticInfo visit_break(ast::Break& node);
    USemanticInfo visit_call(ast::Call& n, bool is_rvalue);
    USemanticInfo visit_class(ast::Klass& node);
    USemanticInfo visit_continue(ast::Continue& node);

    USemanticInfo visit_lvalue_subscript(ast::Subscript& node);

    USemanticInfo visit_declaration(ast::Declaration& n);
    USemanticInfo check_declaration_with_type(ast::Declaration& n);
    USemanticInfo check_declaration_without_type(ast::Declaration& n);

    USemanticInfo visit_dict(ast::DictNode& node);
    USemanticInfo visit_emptydict(ast::EmptyDict& node);
    USemanticInfo visit_emptylist(ast::EmptyList& node);
    USemanticInfo visit_unary(ast::UnaryOp& n);
    USemanticInfo visit_for(ast::For& node);
    USemanticInfo visit_function(ast::Function& n);
    USemanticInfo visit_id(ast::Id& n);
    USemanticInfo visit_if(ast::If& n);
    USemanticInfo visit_list(ast::List& node);
    USemanticInfo visit_member(ast::Member& n);
    USemanticInfo visit_none(ast::None& node);
    USemanticInfo visit_import(ast::Import& node);
    USemanticInfo visit_number(ast::Number& node);
    USemanticInfo visit_partial(ast::PartialApplication& node);
    USemanticInfo visit_return(ast::Return& n);
    USemanticInfo visit_string(ast::String& node);
    USemanticInfo visit_subscript(ast::Subscript& node);
    USemanticInfo visit_ternary(ast::Ternary& node);
    USemanticInfo visit_tuple(ast::Tuple& node);
    USemanticInfo visit_while(ast::While& node);
    USemanticInfo visit_cast(ast::Cast& n);
    USemanticInfo visit_defconst(ast::DefaultConstructor& node);


    USemanticInfo object_member(sem::SNode* object_snode, Value& p_value, const std::string& child, ast::Member& n);
    USemanticInfo class_member(Class* cls, const std::string& child, ast::Member& n);
    USemanticInfo package_member(Package& package, const std::string& child, ast::Member& n);
    USemanticInfo module_member(Module& mod, const std::string& child, ast::Member& n);


    USemanticInfo visit_match(ast::Match& node);
    USemanticInfo visit_alias(ast::Alias& p_node);
    USemanticInfo enum_member(Enum* enumm, const std::string& value, ast::Member& node);
    USemanticInfo visit_enum(ast::EnumNode& p_node);
    sem::SNode* make_rvalue(const Entity& value_entity, sem::SNode* value_snode, const TypeNode& target);
    USemanticInfo dispatch(ast::Node& nod);
    void fill_value(Value& value);
    std::unique_ptr<SemanticInfo> expect_rvalue_of_type(const TypeNode& target, ast::Node& node);
    void process_function_arguments(SemanticInfo& retv, std::vector<Entity*>& arg_entities, sem::Call* sn, ast::Call& n,
                                    FunctionType* function_type, SemanticInfo* fun_info_p);
    bool check_arguments(ast::Call& n, sem::Call* sn, VectorOfTypes& arg_types, std::vector<Entity*>& arg_entities);
    USemanticInfo
    make_return_info(const ast::Call& n, bool is_rvalue, SemanticInfo& retv, bool is_def_const, bool args_are_constant);
    sem::SNode* make_union_rvalue(sem::SNode* value_snode, const TypeNode* unaliased_value_type,
                             const TypeNode* unaliased_target_type) const;
    sem::SNode* make_option_rvalue(sem::SNode* value_snode, const TypeNode* unaliased_value_type,
                              const TypeNode* unaliased_target_type) const;
    // USemanticInfo visit_throw(ast::ThrowNode& n);
    void init();
    USemanticInfo dispatch_any(ast::Node& n, bool is_rvalue);
    EntityValue& entity_value_from_actual_base_path_no_generic(const Path& p);
};

#endif //CHECKER_H


