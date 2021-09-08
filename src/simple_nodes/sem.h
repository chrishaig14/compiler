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

    class ObjectMethodCall;

    class Break;

    class Call;

    class KlassDef;

    class Continue;

    class Declaration;

    class Dict;

    class EnumMember;

    class EnumDef;

    class Float;

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

    enum class Kind;

    class ObjectTypeData;

    class Type;

    typedef std::unique_ptr<TypeObject> UTypeObject;
    typedef std::unique_ptr<TypeFunction> UTypeFunction;
    typedef std::unique_ptr<Type> UType;
    typedef std::vector<UType> VectorOfUTypes;
    typedef std::vector<Type*> VectorOfTypes;

}

#endif //XLANG_SEM_H
