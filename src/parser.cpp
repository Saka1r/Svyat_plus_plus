#include "parser.hpp"
#include <iostream>
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

Token Parser::getCurrentToken() const {
    if (current >= tokens.size()) {
        return tokens.back(); // Возвращаем ТОКЕН_КОНЕЦ_ФАЙЛА
    }
    return tokens[current];
}

void Parser::moveToNextToken() {
    if (current < tokens.size()) {
        current++;
    }
}

bool Parser::isAtEnd() const {
    return getCurrentToken().type == TokenType::ТОКЕН_КОНЕЦ_ФАЙЛА;
}

std::unique_ptr<Program> Parser::parse() {
    auto program = std::make_unique<Program>();
    
    std::cout << "🔍 Начинаю синтаксический анализ...\n";
    
    while (!isAtEnd()) {
        std::cout << "  📖 Читаю инструкцию (токен #" << current << ": '" 
                  << getCurrentToken().lexeme << "')\n";
        
        auto stmt = parseStatement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
    }
    
    std::cout << "✅ Синтаксический анализ завершен!\n";
    return program;
}

std::unique_ptr<Stmt> Parser::parseStatement() {
    Token current_token = getCurrentToken();
    
    std::cout << "    🤔 Проверяю тип инструкции: '" << current_token.lexeme << "'\n";
    
    if (current_token.type == TokenType::ТОКЕН_КЛЮЧЕВОЕ_СЛОВО && current_token.lexeme == "реку") {
        return parseVarDecl();
    }
    
    if (current_token.type == TokenType::ТОКЕН_КЛЮЧЕВОЕ_СЛОВО && current_token.lexeme == "глаголи") {
        return parsePrintStmt();
    }
    
    if (current_token.type == TokenType::ТОКЕН_КЛЮЧЕВОЕ_СЛОВО && current_token.lexeme == "доколе") {
        throw std::runtime_error("Циклы пока не поддерживаются (твоя задача!)");
    }
    
    throw std::runtime_error("Неизвестная инструкция: '" + current_token.lexeme + 
                           "' в строке " + std::to_string(current_token.line));
}

std::unique_ptr<Stmt> Parser::parseVarDecl() {
    std::cout << "      📝 Разбираю объявление переменной\n";
    
    moveToNextToken(); // съедаем "реку"
    
    Token name_token = getCurrentToken();
    if (name_token.type != TokenType::ТОКЕН_ИДЕНТИФИКАТОР) {
        throw std::runtime_error("Ожидалось имя переменной после 'реку', получено: '" + 
                               name_token.lexeme + "' в строке " + std::to_string(name_token.line));
    }
    std::string var_name = name_token.lexeme;
    std::cout << "        ✓ Имя переменной: '" << var_name << "'\n";
    moveToNextToken();
    
    Token est_token = getCurrentToken();
    if (est_token.type != TokenType::ТОКЕН_КЛЮЧЕВОЕ_СЛОВО || est_token.lexeme != "есть") {
        throw std::runtime_error("Ожидалось 'есть' после имени переменной, получено: '" + 
                               est_token.lexeme + "' в строке " + std::to_string(est_token.line));
    }
    std::cout << "        ✓ Нашел 'есть'\n";
    moveToNextToken();
    
    std::cout << "        🔢 Разбираю выражение...\n";
    auto value = parseExpression();
    
    Token semicolon_token = getCurrentToken();
    if (semicolon_token.type != TokenType::ТОКЕН_КОНЕЦ_СТРОКИ) {
        throw std::runtime_error("Ожидалось ';' в конце объявления переменной, получено: '" + 
                               semicolon_token.lexeme + "' в строке " + std::to_string(semicolon_token.line));
    }
    std::cout << "        ✓ Нашел ';'\n";
    moveToNextToken();
    
    auto stmt = std::make_unique<VarDeclStmt>();
    stmt->name = var_name;
    stmt->init_value = std::move(value);
    
    std::cout << "      ✅ Объявление переменной разобрано!\n";
    return stmt;
}

// === НОВЫЙ МЕТОД ДЛЯ ГЛАГОЛИ ===
std::unique_ptr<Stmt> Parser::parsePrintStmt() {
    std::cout << "      📝 Разбираю инструкцию печати (глаголи)\n";
    
    moveToNextToken(); // съедаем "глаголи"
    
    if (getCurrentToken().lexeme != "(") {
        throw std::runtime_error("Ожидалось '(' после 'глаголи' в строке " + std::to_string(getCurrentToken().line));
    }
    moveToNextToken();
    
    auto value = parseExpression();
    
    if (getCurrentToken().lexeme != ")") {
        throw std::runtime_error("Ожидалось ')' после выражения в строке " + std::to_string(getCurrentToken().line));
    }
    moveToNextToken();
    
    if (getCurrentToken().type != TokenType::ТОКЕН_КОНЕЦ_СТРОКИ) {
        throw std::runtime_error("Ожидалось ';' в конце 'глаголи' в строке " + std::to_string(getCurrentToken().line));
    }
    std::cout << "        ✓ Нашел ';'\n";
    moveToNextToken();
    
    auto stmt = std::make_unique<PrintStmt>();
    stmt->value = std::move(value);
    
    std::cout << "      ✅ Инструкция печати разобрана!\n";
    return stmt;
}

std::unique_ptr<Expr> Parser::parseExpression() {
    Token token = getCurrentToken();
    
    std::cout << "          🔍 Анализирую выражение: '" << token.lexeme << "'\n";
    
    if (token.type == TokenType::ТОКЕН_ЧИСЛО) {
        auto expr = std::make_unique<NumberExpr>();
        expr->value = std::stoi(token.lexeme);
        std::cout << "          ✓ Это число: " << expr->value << "\n";
        moveToNextToken();
        return expr;
    }
    
    if (token.type == TokenType::ТОКЕН_ИДЕНТИФИКАТОР) {
        auto expr = std::make_unique<IdentifierExpr>();
        expr->name = token.lexeme;
        std::cout << "          ✓ Это имя переменной: '" << expr->name << "'\n";
        moveToNextToken();
        return expr;
    }
    
    if (token.type == TokenType::ТОКЕН_СТРОКА) {
        auto expr = std::make_unique<StringExpr>();
        expr->value = token.lexeme;
        std::cout << "          ✓ Это строка: " << expr->value << "\n";
        moveToNextToken();
        return expr;
    }
    
    throw std::runtime_error("Неожиданный токен в выражении: '" + token.lexeme + "' в строке " + std::to_string(token.line));
}
