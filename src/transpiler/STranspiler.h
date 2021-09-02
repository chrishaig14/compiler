//
// Created by chris on 4/4/21.
//

#ifndef XLANG_STRANSPILER_H
#define XLANG_STRANSPILER_H

#include "../simple_nodes/Block.h"
#include "../simple_nodes/FunctionDef.h"
#include "../simple_nodes/with_unique/Assignment.h"
#include "../simple_nodes/Declaration.h"
#include "../simple_nodes/with_unique/Integer.h"
#include "../simple_nodes/with_unique/Id.h"
#include "../simple_nodes/with_unique/Return.h"
#include "../simple_nodes/with_unique/EnumDef.h"
#include "../simple_nodes/with_unique/Call.h"
#include "../simple_nodes/with_unique/String.h"
#include "../simple_nodes/with_unique/Bool.h"
#include "../simple_nodes/with_unique/Float.h"
#include "../simple_nodes/KlassDef.h"
#include "../simple_nodes/NewObject.h"
#include "../simple_nodes/Match.h"
#include "../simple_nodes/with_unique/ObjectMember.h"
#include "../simple_nodes/While.h"
#include "../simple_nodes/with_unique/List.h"
#include "../simple_nodes/IfSNode.h"
#include "../simple_nodes/with_unique/Break.h"
#include "../simple_nodes/with_unique/Continue.h"
#include "../simple_nodes/with_unique/EnumMember.h"
#include "../simple_nodes/Ternary.h"
#include "../simple_nodes/with_unique/None.h"
#include "../simple_nodes/with_unique/Dict.h"
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

class CppOutputCode {
public:
    CppOutputCode(const std::string& pre_code, const std::string& code);
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

    CppOutputCode transpile_integer(sem::Integer& node);

    void transpile_function(sem::FunctionDef& node);

    CppOutputCode transpile_declaration(sem::Declaration& node);

    CppOutputCode transpile_assignment(sem::Assignment& node);
    CppOutputCode transpile_block(sem::Block& node);
    void transpile_program(sem::Block& node);
    CppOutputCode transpile_id(sem::Id& node);

    CppOutputCode transpile_new(sem::NewObject& node);

    CppOutputCode transpile_return(sem::Return& node);

    void transpile_class(sem::KlassDef& node);
    void transpile_enum(sem::EnumDef& node);

    void dispatch_top(sem::SNode& node) {
        switch (node.type) {
            case SNodeType::FUNCTION:
                this->transpile_function((sem::FunctionDef&) (node));
                break;
            case SNodeType::ENUM:
                this->transpile_enum((sem::EnumDef&) node);
                break;
            case SNodeType::CLASS:
                this->transpile_class((sem::KlassDef&) node);
                break;
            default:
                throw std::runtime_error("Don't know what to do with this SNode!");
        }
    }

    CppOutputCode transpile_call(sem::Call& node);

    CppOutputCode transpile_string(sem::String& node);

    CppOutputCode transpile_boolean(sem::Bool& pNode);

    CppOutputCode transpile_float(sem::Float& node);

    CppOutputCode transpile_object_member(sem::ObjectMember& node);

    CppOutputCode transpile_while(sem::While& node);

    CppOutputCode transpile_list(sem::List& node);

    CppOutputCode transpile_if(sem::IfSNode& node);

    CppOutputCode transpile_break(sem::Break& node);

    CppOutputCode transpile_continue(sem::Continue& node);

    CppOutputCode transpile_match(sem::Match& node);

    CppOutputCode transpile_enum_member(sem::EnumMember& node);

    CppOutputCode transpile_ternary(sem::Ternary& node);

    CppOutputCode transpile_none(sem::None& node);

    CppOutputCode transpile_dict(sem::Dict& node);
    CppOutputCode transpile_try_catch(sem::TryCatch& node);

    CppOutputCode dispatch(sem::SNode& node) {
        switch (node.type) {
            case SNodeType::BLOCK:
                return this->transpile_block((sem::Block&) node);
            case SNodeType::ENUM_MEMBER:
                return this->transpile_enum_member((sem::EnumMember&) node);
            case SNodeType::BOOLEAN:
                return this->transpile_boolean((sem::Bool&) node);
            case SNodeType::MATCH:
                return this->transpile_match((sem::Match&) node);
            case SNodeType::IF:
                return this->transpile_if((sem::IfSNode&) node);
            case SNodeType::BREAK:
                return this->transpile_break((sem::Break&) node);
            case SNodeType::CONTINUE:
                return this->transpile_continue((sem::Continue&) node);
            case SNodeType::ID:
                return this->transpile_id((sem::Id&) node);
            case SNodeType::STRING:
                return this->transpile_string((sem::String&) node);
            case SNodeType::CALL:
                return this->transpile_call((sem::Call&) node);
            case SNodeType::NEW:
                return this->transpile_new((sem::NewObject&) node);
            case SNodeType::DICT:
                return this->transpile_dict((sem::Dict&) node);
            case SNodeType::LIST:
                return this->transpile_list((sem::List&) node);
            case SNodeType::OBJECT_MEMBER:
                return this->transpile_object_member((sem::ObjectMember&) node);
            case SNodeType::DECLARATION:
                return this->transpile_declaration((sem::Declaration&) node);
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
                return this->transpile_float((sem::Float&) node);
            case SNodeType::NONE:
                return this->transpile_none((sem::None&) node);
            case SNodeType::TERNARY:
                return this->transpile_ternary((sem::Ternary&) node);
            case SNodeType::INTEGER:
                return this->transpile_integer((sem::Integer&) node);
                break;
            default:
                throw std::runtime_error("Don't know what to do with this SNode!");
        }
    }

    CppOutputCode transpile_throw(sem::Throw& node);
    bool in_try_catch;
};


#endif //XLANG_STRANSPILER_H
