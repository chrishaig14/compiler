//
// Created by chris on 29/8/21.
//

#ifndef XLANG_SEM_H
#define XLANG_SEM_H

#include <vector>
#include <memory>

namespace sem {
    class Assignment;

    class Block;

    class Bool;

    class ConstFunctionCall;

    class ConstFunction;

    class ObjectMethod;

    class Break;

    class Call;

    class KlassDef;

    class Continue;

    class Declaration;

    class Dict;

    class EnumMember;

    class EnumDef;

    class Float;

    class ObjectConstructor;

    class ObjectConstructorCall;

    class FunctionDef;

    class Id;

    class IfSNode;

    class Integer;

    class List;

    class Match;

    class NewObject;

    class None;

    class ObjectMember;

    class Return;

    class String;

    class Ternary;

    class Throw;

    class TryCatch;

    class While;

    class SNode;

    class TypeObject;

    class TypeFunction;

    class CallExp;

    enum class Kind;

    class ObjectTypeData;

    class Type;

    class Exp;

    enum class SNodeType {
        FUNCTION,
        ID,
        DECLARATION,
        BLOCK,
        ASSIGNMENT,
        RETURN,
        INTEGER,
        CALL,
        STRING,
        BOOLEAN,
        FLOAT,
        CLASS,
        NEW,
        OBJECT_MEMBER,
        WHILE,
        LIST,
        IF,
        BREAK,
        CONTINUE,
        MATCH,
        ENUM,
        ENUM_MEMBER,
        NONE,
        TERNARY,
        DICT,
        THROW,
        TRY_CATCH,
        OBJECT_METHOD_CALL,
        OBJECT_METHOD,
        CONST_FUNCTION_CALL,
        CONST_FUNCTION,
        OBJECT_CONSTRUCTOR
    };

    enum class ExpType {
        FUNCTION,
        ID,
        INTEGER,
        CALL,
        STRING,
        BOOLEAN,
        FLOAT,
        NEW,
        OBJECT_MEMBER,
        LIST,
        MATCH,
        ENUM_MEMBER,
        NONE,
        TERNARY,
        DICT,
        OBJECT_METHOD_CALL,
        OBJECT_METHOD,
        CONST_FUNCTION_CALL,
        CONST_FUNCTION,
        OBJECT_CONSTRUCTOR
    };

    typedef std::unique_ptr<SNode> USNode;
    typedef std::unique_ptr<Exp> UExp;

    typedef std::unique_ptr<TypeObject> UTypeObject;
    typedef std::unique_ptr<TypeFunction> UTypeFunction;
    typedef std::unique_ptr<Type> UType;
    typedef std::vector<UType> VectorOfUTypes;

    typedef std::vector<Type*> VectorOfTypes;

}

#endif //XLANG_SEM_H
