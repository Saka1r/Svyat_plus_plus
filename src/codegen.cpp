#include "codegen.hpp"

CodeGen::CodeGen() {
    // Заголовки NASM-файла
    data_section << "section .data\n";
    bss_section << "section .bss\n";
    text_section << "section .text\n";
	text_section << "default rel\n";
    text_section << "    global main\n\n";
    text_section << "main:\n";
    text_section << "    push rbp\n";
    text_section << "    mov rbp, rsp\n\n";
}

std::string CodeGen::generate(const Program& program) {
    // Генерируем код для каждой инструкции
    for (const auto& stmt : program.statements) {
        generateStmt(*stmt);
    }
    
    // Эпилог: возврат 0 из main
    text_section << "\n    ; Возврат 0 из main\n";
    text_section << "    mov rax, 0\n";
    text_section << "    pop rbp\n";
    text_section << "    ret\n";
    
    // Собираем всё вместе
    std::stringstream result;
    result << "; 🏰 Сгенерировано компилятором Свят++\n";
    result << "; Архитектура: Linux x86-64\n\n";
    result << data_section.str() << "\n";
    result << bss_section.str() << "\n";
    result << text_section.str();
    
    return result.str();
}

void CodeGen::generateStmt(const Stmt& stmt) {
    // Объявление переменной: реку имя есть значение;
    if (auto var_decl = dynamic_cast<const VarDeclStmt*>(&stmt)) {
        std::string asm_label = allocateVariable(var_decl->name);
        
        // Если значение - число, записываем его в переменную
        if (auto num = dynamic_cast<const NumberExpr*>(var_decl->init_value.get())) {
            text_section << "    ; реку " << var_decl->name << " есть " << num->value << "\n";
            text_section << "    mov qword [" << asm_label << "], " << num->value << "\n\n";
        }
        // Если значение - строка, запоминаем её адрес
        else if (auto str = dynamic_cast<const StringExpr*>(var_decl->init_value.get())) {
            std::string str_label = allocateString(str->value);
            text_section << "    ; реку " << var_decl->name << " есть строка\n";
            text_section << "    mov qword [" << asm_label << "], " << str_label << "\n\n";
        }
        return;
    }
    
    // Печать: глаголи(выражение);
    if (auto print_stmt = dynamic_cast<const PrintStmt*>(&stmt)) {
        // Если печатаем строку
        if (auto str = dynamic_cast<const StringExpr*>(print_stmt->value.get())) {
            std::string str_label = allocateString(str->value);
            
            text_section << "    ; глаголи(\"" << str->value << "\")\n";
            text_section << "    mov rax, 1              ; syscall: sys_write\n";
            text_section << "    mov rdi, 1              ; файловый дескриптор: stdout\n";
            text_section << "    mov rsi, " << str_label << " ; адрес строки\n";
            text_section << "    mov rdx, " << str_label << "_len ; длина строки\n";
            text_section << "    syscall\n\n";
        }
        return;
    }
}

void CodeGen::generateExpr(const Expr& expr) {
    // Пока заглушка - выражения обрабатываются внутри generateStmt
}

// Выделяем место для строки в .data
std::string CodeGen::allocateString(const std::string& value) {
    std::string label = ".str" + std::to_string(string_counter++);
    
    // Убираем кавычки из значения, если они есть
    std::string clean_value = value;
    if (clean_value.front() == '"') clean_value = clean_value.substr(1);
    if (clean_value.back() == '"') clean_value.pop_back();
    
    data_section << label << " db \"" << clean_value << "\", 10\n";
    data_section << label << "_len equ $ - " << label << "\n";
    
    return label;
}

// Выделяем место для переменной в .bss
std::string CodeGen::allocateVariable(const std::string& name) {
    std::string label = "var_" + name;
    bss_section << label << " resq 1    ; 8 байт (qword) для переменной '" << name << "'\n";
    variables[name] = label;
    return label;
}
