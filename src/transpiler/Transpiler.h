//
// Created by chris on 2/8/20.
//

#ifndef TRANSLATOR_H
#define TRANSLATOR_H


#include <set>
#include "../nodes/nodes.h"

std::string generate_function_class(int num_args);

bool is_object(const TypeNode& t);

class Transpiler {
public:
    std::string header;
    std::string static_declarations;
    std::string static_initializations;
    std::string externs_declaration;
    std::string globals_initialization;
    std::string method_class;
    std::vector<ObjectType*> tuple_types;
    Transpiler();
    std::string dispatch(Node* nptr);
    std::string visit_assignment(AssignmentNode& node);
    std::string visit_binop(BinopNode& node);
    std::string visit_block(BlockNode& node);
    std::string visit_boolean(BooleanNode& node);
    std::string visit_bool_op(BoolOpNode& node);
    std::string visit_break(BreakNode& node);
    std::string visit_call(CallNode& node);
    std::string visit_class(ClassNode& node);
    std::string visit_continue(ContinueNode& node);
    std::string visit_declaration(DeclarationNode& node);
    std::string visit_empty_list(EmptyListNode& node);
    std::string visit_for(ForNode& node);
    std::string visit_function(FunctionNode& node);
    std::string visit_id(IdNode& node);
    std::string visit_if(IfNode& node);
    std::string visit_list(ListNode& node);
    std::string visit_member(MemberNode& node);
    std::string visit_none(NoneNode& node);
    std::string visit_partial(PartialApplication& node);
    std::string visit_number(NumberNode& node);
    std::string visit_return(ReturnNode& node);
    std::string visit_string(StringNode& node);
    std::string visit_subscript(SubscriptNode& node);
    std::string visit_ternary(TernaryNode& node);
    std::string visit_tuple(TupleNode& node);
    std::string visit_type(TypeNode& node);
    std::string visit_while(WhileNode& node);

    void foo();
    std::string transpile(BlockNode* node);
    std::string generate_tuple_types();
    std::string generate_tuple(int n);
    std::string object_type_mapper(const ObjectType& t);
    std::string type_mapper(const TypeNode& t);
    std::string function_type_mapper(const FunctionType& t);
    bool is_lvalue;
    std::string wrap_in_function_type(const FunctionType& t, std::string n);
    std::string add_type(const TypeNode& t, std::string n);
    std::string ptr_to_type(const TypeNode& t);
    std::string ptr_to_type_object(const ObjectType& t);
    std::string visit_dict(DictNode& node);
    unsigned long num_members_class;
};


#endif //TRANSLATOR_H
