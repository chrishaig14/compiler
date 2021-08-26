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
#include "../simple_nodes/BlockSNode.h"
#include "../units/ObjectValue.h"
#include "../units/FunctionValue.h"
#include "../ast/EnumNode.h"
#include "../simple_nodes/EnumSNode.h"
#include "errors/ErrorReporter.h"
#include "util.h"
// #include "../logger/Logger.h"
#include "../simple_nodes/BlockSNode.h"
#include "../simple_nodes/AssignmentSNode.h"
#include "../simple_nodes/ReturnSNode.h"
#include "../simple_nodes/ContinueSNode.h"
#include "../simple_nodes/IntegerSNode.h"
#include "../simple_nodes/FunctionSNode.h"
#include "../simple_nodes/DeclarationSNode.h"
#include "../simple_nodes/IdSNode.h"
#include "../simple_nodes/CallSNode.h"
#include "../simple_nodes/StringSNode.h"
#include "../units/FunctionValue.h"
#include "../simple_nodes/BoolSNode.h"
#include "../simple_nodes/FloatSNode.h"
#include "../simple_nodes/ClassSNode.h"
#include "../simple_nodes/NewObjectSNode.h"
#include "../simple_nodes/WhileSNode.h"
#include "../simple_nodes/BreakSNode.h"
#include "../simple_nodes/ListSNode.h"
#include "../simple_nodes/IfSNode.h"
#include "../simple_nodes/ObjectMemberSNode.h"
#include "../simple_nodes/MatchSNode.h"
#include "CheckerUtils.h"
#include "../ast/UnaryOpNode.h"
#include "../ast/TryCatchNode.h"
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

SNode* make_for_snode(ForNode& node, USemanticInfo& binfo, USemanticInfo& exp_info_p, std::string loop_list_var_id,
                      std::string loop_index_var_id, std::string loop_list_len_var_id, SNode* update_loop_index_snode);

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
    SNode* update_loop_index_snode;

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

    USemanticInfo dispatch_rvalue(Node& nod);

    USemanticInfo visit_assignment(AssignmentNode& n);
    USemanticInfo visit_binop(ast::BinopNode& node);
    USemanticInfo visit_block(BlockNode& node);
    USemanticInfo visit_root(BlockNode& node);
    USemanticInfo visit_boolean(BooleanNode& node);
    USemanticInfo visit_boolop(BoolOpNode& n);
    USemanticInfo visit_break(BreakNode& node);
    USemanticInfo visit_call(ast::CallNode& n, bool is_rvalue);
    USemanticInfo visit_class(ClassNode& node);
    USemanticInfo visit_continue(ContinueNode& node);

    USemanticInfo visit_lvalue_subscript(SubscriptNode& node);

    USemanticInfo visit_declaration(ast::DeclarationNode& n);
    USemanticInfo check_declaration_with_type(ast::DeclarationNode& n);
    USemanticInfo check_declaration_without_type(ast::DeclarationNode& n);

    USemanticInfo visit_dict(DictNode& node);
    USemanticInfo visit_emptydict(EmptyDictNode& node);
    USemanticInfo visit_emptylist(EmptyListNode& node);
    USemanticInfo visit_unary(UnaryOpNode& n);
    USemanticInfo visit_for(ForNode& node);
    USemanticInfo visit_function(FunctionNode& n);
    USemanticInfo visit_id(ast::IdNode& n);
    USemanticInfo visit_if(IfNode& n);
    USemanticInfo visit_list(ListNode& node);
    USemanticInfo visit_member(MemberNode& n);
    USemanticInfo visit_none(NoneNode& node);
    USemanticInfo visit_import(ImportNode& node);
    USemanticInfo visit_number(NumberNode& node);
    USemanticInfo visit_partial(PartialApplication& node);
    USemanticInfo visit_return(ReturnNode& n);
    USemanticInfo visit_string(StringNode& node);
    USemanticInfo visit_subscript(SubscriptNode& node);
    USemanticInfo visit_ternary(TernaryNode& node);
    USemanticInfo visit_tuple(TupleNode& node);
    USemanticInfo visit_while(WhileNode& node);
    USemanticInfo visit_cast(CastNode& n);
    USemanticInfo visit_defconst(DefaultConstructorNode& node);


    USemanticInfo object_member(SNode* object_snode, Value& p_value, const std::string& child, MemberNode& n);
    USemanticInfo class_member(Class* cls, const std::string& child, MemberNode& n);
    USemanticInfo package_member(Package& package, const std::string& child, MemberNode& n);
    USemanticInfo module_member(Module& mod, const std::string& child, MemberNode& n);


    USemanticInfo visit_match(MatchExpressionNode& node);
    USemanticInfo visit_alias(AliasNode& p_node);
    USemanticInfo enum_member(Enum* enumm, const std::string& value, MemberNode& node);
    USemanticInfo visit_enum(EnumNode& p_node);
    SNode* make_rvalue(const Entity& value_entity, SNode* value_snode, const TypeNode& target);
    USemanticInfo dispatch(Node& nod);
    void fill_value(Value& value);
    std::unique_ptr<SemanticInfo> expect_rvalue_of_type(const TypeNode& target, Node& node);
    void process_function_arguments(SemanticInfo& retv, std::vector<Entity*>& arg_entities, CallSNode* sn, ast::CallNode& n,
                                    FunctionType* function_type, SemanticInfo* fun_info_p);
    bool check_arguments(ast::CallNode& n, CallSNode* sn, VectorOfTypes& arg_types, std::vector<Entity*>& arg_entities);
    USemanticInfo
    make_return_info(const ast::CallNode& n, bool is_rvalue, SemanticInfo& retv, bool is_def_const, bool args_are_constant);
    SNode* make_union_rvalue(SNode* value_snode, const TypeNode* unaliased_value_type,
                             const TypeNode* unaliased_target_type) const;
    SNode* make_option_rvalue(SNode* value_snode, const TypeNode* unaliased_value_type,
                              const TypeNode* unaliased_target_type) const;
    // USemanticInfo visit_throw(ThrowNode& n);
    USemanticInfo visit_try_catch(TryCatchNode& node);
    void init();
    USemanticInfo dispatch_any(Node& n, bool is_rvalue);
    EntityValue& entity_value_from_actual_base_path_no_generic(const Path& p);
};

#endif //CHECKER_H


