//
// Created by chris on 28/6/20.
//

#ifndef CHECKER_H
#define CHECKER_H

#include <unordered_map>
#include "GlobalProcessor.h"
#include "../utils.h"
#include "../nodes/nodes.h"
#include <set>
#include "../scanner/CodeLines.h"
#include <iostream>
#include <set>
#include "../macros.h"
#include "../logging/logging.h"
#include "../simple_nodes/BlockSNode.h"
#include "../units/ObjectValue.h"
#include "../units/FunctionValue.h"
#include "../nodes/EnumNode.h"
#include "../simple_nodes/EnumSNode.h"
#include "ErrorReporter.h"
#include "util.h"
#include "../logger/Logger.h"
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
#include "../nodes/UnaryOpNode.h"

#define T_NONE ObjectType(".None")

typedef std::unique_ptr<SemanticInfo> USemanticInfo;

bool type_matches(TypeNode* a, TypeNode* b);

bool is_generic(const TypeNode& t);

MapStringType make_replacements(TypeNode* a, TypeNode* b);
TypeNode* make_type_from_object_pattern(const ObjectType& object_type, const MapStringType& replacements);
TypeNode* make_type_from_function_pattern(const FunctionType& ftn, const MapStringType& replacements);
TypeNode* make_type(const TypeNode& original, const MapStringType& replacements);
SemanticInfo match_arguments_to_generic_function(const FunctionType& function_type, VectorOfTypes arg_types);
USemanticInfo error_stub();

Class* make_list_class_info();

Class* make_file_class_info();

Class* make_int_class_info();

Class* make_boolean_class_info();

Class* make_float_class_info();

Class* make_double_class_info();
Entity entity_from_type(const TypeNode& type);

Class* make_string_class_info();
Entity map_flirpin_to_entity(Flirpin flirpin);

Flirpin map_unit_to_flirpin(Unit u);
int target_union_type(const ObjectType& target, const TypeNode& source);
SNode* make_union_wrapper(int type_index, SNode* expression);

std::string binoptype_to_str(OpType op);

class Checker {
    int loop_count;
    bool add_this;
    std::unordered_map<std::string, SymbolTable*> scopes;
    SymbolTable* scope;
public:
    std::string __file__;
    BlockSNode* root_snode;
    Module* module;
    bool is_call;
    CodeLines code_lines;

    Checker();
    ~Checker();

    bool can_assign(const TypeNode& from, const TypeNode& to);
    bool can_assign_generic(TypeNode& from, TypeNode& to, VectorOfStrings type_params);
    bool is_immutable(const TypeNode& node);

    void enter_scope(std::string name);
    void leave_scope();

    bool assert_type_exists(TypeNode& type, TextPosition pos);

    Class* instantiate_generic(Class* generic, const ObjectType& instance);

    bool is_variable(const ObjectType& a);
    VectorOfTypes get_replacements_in_order(const FunctionType& function_type, VectorOfTypes arg_types);
    std::pair<std::string, TypeNode*>*
    get_first_substitution_function(FunctionType& a, FunctionType& b, bool is_top_level_arg);
    std::pair<std::string, TypeNode*>*
    get_first_substitution_object(ObjectType& a, ObjectType& b, bool is_top_level_arg);
    std::pair<std::string, TypeNode*>* get_first_substitution(TypeNode& a, TypeNode& b, bool is_top_level_arg);
    TypeNode* substitute(TypeNode* t, std::string var, TypeNode* replacement);
    void unify_function_call(FunctionType& fun, VectorOfTypes& args);
    SemanticInfo match_arguments_to_generic_function(const FunctionType& ft, VectorOfTypes arg_types);
    void fail(std::string msg);


    USemanticInfo dispatch_rvalue(Node* nod);

    USemanticInfo visit_assignment(AssignmentNode& n);
    USemanticInfo visit_binop(BinopNode& node);
    USemanticInfo visit_block(BlockNode& node);
    USemanticInfo visit_root(BlockNode& node);
    USemanticInfo visit_boolean(BooleanNode& node);
    USemanticInfo visit_boolop(BoolOpNode& n);
    USemanticInfo visit_break(BreakNode& node);
    USemanticInfo visit_call(CallNode& n, bool is_rvalue);
    USemanticInfo visit_class(ClassNode& node);
    USemanticInfo visit_continue(ContinueNode& node);

    USemanticInfo visit_lvalue_subscript(SubscriptNode& node);

    USemanticInfo visit_declaration(DeclarationNode& n);
    USemanticInfo check_declaration_with_type(DeclarationNode& n);
    USemanticInfo check_declaration_without_type(DeclarationNode& n);

    USemanticInfo visit_dict(DictNode& node);
    USemanticInfo visit_emptydict(EmptyDictNode& node);
    USemanticInfo visit_emptylist(EmptyListNode& node);
    USemanticInfo visit_float(FloatNode& node);
    USemanticInfo visit_unary(UnaryOpNode& n);
    USemanticInfo visit_for(ForNode& node);
    USemanticInfo visit_function(FunctionNode& n);
    USemanticInfo visit_id(IdNode& n);
    USemanticInfo visit_if(IfNode& n);
    USemanticInfo visit_list(ListNode& node);

    USemanticInfo visit_member(MemberNode& n);
    USemanticInfo class_member(Class* cls, std::string child);

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
    USemanticInfo member_class_method(std::string class_name, std::string child, MemberNode& n);
    USemanticInfo member_tuple(const ObjectType& final_type, MemberNode& n);
    USemanticInfo visit_cast(CastNode& n);
    USemanticInfo visit_defconst(DefaultConstructorNode& node);

    ErrorReporter error_reporter;
    Entity this_entity;
    USemanticInfo object_member(SNode* object_snode, Value* pValue, std::string child, MemberNode& n);
    USemanticInfo class_member(Class* cls, std::string child, MemberNode& n);
    USemanticInfo package_member(Package* package, std::string child, MemberNode& n);
    USemanticInfo module_member(Module* mod, std::string child, MemberNode& n);
    Package* root_package;
    SNode* update_loop_index_snode;
    SNode* make_for_snode(ForNode& node, USemanticInfo& binfo, USemanticInfo& exp_info_p);
    std::string loop_list_var_id;
    std::string loop_index_var_id;
    std::string loop_list_len_var_id;
    USemanticInfo visit_match(MatchExpressionNode* node);
    USemanticInfo visit_alias(AliasNode* pNode);
    USemanticInfo enum_member(Enum* enumm, std::string value, MemberNode& node);
    USemanticInfo visit_enum(EnumNode& pNode);
    bool can_be_assigned_to(const TypeNode& value, const TypeNode& target);
    SNode* make_rvalue(Entity value_entity, const TypeNode& target);
    SNode* make_rvalue(Entity value_entity, SNode* value_snode, const TypeNode& target);
    USemanticInfo dispatch(Node* nod);
    USemanticInfo dispatch_any(Node* pNode, bool b);
    void fill_value(Value* value);
};

int target_union_type(const ObjectType& target, const TypeNode& source);

bool function_is_generic(const FunctionType& ft);

#endif //CHECKER_H


