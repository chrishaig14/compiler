//
// Created by chris on 25/5/21.
//

#ifndef XLANG_ANALYZE_H
#define XLANG_ANALYZE_H

#include <units/infos/Module.h>
#include <semantic/ModuleChecker.h>


void resolve_module_imports(Module& module, Package& top_package);

#endif //XLANG_ANALYZE_H
