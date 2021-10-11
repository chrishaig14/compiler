//
// Created by chris on 11/10/21.
//

#ifndef XLANG_PYTHONPROGRAMTRANSPILER_H
#define XLANG_PYTHONPROGRAMTRANSPILER_H


#include "PythonModuleTranspiler.h"

class PythonProgramTranspiler {
public:
    void transpile_module(Module& module, const std::string& output_dir);

    void transpile_package(Package& package, const std::string& output_dir);
};


#endif //XLANG_PYTHONPROGRAMTRANSPILER_H
