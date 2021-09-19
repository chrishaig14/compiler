//
// Created by chris on 28/6/20.
//

#ifndef CHECKER_H
#define CHECKER_H

#include <iostream>
#include <set>
#include <set>
#include <unordered_map>

#include "../ast/EnumNode.h"
#include "../ast/Module.h"
#include "../ast/nodes.h"
#include "../ast/ObjectType.h"
#include "../ast/UnaryOp.h"
#include "../logging/logging.h"
#include "../macros.h"
#include "../scanner/CodeLines.h"
#include "../simple_nodes/common/include/Block.h"
#include "../simple_nodes/common/include/Block.h"
#include "../simple_nodes/common/include/Call.h"
#include "../simple_nodes/common/include/Declaration.h"
#include "../simple_nodes/common/include/KlassDef.h"
#include "../simple_nodes/common/include/Match.h"
#include "../simple_nodes/common/include/NewObject.h"
#include "../simple_nodes/common/include/Return.h"
#include "../simple_nodes/common/include/TypeObject.h"
#include "../simple_nodes/common/src/TypeFunction.h"
#include "../simple_nodes/expressions/include/expressions.h"
#include "../simple_nodes/with_unique/Assignment.h"
#include "../simple_nodes/with_unique/Break.h"
#include "../simple_nodes/with_unique/Continue.h"
#include "../simple_nodes/with_unique/EnumDef.h"
#include "../simple_nodes/with_unique/FunctionDef.h"
#include "../simple_nodes/with_unique/IfSNode.h"
#include "../simple_nodes/with_unique/While.h"
#include "../units/FunctionValue.h"
#include "../units/FunctionValue.h"
#include "../units/ObjectValue.h"
#include "../utils.h"
#include "CheckerUtils.h"
#include "errors/ErrorReporter.h"
#include "GlobalProcessor.h"
#include "util.h"

// #include "../logger/Logger.h"

#define T_NONE ast::ObjectType(".None")

typedef std::unique_ptr<SemanticInfo> USemanticInfo;
typedef std::unique_ptr<SemanticInfoBlock> USemanticInfoBlock;

bool is_generic(const sem::Type& t);
ast::UTypeNode make_type_from_object_pattern(const ast::ObjectType& object_type, const MapStringType& replacements);
ast::UTypeNode make_type_from_function_pattern(const ast::FunctionType& ftn, const MapStringType& replacements);
ast::UTypeNode make_type(const ast::Type& original, const MapStringType& replacements);
SemanticInfo match_arguments_to_generic_function(const ast::FunctionType& function_type, ast::VectorOfTypes arg_types);
USemanticInfo error_stub();
Entity* map_module_member_to_entity(ModuleMember module_member);
ModuleMember map_unit_to_module_member(Unit u);
TextPosition add_one_col(TextPosition t);
bool function_is_generic(const sem::TypeFunction& ft);

sem::SNode*
make_for_snode(ast::For& node, USemanticInfoBlock& binfo, USemanticInfo& exp_info_p, std::string loop_list_var_id,
               std::string loop_index_var_id, std::string loop_list_len_var_id, sem::SNode* update_loop_index_snode);
const sem::TypeFunction& get_function_type(const SemanticInfo& fun_info);

class Checker {
    int loop_count;
    bool add_this;
    std::map<std::string, std::unique_ptr<Value>> entity_values_no_generic;
    std::unordered_map<std::string, SymbolTable*> scopes;
    SymbolTable* scope;
    std::map<std::string, std::unique_ptr<Entity>> entities;
public:
    Module& module;
    bool is_call;
    ErrorReporter error_reporter;
    Entity* this_entity;
    Package& top_package;
    sem::SNode* update_loop_index_snode;

    Checker(Package& top_package, Module& module);
    ~Checker();

    std::unique_ptr<Entity> entity_from_type(const ast::Type& type);

    bool is_immutable(const ast::Type& node);
    void enter_scope(const std::string& name);
    void leave_scope();
    bool assert_type_exists(const ast::Type& type, TextPosition pos);
    Class* instantiate_generic(const Class& generic_p, const ast::ObjectType& instance);
    bool is_variable(const ast::ObjectType& a);
    std::pair<std::string, ast::Type*>*
    get_first_substitution_function(ast::FunctionType& a, ast::FunctionType& b, bool is_top_level_arg);
    std::pair<std::string, ast::Type*>*
    get_first_substitution_object(ast::ObjectType& a, ast::ObjectType& b, bool is_top_level_arg);
    std::pair<std::string, ast::Type*>* get_first_substitution(ast::Type& a, ast::Type& b, bool is_top_level_arg);
    ast::UTypeNode substitute(const ast::Type& t, const std::string& var, const ast::Type& replacement);
    std::unique_ptr<ast::FunctionType> unify_function_call(const ast::FunctionType& fun, ast::VectorOfTypes& args,
                                                           std::map<std::string, ast::Type*>& all_substitutions);
    std::unique_ptr<SemanticInfo>
    match_arguments_to_generic_function(const ast::FunctionType& ft, ast::VectorOfTypes arg_types,
                                        std::map<std::string, ast::Type*>& all_substitutions);
    void fail(std::string msg);

    USemanticInfo dispatch_rvalue(ast::Node& nod);

    USemanticInfo visit_assignment(ast::Assignment& n);
    USemanticInfo visit_binop(ast::BinaryOp& node);
    USemanticInfoBlock visit_block(ast::Block& node);
    USemanticInfoBlock visit_root(ast::Module& node);
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


    USemanticInfo object_member(sem::UExp object_snode, Value& p_value, const std::string& child, ast::Member& n);
    USemanticInfo class_member(Class* cls, const std::string& child, ast::Member& n);
    USemanticInfo package_member(Package& package, const std::string& child, ast::Member& n);
    USemanticInfo module_member(Module& mod, const std::string& child, ast::Member& n);


    USemanticInfo visit_match(ast::Match& node);
    USemanticInfo visit_alias(ast::Alias& p_node);
    USemanticInfo enum_member(Enum* enumm, const std::string& value, ast::Member& node);
    USemanticInfo visit_enum(ast::EnumNode& p_node);
    sem::UExp make_rvalue(const Entity& t_entity, sem::UExp value_snode, const sem::Type& target);
    USemanticInfo dispatch(ast::Node& nod);
    void fill_value(Value& value);
    std::unique_ptr<SemanticInfo> expect_rvalue_of_type(const sem::Type& target, ast::Node& node);
    void process_function_arguments(SemanticInfo& retv, std::vector<std::unique_ptr<Entity>>& arg_entities,
                                    std::vector<sem::UExp>& arguments, ast::Call& n,
                                    const sem::TypeFunction& function_type, SemanticInfo* fun_info_p);
    bool check_arguments(ast::Call& n, std::vector<sem::UExp>& arguments,
                         std::vector<std::unique_ptr<Entity>>& arg_entities);
    USemanticInfo
    make_return_info(const ast::Call& n, bool is_rvalue, USemanticInfo retv, bool is_def_const, bool args_are_constant);
    sem::UExp make_union_rvalue(sem::UExp value_snode, const sem::Type* unaliased_value_type,
                                const sem::Type* unaliased_target_type) const;
    sem::SNode* make_option_rvalue(sem::SNode* value_snode, const ast::Type* unaliased_value_type,
                                   const ast::Type* unaliased_target_type) const;
    // USemanticInfo visit_throw(ast::ThrowNode& n);

    USemanticInfo dispatch_any(ast::Node& n, bool is_rvalue);
    Value& entity_value_from_actual_base_path_no_generic(const Path& p);

    std::map<std::string, std::unique_ptr<Class>> classes;
    std::unique_ptr<Value> make_value(sem::Type* type);
    void init();
    sem::Exp* make_option_rvalue(sem::Exp* value_snode, const ast::Type* unaliased_value_type,
                                 const ast::Type* unaliased_target_type) const;
};

#endif //CHECKER_H


