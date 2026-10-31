#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include <map>

enum class TokenType {
    ТОКЕН_КЛЮЧЕВОЕ_СЛОВО,
    ТОКЕН_ИДЕНТИФИКАТОР,
    ТОКЕН_ЧИСЛО,
    ТОКЕН_СТРОКА,
    ТОКЕН_ОПЕРАТОР,
    ТОКЕН_КОНЕЦ_СТРОКИ, // ;
    ТОКЕН_КОНЕЦ_ФАЙЛА,
    ТОКЕН_ОШИБКА
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};

class Lexer {
public:
    explicit Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    std::string source;
    size_t current = 0;
    int line = 1;

    char32_t peek() const;
    char32_t advance();
    bool match(char32_t expected);
    bool isAtEnd() const;
};

#endif // LEXER_HPP
