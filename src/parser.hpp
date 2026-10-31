#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <memory>
#include <string>
#include "lexer.hpp"
#include "ast.hpp"

class Parser {
private:
    std::vector<Token> tokens;
    size_t current = 0;
    
    Token getCurrentToken() const;
    void moveToNextToken();
    bool isAtEnd() const;
    
    std::unique_ptr<Stmt> parseStatement();
    std::unique_ptr<Stmt> parseVarDecl();
    std::unique_ptr<Stmt> parsePrintStmt(); // <--- ДОБАВЬ ЭТУ СТРОКУ
    std::unique_ptr<Expr> parseExpression();

public:
    explicit Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Program> parse();
};

#endif
