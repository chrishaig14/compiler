//
// Created by chris on 25/5/21.
//

#include "parse.h"
#include "Compiler.h"

void Compiler::parse_single_module(Module& module) {
    std::string __file__ = module.abs_path;
    Scanner scanner;
    scanner.load_file(__file__);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(__file__, scanner.code_lines, tokens);
    parser.top_package_name = this->top_package_name;
    module.code_lines = scanner.code_lines;
    BlockNode* ast;
    try {
        ast = parser.parse_program();
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        exit(1);
    }
    module.ast = ast;
}

void Compiler::parse_all_modules(Package& package) {
    if (package.units.empty()) {
        std::cerr << "Warning: package " << package.name << " (" << package.abs_path << ") is empty" << std::endl;
        return;
    }
    for (const auto& unit: package.units) {
        if (unit.second.type == U_TYPE::PACKAGE) {
            parse_all_modules(*unit.second.package);
        } else if (unit.second.type == U_TYPE::MODULE) {
            parse_single_module(*unit.second.module);
        }
    }
}
