//
// Created by chris on 4/4/21.
//

#ifndef XLANG_STRANSPILER_H
#define XLANG_STRANSPILER_H

#include "../simple_nodes/common/include/Block.h"
#include "../simple_nodes/common/include/FunctionDef.h"
#include "../simple_nodes/common/include/Assignment.h"
#include "../simple_nodes/common/include/Declaration.h"
#include "../simple_nodes/expressions/include/Integer.h"
#include "../simple_nodes/expressions/include/Id.h"
#include "../simple_nodes/common/include/Return.h"
#include "../simple_nodes/common/include/EnumDef.h"
#include "../simple_nodes/common/include/Call.h"
#include "../simple_nodes/expressions/include/String.h"
#include "../simple_nodes/expressions/include/Bool.h"
#include "../simple_nodes/expressions/include/Float.h"
#include "../simple_nodes/common/include/KlassDef.h"
#include "../simple_nodes/expressions/include/NewObject.h"
#include "../simple_nodes/common/include/Match.h"
#include "../simple_nodes/expressions/include/ObjectMember.h"
#include "../simple_nodes/common/include/While.h"
#include "../simple_nodes/expressions/include/List.h"
#include "../simple_nodes/common/include/IfSNode.h"
#include "../simple_nodes/common/include/Break.h"
#include "../simple_nodes/common/include/Continue.h"
#include "../simple_nodes/expressions/include/EnumMember.h"
#include "../simple_nodes/expressions/include/Ternary.h"
#include "../simple_nodes/expressions/include/None.h"
#include "../simple_nodes/expressions/include/Dict.h"
#include "../simple_nodes/common/include/Throw.h"
#include "../simple_nodes/common/include/TryCatch.h"

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
            case sem::SNodeType::FUNCTION:
                this->transpile_function((sem::FunctionDef&) (node));
                break;
            case sem::SNodeType::ENUM:
                this->transpile_enum((sem::EnumDef&) node);
                break;
            case sem::SNodeType::CLASS:
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

    CppOutputCode dispatch(sem::SNode& node);

    CppOutputCode transpile_throw(sem::Throw& node);
    bool in_try_catch;
};


#endif //XLANG_STRANSPILER_H
