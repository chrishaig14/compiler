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

    class For;

    class ObjectConstructor;

    class ObjectConstructorCall;

    class FunctionDef;

    class Id;

    class If;

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

    class TypeObject;

    class TypeFunction;

    class CallExp;

    enum class Kind;

    class ObjectTypeData;

    class Type;

    class Exp;

    class Common;

    class Top;

    class Module;

    class StaticMethod;

    enum class CommonType {
        DECLARATION, BLOCK, ASSIGNMENT, RETURN, CALL, WHILE, IF, BREAK, CONTINUE, MATCH, THROW, TRY_CATCH, FOR
    };

    enum class ExpType {
        ID,
        INTEGER,
        CALL,
        STRING,
        BOOLEAN,
        FLOAT,
        NEW,
        OBJECT_MEMBER,
        LIST,
        ENUM_MEMBER,
        NONE,
        TERNARY,
        DICT,
        OBJECT_METHOD,
        CONST_FUNCTION,
        OBJECT_CONSTRUCTOR, STATIC_METHOD
    };

    enum class TopType {
        CLASS, ENUM, FUNCTION
    };

    typedef std::unique_ptr<Common> UCommon;
    typedef std::unique_ptr<Exp> UExp;
    typedef std::unique_ptr<Top> UTop;

    typedef std::unique_ptr<TypeObject> UTypeObject;
    typedef std::unique_ptr<TypeFunction> UTypeFunction;
    typedef std::unique_ptr<Type> UType;
    typedef std::vector<UType> VectorOfUTypes;

    typedef std::vector<Type*> VectorOfTypes;

}

#endif //XLANG_SEM_H
