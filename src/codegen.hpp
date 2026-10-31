#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include "ast.hpp"

class CodeGen {
private:
    std::stringstream data_section;   // section .data
    std::stringstream bss_section;    // section .bss
    std::stringstream text_section;   // section .text
    
    int string_counter = 0;           // для уникальных имён строк
    std::map<std::string, std::string> variables; // имя_переменной -> имя_метки в asm
    
public:
    CodeGen();
    
    std::string generate(const Program& program);
    
private:
    void generateStmt(const Stmt& stmt);
    void generateExpr(const Expr& expr);
    
    std::string allocateString(const std::string& value);
    std::string allocateVariable(const std::string& name);
};

#endif
