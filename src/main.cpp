#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

#include "lexer.hpp"
#include "parser.hpp"
#include "codegen.hpp"

// Вспомогательная функция: читаем весь файл в строку
std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + path);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    // Проверка аргументов
    if (argc < 2) {
        std::cerr << "⛪ Свят++ компилятор\n";
        std::cerr << "Использование: " << argv[0] << " <файл.svyat> [output]\n";
        std::cerr << "\nПример:\n";
        std::cerr << "  " << argv[0] << " example.svyat\n";
        std::cerr << "  " << argv[0] << " example.svyat my_program\n";
        return 1;
    }
    
    std::string input_file = argv[1];
    std::string output_name = (argc >= 3) ? argv[2] : "a.out";
    
    std::cout << "⛪ Свят++ компилятор\n";
    std::cout << "====================\n\n";
    
    try {
        // === Шаг 1: Чтение файла ===
        std::cout << "📂 Читаю файл: " << input_file << "\n";
        std::string holy_code = readFile(input_file);
        std::cout << "   ✓ Прочитано символов: " << holy_code.size() << "\n\n";
        
        // === Шаг 2: Лексический анализ ===
        std::cout << "📖 ЭТАП 1: Лексический анализ\n";
        Lexer lexer(holy_code);
        std::vector<Token> tokens = lexer.scanTokens();
        std::cout << "   ✓ Найдено токенов: " << tokens.size() << "\n\n";
        
        // === Шаг 3: Синтаксический анализ ===
        std::cout << "🔧 ЭТАП 2: Синтаксический анализ\n";
        Parser parser(tokens);
        std::unique_ptr<Program> ast = parser.parse();
        std::cout << "   ✓ Инструкций в AST: " << ast->statements.size() << "\n\n";
        
        // === Шаг 4: Генерация ассемблера ===
        std::cout << "⚙️  ЭТАП 3: Генерация кода (NASM x86-64)\n";
        CodeGen codegen;
        std::string asm_code = codegen.generate(*ast);
        
        // Сохраняем .asm файл
        std::string asm_file = output_name + ".asm";
        std::ofstream out(asm_file);
        if (!out.is_open()) {
            throw std::runtime_error("Не удалось создать файл: " + asm_file);
        }
        out << asm_code;
        out.close();
        std::cout << "   ✓ Создан файл: " << asm_file << "\n\n";
        
        // === Шаг 5: Сборка через nasm и gcc ===
        std::cout << "🔨 ЭТАП 4: Сборка исполняемого файла\n";
        
        std::string obj_file = output_name + ".o";
        
        // Команда для NASM
        std::string nasm_cmd = "nasm -f elf64 " + asm_file + " -o " + obj_file;
        std::cout << "   → " << nasm_cmd << "\n";
        if (std::system(nasm_cmd.c_str()) != 0) {
            throw std::runtime_error("Ошибка при сборке NASM");
        }
        
        // Команда для GCC (с -no-pie для простоты)
        std::string gcc_cmd = "gcc -no-pie " + obj_file + " -o " + output_name;
        std::cout << "   → " << gcc_cmd << "\n";
        if (std::system(gcc_cmd.c_str()) != 0) {
            throw std::runtime_error("Ошибка при линковке GCC");
        }
        
        std::cout << "   ✓ Исполняемый файл создан: " << output_name << "\n\n";
        
        // === Шаг 6: Успех ===
        std::cout << "🎉 КОМПИЛЯЦИЯ ЗАВЕРШЕНА УСПЕШНО!\n";
        std::cout << "   Запустите программу: ./" << output_name << "\n";
        
        // Можно сразу запустить
        std::cout << "\n▶️  Запускаю программу...\n";
        std::cout << "─────────────────────────\n";
        std::system(("./" + output_name).c_str());
        std::cout << "─────────────────────────\n";

    } catch (const std::exception& e) {
        std::cerr << "\n❌ Ошибка компиляции Свят++: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
