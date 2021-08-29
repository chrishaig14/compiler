//
// Created by chris on 4/4/21.
//

#ifndef XLANG_STRANSPILER_H
#define XLANG_STRANSPILER_H

#include "../simple_nodes/Block.h"
#include "../simple_nodes/FunctionSNode.h"
#include "../simple_nodes/Assignment.h"
#include "../simple_nodes/DeclarationSNode.h"
#include "../simple_nodes/IntegerSNode.h"
#include "../simple_nodes/IdSNode.h"
#include "../simple_nodes/Return.h"
#include "../simple_nodes/EnumSNode.h"
#include "../simple_nodes/Call.h"
#include "../simple_nodes/String.h"
#include "../simple_nodes/Bool.h"
#include "../simple_nodes/FloatSNode.h"
#include "../simple_nodes/Klass.h"
#include "../simple_nodes/NewObjectSNode.h"
#include "../simple_nodes/Match.h"
#include "../simple_nodes/ObjectMember.h"
#include "../simple_nodes/While.h"
#include "../simple_nodes/List.h"
#include "../simple_nodes/IfSNode.h"
#include "../simple_nodes/Break.h"
#include "../simple_nodes/ContinueSNode.h"
#include "../simple_nodes/EnumMemberSNode.h"
#include "../simple_nodes/Ternary.h"
#include "../simple_nodes/None.h"
#include "../simple_nodes/DictSNode.h"
#include "../simple_nodes/Throw.h"
#include "../simple_nodes/TryCatch.h"

const std::string TOBJECT = "TaggedObject*";
const std::string GCDECLARE = "GC::declare";
const std::string GCASSIGN = "GC::assign";
const std::string GCRETURN = "GC::set_return";
const std::string GCOUTOFSCOPE = "GC::out_of_scope";
const std::string LPAREN = "(";
const std::string RPAREN = ")";
const std::string SEMIC = ";";
const std::string LCURLY = "{";
const std::string RCURLY = "}";
const std::string ASSIGN = "=";
const std::string QUOTE = "\"";
const std::string SPACE = " ";
const std::string COMMA = ",";
const std::string NEWLINE = "\n";
const std::string EXTERN = "extern";
const std::string RETURN = "return";
const std::string CLASS = "class";
const std::string RETURN_VAR = "__return__";


#define GASSIGN(exp, new_value) "GC::assign(" exp, new_value ")"

class OutputCode {
public:
    OutputCode(const std::string& pre_code, const std::string& code);
public:
    std::string pre_code;
    std::string code;

};

class STranspiler {

public:
    std::string source;
    std::string header;
    std::string static_initializations;
    std::string static_cleanups;

    OutputCode transpile_integer(sem::IntegerSNode& node);

    void transpile_function(sem::FunctionSNode& node);

    OutputCode transpile_declaration(sem::DeclarationSNode& node);

    OutputCode transpile_assignment(sem::Assignment& node);
    OutputCode transpile_block(sem::Block& node);
    void transpile_program(sem::Block& node);
    OutputCode transpile_id(sem::IdSNode& node);

    OutputCode transpile_new(sem::NewObjectSNode& node);

    OutputCode transpile_return(sem::Return& node);

    void transpile_class(sem::Klass& node);
    void transpile_enum(sem::EnumSNode& node);

    void dispatch_top(sem::SNode& node) {
        switch (node.type) {
            case SNodeType::FUNCTION:
                this->transpile_function((sem::FunctionSNode&) (node));
                break;
            case SNodeType::ENUM:
                this->transpile_enum((sem::EnumSNode&) node);
                break;
            case SNodeType::CLASS:
                this->transpile_class((sem::Klass&) node);
                break;
            default:
                throw std::runtime_error("Don't know what to do with this SNode!");
        }
    }

    OutputCode transpile_call(sem::Call& node);

    OutputCode transpile_string(sem::String& node);

    OutputCode transpile_boolean(sem::Bool& pNode);

    OutputCode transpile_float(sem::FloatSNode& node);

    OutputCode transpile_object_member(sem::ObjectMember& node);

    OutputCode transpile_while(sem::While& node);

    OutputCode transpile_list(sem::List& node);

    OutputCode transpile_if(sem::IfSNode& node);

    OutputCode transpile_break(sem::Break& node);

    OutputCode transpile_continue(sem::ContinueSNode& node);

    OutputCode transpile_match(sem::Match& node);

    OutputCode transpile_enum_member(sem::EnumMemberSNode& node);

    OutputCode transpile_ternary(sem::Ternary& node);

    OutputCode transpile_none(sem::None& node);

    OutputCode transpile_dict(sem::DictSNode& node);
    OutputCode transpile_try_catch(sem::TryCatch& node);

    OutputCode dispatch(sem::SNode& node) {
        switch (node.type) {
            case SNodeType::BLOCK:
                return this->transpile_block((sem::Block&) node);
            case SNodeType::ENUM_MEMBER:
                return this->transpile_enum_member((sem::EnumMemberSNode&) node);
            case SNodeType::BOOLEAN:
                return this->transpile_boolean((sem::Bool&) node);
            case SNodeType::MATCH:
                return this->transpile_match((sem::Match&) node);
            case SNodeType::IF:
                return this->transpile_if((sem::IfSNode&) node);
            case SNodeType::BREAK:
                return this->transpile_break((sem::Break&) node);
            case SNodeType::CONTINUE:
                return this->transpile_continue((sem::ContinueSNode&) node);
            case SNodeType::ID:
                return this->transpile_id((sem::IdSNode&) node);
            case SNodeType::STRING:
                return this->transpile_string((sem::String&) node);
            case SNodeType::CALL:
                return this->transpile_call((sem::Call&) node);
            case SNodeType::NEW:
                return this->transpile_new((sem::NewObjectSNode&) node);
            case SNodeType::DICT:
                return this->transpile_dict((sem::DictSNode&) node);
            case SNodeType::LIST:
                return this->transpile_list((sem::List&) node);
            case SNodeType::OBJECT_MEMBER:
                return this->transpile_object_member((sem::ObjectMember&) node);
            case SNodeType::DECLARATION:
                return this->transpile_declaration((sem::DeclarationSNode&) node);
            case SNodeType::WHILE:
                return this->transpile_while((sem::While&) node);
            case SNodeType::ASSIGNMENT:
                return this->transpile_assignment((sem::Assignment&) node);
            case SNodeType::RETURN:
                return this->transpile_return((sem::Return&) node);
            case SNodeType::TRY_CATCH:
                return this->transpile_try_catch((sem::TryCatch&) node);
            case SNodeType::THROW:
                return this->transpile_throw((sem::Throw&) node);
                break;
            case SNodeType::FLOAT:
                return this->transpile_float((sem::FloatSNode&) node);
            case SNodeType::NONE:
                return this->transpile_none((sem::None&) node);
            case SNodeType::TERNARY:
                return this->transpile_ternary((sem::Ternary&) node);
            case SNodeType::INTEGER:
                return this->transpile_integer((sem::IntegerSNode&) node);
                break;
            default:
                throw std::runtime_error("Don't know what to do with this SNode!");
        }
    }

    OutputCode transpile_throw(sem::Throw& node);
    bool in_try_catch;
};


#endif //XLANG_STRANSPILER_H
