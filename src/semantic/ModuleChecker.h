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
#include "../ast/expressions/include/UnaryOp.h"
#include "../logging/logging.h"
#include "../util/macros.h"
#include "../scanner/CodeLines.h"
#include "../simple_nodes/common/include/Assignment.h"
#include "../simple_nodes/common/include/Block.h"
#include "../simple_nodes/common/include/For.h"
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
#include "../simple_nodes/common/include/TypeFunction.h"
#include "../simple_nodes/expressions/include/expressions.h"
#include "../simple_nodes/top/include/EnumDef.h"
#include "../simple_nodes/top/include/FunctionDef.h"
#include "../simple_nodes/common/include/If.h"
#include "../simple_nodes/common/include/While.h"
#include "../util/utils.h"
#include "CheckerUtils.h"
#include "errors/include/ErrorReporter.h"
#include "errors/include/errors.h"
#include "ModulePrechecker.h"
#include "util.h"
#include "ExpressionInfo.h"
#include "../units/entities/EntityConstFunction.h"
#include "../units/entities/EntityModule.h"
#include "../units/entities/EntityPackage.h"
#include "../units/entities/EntityClass.h"
#include "../units/entities/EntityEnum.h"
#include "../units/entities/EntityValue.h"
#include "../units/entities/EntityError.h"
#include "../units/entities/EntityNotFound.h"
#include "../units/entities/EntityTypeclass.h"
#include "../units/entities/EntityNothing.h"

// #include "../logger/Logger.h"

#define T_NONE ast::ObjectType(".None")


bool is_generic(const sem::Type& t);
ast::UTypeNode make_type_from_object_pattern(const ast::ObjectType& object_type, const MapStringType& replacements);
ast::UTypeNode make_type_from_function_pattern(const ast::FunctionType& ftn, const MapStringType& replacements);
ast::UTypeNode make_type(const ast::Type& original, const MapStringType& replacements);
sem::UCommon match_arguments_to_generic_function(const ast::FunctionType& function_type, ast::VectorOfTypes arg_types);
std::unique_ptr<Entity> map_module_member_to_entity(const ModuleMember& module_member);
ModuleMember* map_unit_to_module_member(Unit& u);
TextPosition add_one_col(TextPosition t);
bool function_is_generic(const sem::TypeFunction& ft);

sem::Common* make_for_snode(ast::For& node, std::unique_ptr<sem::Block>& binfo, UExpressionInfo& exp_info_p,
                            std::string loop_list_var_id, std::string loop_index_var_id,
                            std::string loop_list_len_var_id, sem::Common* update_loop_index_snode);
const sem::TypeFunction& get_function_type(const ExpressionInfo& fun_info);

UExpressionInfo exp_error_stub();

class ModuleChecker {
    bool add_this;
    std::map<std::string, std::unique_ptr<EntityValue>> entity_values_no_generic;
    std::unordered_map<std::string, SymbolTable*> scopes;
    SymbolTable* scope;
    std::map<std::string, std::unique_ptr<Entity>> entities;
public:
    Module& module;
    bool is_call;
    ErrorReporter error_reporter;
    std::unique_ptr<Entity> this_entity;
    Package& top_package;

    ModuleChecker(Package& top_package, Module& module);
    ~ModuleChecker();

    std::unique_ptr<Entity> entity_from_type(const ast::Type& type);

    bool is_immutable(const ast::Type& node);
    void enter_scope(const std::string& name);
    void leave_scope();
    bool assert_type_exists(const ast::Type& type, TextPosition pos);
    ConcreteClass* instantiate_generic(const ConcreteClass& generic_p, const ast::ObjectType& instance);
    bool is_variable(const ast::ObjectType& a);
    void fail(std::string msg);
    void init();

    std::unique_ptr<sem::Module> check_module();

    std::unique_ptr<sem::FunctionDef> visit_function(ast::Function& n);
    std::unique_ptr<sem::KlassDef> visit_class(ast::Klass& node);
    std::unique_ptr<sem::EnumDef> visit_enum(ast::EnumNode& p_node);
    std::unique_ptr<sem::Top> visit_typeclass(const ast::TypeclassAst& typeclass);

    std::unique_ptr<sem::Block> visit_block(const ast::Block& node);

    // statements
    sem::UCommon dispatch_statement(const ast::Statement& n, bool is_rvalue);
    sem::UCommon dispatch(const ast::Statement& nod);
    sem::UCommon visit_match(const ast::Match& node);
    sem::UCommon visit_alias(const ast::Alias& p_node);
    sem::UCommon visit_assignment(const ast::Assignment& n);
    sem::UCommon visit_break(const ast::Break& node);
    sem::UCommon visit_call(const ast::Call& n, bool is_rvalue);
    sem::UCommon visit_continue(const ast::Continue& node);
    sem::UCommon visit_declaration(const ast::Declaration& n);
    sem::UCommon check_declaration_with_type(const ast::Declaration& n);
    sem::UCommon check_declaration_without_type(const ast::Declaration& n);
    sem::UCommon visit_for(const ast::For& node);
    sem::UCommon visit_if(const ast::If& n);
    sem::UCommon visit_import(const ast::Import& node);
    sem::UCommon visit_return(const ast::Return& n);
    sem::UCommon visit_while(const ast::While& node);
    sem::UCommon visit_cast(const ast::Cast& n);

    // expressions
    UExpressionInfo expect_rvalue_of_type(const sem::Type& target, ast::ExpNode& node);
    UExpressionInfo dispatch_rvalue(const ast::ExpNode& n);
    UExpressionInfo visit_binop(const ast::BinaryOp& node);
    UExpressionInfo visit_boolean(const ast::Boolean& node);
    UExpressionInfo visit_call_exp(const ast::CallExp& n);
    sem::UCommon visit_lvalue_subscript(const ast::Subscript& node);
    UExpressionInfo visit_dict(const ast::DictNode& node);
    UExpressionInfo visit_unary(const ast::UnaryOp& n);
    UExpressionInfo visit_emptydict(const ast::EmptyDict& node);
    UExpressionInfo visit_emptylist(const ast::EmptyList& node);
    UExpressionInfo visit_id(const ast::Id& n);
    UExpressionInfo visit_number(const ast::Number& node);
    UExpressionInfo visit_partial(const ast::PartialApplication& node);
    UExpressionInfo visit_string(const ast::String& node);
    UExpressionInfo visit_subscript(const ast::Subscript& node);
    UExpressionInfo visit_ternary(const ast::Ternary& node);
    UExpressionInfo visit_tuple(const ast::Tuple& node);
    UExpressionInfo visit_defconst(const ast::DefaultConstructor& node);
    UExpressionInfo visit_list(const ast::List& node);
    UExpressionInfo visit_member(const ast::Member& n);
    UExpressionInfo visit_none(const ast::None& node);
    UExpressionInfo visit_callexp(ast::CallExp& n, bool is_rvalue);
    UExpressionInfo
    object_member(sem::UExp object_snode, EntityValue& p_value, const std::string& child, const ast::Member& n);
    UExpressionInfo package_member(const ast::Member& n, const Package& package);
    UExpressionInfo module_member(const ast::Member& n, const Module& mod);
    UExpressionInfo enum_member(const ast::Member& node, const Enum& enumm);
    UExpressionInfo const_function_member(const ast::Member& n, UExpressionInfo unique_ptr_1, ConstFunction& function);
    UExpressionInfo class_member(const ast::Member& n, UExpressionInfo parent_info, ConcreteClass& cls);
    UExpressionInfo value_member(const ast::Member& n, UExpressionInfo parent_info, EntityValue& value);
    sem::UExp make_union_rvalue(sem::UExp value_snode, const sem::Type* unaliased_value_type,
                                const sem::Type* unaliased_target_type) const;
    sem::UExp make_rvalue(const Entity& t_entity, sem::UExp value_snode, const sem::Type& target);
    sem::Exp* make_option_rvalue(sem::Exp* value_snode, const ast::Type* unaliased_value_type,
                                 const ast::Type* unaliased_target_type) const;
    sem::Common* make_option_rvalue(sem::Common* value_snode, const ast::Type* unaliased_value_type,
                                    const ast::Type* unaliased_target_type) const;

    // function call
    UExpressionInfo make_return_info(bool is_rvalue, UExpressionInfo retv_p, bool is_def_const, bool args_are_constant);
    std::pair<std::string, ast::Type*>*
    get_first_substitution_function(ast::FunctionType& a, ast::FunctionType& b, bool is_top_level_arg);
    std::pair<std::string, ast::Type*>*
    get_first_substitution_object(ast::ObjectType& a, ast::ObjectType& b, bool is_top_level_arg);
    std::pair<std::string, ast::Type*>* get_first_substitution(ast::Type& a, ast::Type& b, bool is_top_level_arg);
    ast::UTypeNode substitute(const ast::Type& t, const std::string& var, const ast::Type& replacement);
    std::unique_ptr<ast::FunctionType> unify_function_call(const ast::FunctionType& fun, ast::VectorOfTypes& args,
                                                           std::map<std::string, ast::Type*>& all_substitutions);
    UExpressionInfo match_arguments_to_generic_function(const ast::FunctionType& ft, ast::VectorOfTypes arg_types,
                                                        std::map<std::string, ast::Type*>& all_substitutions);
    UExpressionInfo analyze_call(const ast::ExpNode& function, std::vector<ast::RExpNode>& arguments, bool is_rvalue,
                                 TextPosition start, TextPosition end);
    bool check_arguments(std::vector<ast::RExpNode>& narguments, std::vector<sem::UExp>& arguments,
                         std::vector<std::unique_ptr<Entity>>& arg_entities);
    void
    process_function_arguments(std::vector<std::unique_ptr<Entity>>& arg_entities, std::vector<sem::UExp>& arguments,
                               std::vector<ast::RExpNode>& narguments, const sem::TypeFunction& function_type,
                               ExpressionInfo* fun_info_p);

    // entities
    EntityValue& entity_value_from_actual_base_path_no_generic(const Path& p);
    std::unique_ptr<EntityValue> make_value(sem::Type* type);

    std::unique_ptr<sem::Top> dispatch_top(const ast::TopNode& n);
    sem::UCommon visit_call(ast::Call& n);

    std::unique_ptr<EntityValue> make_entity_value(sem::Type& type);
    void add_typeclasses_to_generic_type(sem::Type& type, std::string gen_type, std::string typeclass_name);
};

#endif //CHECKER_H


