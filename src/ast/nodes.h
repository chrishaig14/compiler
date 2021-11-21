//
// Created by chris on 1/8/20.
//

#ifndef NODES_H
#define NODES_H


#include "statements/Assignment.h"
#include "statements/Break.h"
#include "statements/Call.h"
#include "statements/Continue.h"
#include "statements/Declaration.h"
#include "statements/For.h"
#include "statements/If.h"
#include "statements/Match.h"
#include "statements/Return.h"
#include "statements/While.h"
#include "expressions/Cast.h"
#include "expressions/include/BinaryOp.h"
#include "expressions/include/Boolean.h"
#include "expressions/include/DefaultConstructor.h"
#include "expressions/include/DictNode.h"
#include "expressions/include/EmptyDict.h"
#include "expressions/include/EmptyList.h"
#include "expressions/include/Id.h"
#include "expressions/include/List.h"
#include "expressions/include/Member.h"
#include "expressions/include/None.h"
#include "expressions/include/Number.h"
#include "expressions/include/PartialApplication.h"
#include "expressions/include/String.h"
#include "expressions/include/Subscript.h"
#include "expressions/include/Ternary.h"
#include "expressions/include/Tuple.h"
#include "general/Type.h"
#include "top/Function.h"
#include "top/Import.h"
#include "top/TemplateClassDef.h"
#include "top/ConcreteClassDef.h"
#include "top/Instance.h"

#endif //NODES_H
