//
// Created by chris on 25/5/21.
//

#include "parse.h"

BlockNode* full_parse(const std::string& __file__, CodeLines* code_lines) {
    BlockNode* tree;
    Scanner scanner;
    scanner.load_file(__file__);
    std::vector<Token> tokens = scanner.scan_all();
    // for (auto token: tokens) {
    //     std::cout << token.to_string() << std::endl;
    // }
    Parser parser(__file__, scanner.code_lines, tokens);
    *code_lines = scanner.code_lines;
    try {
        tree = parser.parse_program();
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        exit(0);
    }
    return tree;
}

void parse_all_modules(Package* package) {
    if (package->units.empty()) {
        std::cerr << "Warning: package " << package->name << " (" << package->abs_path << ") is empty" << std::endl;
        return;
    }
    for (const auto& ep: package->units) {
        if (ep.second.type == U_TYPE::PACKAGE) {
            Package* subpackage = ep.second.package;
            parse_all_modules((Package*) subpackage);
        } else if (ep.second.type == U_TYPE::MODULE) {
            Module* module = ep.second.module;
            CodeLines code_lines;
            BlockNode* ast = full_parse(module->abs_path, &code_lines);
            module->ast = ast;
            module->code_lines = code_lines;
        }
    }
}
