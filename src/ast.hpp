#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>

// === Базовые классы ===
struct Expr {
    virtual ~Expr() = default;
    virtual void print(int indent = 0) const = 0;
};

struct Stmt {
    virtual ~Stmt() = default;
    virtual void print(int indent = 0) const = 0;
};

// === Выражения (Expressions) ===
struct NumberExpr : Expr {
    int value;
    void print(int indent = 0) const override {
        std::cout << std::string(indent * 2, ' ') << "Число: " << value << "\n";
    }
};

struct IdentifierExpr : Expr {
    std::string name;
    void print(int indent = 0) const override {
        std::cout << std::string(indent * 2, ' ') << "Переменная: '" << name << "'\n";
    }
};

struct StringExpr : Expr {
    std::string value;
    void print(int indent = 0) const override {
        std::cout << std::string(indent * 2, ' ') << "Строка: " << value << "\n";
    }
};

// === Инструкции (Statements) ===
struct VarDeclStmt : Stmt {
    std::string name;
    std::unique_ptr<Expr> init_value;
    
    void print(int indent = 0) const override {
        std::cout << std::string(indent * 2, ' ') 
                  << "Объявление переменной: '" << name << "'\n";
        if (init_value) {
            init_value->print(indent + 1);
        }
    }
};

// ВОТ ОНА, ТА САМАЯ СТРУКТУРА, КОТОРОЙ НЕ ХВАТАЛО:
struct PrintStmt : Stmt {
    std::unique_ptr<Expr> value;
    
    void print(int indent = 0) const override {
        std::cout << std::string(indent * 2, ' ') << "Печать (глаголи):\n";
        if (value) {
            value->print(indent + 1);
        }
    }
};

// === Корень программы ===
struct Program {
    std::vector<std::unique_ptr<Stmt>> statements;
    
    void print() const {
        std::cout << "\n🌳 Дерево AST:\n";
        for (const auto& stmt : statements) {
            stmt->print(1);
        }
    }
};

#endif // AST_HPP
