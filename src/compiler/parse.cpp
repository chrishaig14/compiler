//
// Created by chris on 25/5/21.
//

#include "parse.h"
#include "Compiler.h"

bool parse_module(Module& module) {
    std::string __file__ = module.abs_path;
    Scanner scanner;
    scanner.load_file(__file__);
    std::vector<Token> tokens = scanner.scan_all();
    Parser parser(__file__, scanner.code_lines, tokens);
    module.code_lines = scanner.code_lines;
    try {
        std::unique_ptr<ast::Module> ast = parser.parse_module();
        assert(ast != nullptr);
        module.ast = std::move(ast);
    } catch (std::runtime_error& e) {
        std::cout << "Error: " << e.what() << std::endl;
        std::cout << "Parsing for module " << module.name << " failed" << std::endl;
        return false;
    }
    return true;
}

bool parse_package(Package& package) {
    bool ok = true;
    if (package.units.empty()) {
        std::cerr << "Warning: package " << package.name << " (" << package.abs_path << ") is empty" << std::endl;
        return ok;
    }
    for (const auto& unit: package.units) {
        Unit* punit = unit.second;
        if (punit->is_package()) {
            ok &= parse_package(punit->package());
        } else if (punit->is_module()) {
            ok &= parse_module(punit->module());
        }
    }
    return ok;
}
