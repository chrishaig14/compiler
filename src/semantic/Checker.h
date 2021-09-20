//
// Created by chris on 28/6/20.
//

#ifndef CHECKER_H
#define CHECKER_H

#include <iostream>
#include <set>
#include <set>
#include <unordered_map>

#include "../ast/top/EnumNode.h"
#include "../ast/top/Module.h"
#include "../ast/nodes.h"
#include "../ast/general/ObjectType.h"
#include "../ast/exp/include/UnaryOp.h"
#include "../logging/logging.h"
#include "../util/macros.h"
#include "../scanner/CodeLines.h"
#include "../simple_nodes/common/include/Assignment.h"
#include "../simple_nodes/common/include/Block.h"
#include "../simple_nodes/common/include/Block.h"
#include "../simple_nodes/common/include/Break.h"
#include "../simple_nodes/common/include/Call.h"
#include "../simple_nodes/common/include/Continue.h"
#include "../simple_nodes/common/include/Declaration.h"
#include "../simple_nodes/top/include/KlassDef.h"
#include "../simple_nodes/common/include/Match.h"
#include "../simple_nodes/expressions/include/NewObject.h"
#include "../simple_nodes/common/include/Return.h"
#include "../simple_nodes/common/include/TypeObject.h"
#include "../simple_nodes/common/src/TypeFunction.h"
#include "../simple_nodes/expressions/include/expressions.h"
#include "../simple_nodes/top/include/EnumDef.h"
#include "../simple_nodes/top/include/FunctionDef.h"
#include "../simple_nodes/common/include/If.h"
#include "../simple_nodes/common/include/While.h"
#include "../units/FunctionValue.h"
#include "../units/FunctionValue.h"
#include "../units/ObjectValue.h"
#include "../util/utils.h"
#include "CheckerUtils.h"
#include "errors/include/ErrorReporter.h"
#include "errors/include/errors.h"
#include "GlobalProcessor.h"
#include "util.h"

// #include "../logger/Logger.h"

#define T_NONE ast::ObjectType(".None")

typedef std::unique_ptr<SemanticInfo> USemanticInfo;
typedef std::unique_ptr<ExpressionInfo> UExpressionInfo;

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

sem::Common* make_for_snode(ast::For& node, std::unique_ptr<sem::Block>& binfo, UExpressionInfo& exp_info_p,
                            std::string loop_list_var_id, std::string loop_index_var_id,
                            std::string loop_list_len_var_id, sem::Common* update_loop_index_snode);
const sem::TypeFunction& get_function_type(const ExpressionInfo& fun_info);

USemanticInfo error_stub();
UExpressionInfo exp_error_stub();

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
    sem::Common* update_loop_index_snode;

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

    std::unique_ptr<sem::Block> visit_block(ast::Block& node);
    std::unique_ptr<sem::Module> visit_root(ast::Module& node);

    USemanticInfo visit_assignment(ast::Assignment& n);
    USemanticInfo visit_break(ast::Break& node);
    USemanticInfo visit_call(ast::Call& n, bool is_rvalue);
    USemanticInfo visit_continue(ast::Continue& node);
    USemanticInfo visit_declaration(ast::Declaration& n);
    USemanticInfo check_declaration_with_type(ast::Declaration& n);
    USemanticInfo check_declaration_without_type(ast::Declaration& n);
    USemanticInfo visit_for(ast::For& node);
    USemanticInfo visit_if(ast::If& n);
    USemanticInfo visit_import(ast::Import& node);
    USemanticInfo visit_return(ast::Return& n);
    USemanticInfo visit_while(ast::While& node);
    USemanticInfo visit_cast(ast::Cast& n);

    UExpressionInfo dispatch_rvalue(ast::ExpNode& n);
    UExpressionInfo visit_binop(ast::BinaryOp& node);
    UExpressionInfo visit_boolean(ast::Boolean& node);

    UExpressionInfo visit_call_exp(ast::Call& n) {
        auto s = this->visit_call(n, true);
        UExpressionInfo u = std::make_unique<ExpressionInfo>();
        u->exp_snode = std::move(s->exp_snode);
        return u;
    }

    UExpressionInfo visit_lvalue_subscript(ast::Subscript& node);
    UExpressionInfo visit_dict(ast::DictNode& node);
    UExpressionInfo visit_unary(ast::UnaryOp& n);
    UExpressionInfo visit_emptydict(ast::EmptyDict& node);
    UExpressionInfo visit_emptylist(ast::EmptyList& node);
    UExpressionInfo visit_id(ast::Id& n);
    UExpressionInfo visit_number(ast::Number& node);
    UExpressionInfo visit_partial(ast::PartialApplication& node);
    UExpressionInfo visit_string(ast::String& node);
    UExpressionInfo visit_subscript(ast::Subscript& node);
    UExpressionInfo visit_ternary(ast::Ternary& node);
    UExpressionInfo visit_tuple(ast::Tuple& node);
    UExpressionInfo visit_defconst(ast::DefaultConstructor& node);
    UExpressionInfo visit_list(ast::List& node);
    UExpressionInfo visit_member(ast::Member& n);
    UExpressionInfo visit_none(ast::None& node);
    UExpressionInfo object_member(sem::UExp object_snode, Value& p_value, const std::string& child, ast::Member& n);
    UExpressionInfo class_member(Class* cls, const std::string& child, ast::Member& n);
    UExpressionInfo package_member(Package& package, const std::string& child, ast::Member& n);
    UExpressionInfo module_member(Module& mod, const std::string& child, ast::Member& n);
    UExpressionInfo enum_member(Enum* enumm, const std::string& value, ast::Member& node);
    UExpressionInfo expect_rvalue_of_type(const sem::Type& target, ast::ExpNode& node);

    std::unique_ptr<sem::FunctionDef> visit_function(ast::Function& n);
    std::unique_ptr<sem::KlassDef> visit_class(ast::Klass& node);
    std::unique_ptr<sem::EnumDef> visit_enum(ast::EnumNode& p_node);

    USemanticInfo visit_match(ast::Match& node);
    USemanticInfo visit_alias(ast::Alias& p_node);
    sem::UExp make_rvalue(const Entity& t_entity, sem::UExp value_snode, const sem::Type& target);
    USemanticInfo dispatch(ast::Node& nod);
    void fill_value(Value& value);
    void process_function_arguments(SemanticInfo& retv, std::vector<std::unique_ptr<Entity>>& arg_entities,
                                    std::vector<sem::UExp>& arguments, ast::Call& n,
                                    const sem::TypeFunction& function_type, ExpressionInfo* fun_info_p);
    bool check_arguments(ast::Call& n, std::vector<sem::UExp>& arguments,
                         std::vector<std::unique_ptr<Entity>>& arg_entities);
    USemanticInfo
    make_return_info(const ast::Call& n, bool is_rvalue, USemanticInfo retv, bool is_def_const, bool args_are_constant);
    sem::UExp make_union_rvalue(sem::UExp value_snode, const sem::Type* unaliased_value_type,
                                const sem::Type* unaliased_target_type) const;
    sem::Common* make_option_rvalue(sem::Common* value_snode, const ast::Type* unaliased_value_type,
                                    const ast::Type* unaliased_target_type) const;
    // USemanticInfo visit_throw(ast::ThrowNode& n);

    USemanticInfo dispatch_statement(ast::Node& n, bool is_rvalue);
    Value& entity_value_from_actual_base_path_no_generic(const Path& p);

    std::map<std::string, std::unique_ptr<Class>> classes;
    std::unique_ptr<Value> make_value(sem::Type* type);
    void init();
    sem::Exp* make_option_rvalue(sem::Exp* value_snode, const ast::Type* unaliased_value_type,
                                 const ast::Type* unaliased_target_type) const;
    std::unique_ptr<sem::Top> dispatch_top(ast::TopNode& n);
};

#endif //CHECKER_H


