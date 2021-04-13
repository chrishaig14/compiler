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
#include "ErrorReporter.h"


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

class Checker {
    bool add_this;
    ErrorReporter error_reporter;
    bool is_lvalue;
    std::map<std::string, bool> inits;
    bool replace_me;
    Node* replacement;
    std::string current_class;
    std::string current_function;
    std::unordered_map<std::string, SymbolTable*> scopes;
    std::vector<ObjectType*> tuple_types;
    SymbolTable* scope;
    TypeNode* this_type;
public:
    bool is_method;
    std::string __file__;
    BlockSNode* root_snode;
    Module* module;
    bool is_call;
    bool failed;
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
    Node* replace_if_necessary(Node* node);
    void fail(std::string msg);


    USemanticInfo dispatch(Node* nod);

    USemanticInfo visit(AssignmentNode& n);
    USemanticInfo visit(BinopNode& node);
    USemanticInfo visit_block(BlockNode& node);
    USemanticInfo visit_root(BlockNode& node);
    USemanticInfo visit(BooleanNode& node);
    USemanticInfo visit(BoolOpNode& n);
    USemanticInfo visit(BreakNode& node);
    USemanticInfo visit(MethodNode& node);
    USemanticInfo visit_call(CallNode& n);
    USemanticInfo visit_class(ClassNode& node);
    USemanticInfo visit(ContinueNode& node);

    USemanticInfo visit(DeclarationNode& node);
    USemanticInfo check_declaration_with_type(DeclarationNode& n);
    USemanticInfo check_declaration_without_type(DeclarationNode& n);

    USemanticInfo visit(DictNode& node);
    USemanticInfo visit(EmptyDictNode& node);
    USemanticInfo visit(EmptyListNode& node);
    USemanticInfo visit(FloatNode& node);
    USemanticInfo visit(ForNode& node);
    USemanticInfo visit_function(FunctionNode& n);
    USemanticInfo visit_id(IdNode& n);
    USemanticInfo visit(IfNode& node);
    USemanticInfo visit(ListNode& node);

    USemanticInfo visit_member(MemberNode& n);
    USemanticInfo class_member(Class* cls, std::string child);
    USemanticInfo object_member(SNode* object_snode, ObjectValue* pValue, std::string child);
    USemanticInfo package_member(Package* package, std::string child);
    USemanticInfo module_member(Module* pModule, std::string basicString);

    USemanticInfo visit(NoneNode& node);
    USemanticInfo visit_import(ImportNode& node);
    USemanticInfo visit(NumberNode& node);
    USemanticInfo visit(PartialApplication& node);
    USemanticInfo visit_return(ReturnNode& n);
    USemanticInfo visit(StringNode& node);
    USemanticInfo visit(SubscriptNode& node);
    USemanticInfo visit(TernaryNode& node);
    USemanticInfo visit(TupleNode& node);
    USemanticInfo visit(WhileNode& node);
    USemanticInfo member_class_method(std::string class_name, std::string child, MemberNode& n);
    USemanticInfo member_tuple(const ObjectType& final_type, MemberNode& n);
    USemanticInfo visit(CastNode& n);
    USemanticInfo visit(DefaultConstructorNode& node);

};

bool function_is_generic(const FunctionType& ft);

#endif //CHECKER_H


