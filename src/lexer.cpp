#include "lexer.hpp"
#include <iostream>

static const std::map<std::string, TokenType> KEYWORDS = {
    {"реку", TokenType::ТОКЕН_КЛЮЧЕВОЕ_СЛОВО},
    {"есть", TokenType::ТОКЕН_КЛЮЧЕВОЕ_СЛОВО},
    {"аще", TokenType::ТОКЕН_КЛЮЧЕВОЕ_СЛОВО},
    {"ино", TokenType::ТОКЕН_КЛЮЧЕВОЕ_СЛОВО},
    {"доколе", TokenType::ТОКЕН_КЛЮЧЕВОЕ_СЛОВО},
    {"чин", TokenType::ТОКЕН_КЛЮЧЕВОЕ_СЛОВО},
    {"возврати", TokenType::ТОКЕН_КЛЮЧЕВОЕ_СЛОВО},
    {"глаголи", TokenType::ТОКЕН_КЛЮЧЕВОЕ_СЛОВО},
    {"цело", TokenType::ТОКЕН_КЛЮЧЕВОЕ_СЛОВО},
    {"слово", TokenType::ТОКЕН_КЛЮЧЕВОЕ_СЛОВО},
    {"пусто", TokenType::ТОКЕН_КЛЮЧЕВОЕ_СЛОВО},
    {"истина", TokenType::ТОКЕН_КЛЮЧЕВОЕ_СЛОВО},
    {"ложь", TokenType::ТОКЕН_КЛЮЧЕВОЕ_СЛОВО}
};

bool is_cyrillic(char32_t c) {
    return (c >= 0x0410 && c <= 0x044F) || c == 0x0401 || c == 0x0451;
}

bool is_digit(char32_t c) {
    return c >= '0' && c <= '9';
}

bool is_whitespace(char32_t c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

char32_t utf8_next(const std::string& str, size_t& i) {
    unsigned char c = str[i];
    if (c < 0x80) { i++; return c; }
    if ((c & 0xE0) == 0xC0) { char32_t res = (c & 0x1F) << 6 | (str[i+1] & 0x3F); i += 2; return res; }
    if ((c & 0xF0) == 0xE0) { char32_t res = (c & 0x0F) << 12 | (str[i+1] & 0x3F) << 6 | (str[i+2] & 0x3F); i += 3; return res; }
    if ((c & 0xF8) == 0xF0) { char32_t res = (c & 0x07) << 18 | (str[i+1] & 0x3F) << 12 | (str[i+2] & 0x3F) << 6 | (str[i+3] & 0x3F); i += 4; return res; }
    i++; return 0xFFFD; 
}

Lexer::Lexer(const std::string& src) : source(src) {}

bool Lexer::isAtEnd() const {
    return current >= source.length();
}

char32_t Lexer::peek() const {
    if (isAtEnd()) return 0;
    size_t temp = current;
    return utf8_next(source, temp);
}

char32_t Lexer::advance() {
    return utf8_next(source, current);
}

bool Lexer::match(char32_t expected) {
    if (peek() != expected) return false; // ИСПРАВЛЕНО: было ==
    advance();
    return true;
}

std::vector<Token> Lexer::scanTokens() {
    std::vector<Token> tokens;

    while (!isAtEnd()) {
        size_t start = current;
        char32_t c = advance();

        if (is_whitespace(c)) {
            if (c == '\n') line++;
            continue;
        } 
        else if (c == '"') {
            while (peek() != '"' && peek() != '\n' && !isAtEnd()) {
                advance();
            }
            if (peek() == '"') {
                advance(); 
                tokens.push_back({TokenType::ТОКЕН_СТРОКА, source.substr(start, current - start), line});
            } else {
                std::cerr << "[:error:] Незакрытая строка в строке " << line << "\n";
                tokens.push_back({TokenType::ТОКЕН_ОШИБКА, source.substr(start, current - start), line});
            }
        }
        else if (c == '#') { 
            while (peek() != '\n' && !isAtEnd()) advance();
        }
        else if (is_digit(c)) {
            while (is_digit(peek())) advance();
            tokens.push_back({TokenType::ТОКЕН_ЧИСЛО, source.substr(start, current - start), line});
        }
        else if (is_cyrillic(c) || c == '_') {
            while (is_cyrillic(peek()) || is_digit(peek()) || peek() == '_') advance();
            std::string text = source.substr(start, current - start);
            
            auto it = KEYWORDS.find(text);
            if (it != KEYWORDS.end()) {
                tokens.push_back({it->second, text, line});
            } else {
                tokens.push_back({TokenType::ТОКЕН_ИДЕНТИФИКАТОР, text, line});
            }
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '<' || c == '>' || c == '=' || c == '(' || c == ')' || c == '{' || c == '}') {
            tokens.push_back({TokenType::ТОКЕН_ОПЕРАТОР, source.substr(start, current - start), line});
        }
        else if (c == ';') {
            tokens.push_back({TokenType::ТОКЕН_КОНЕЦ_СТРОКИ, ";", line});
        }
        else {
            std::cerr << "[:error:] Неведомый символ в строке " << line << "\n";
            tokens.push_back({TokenType::ТОКЕН_ОШИБКА, source.substr(start, current - start), line});
        }
    }

    tokens.push_back({TokenType::ТОКЕН_КОНЕЦ_ФАЙЛА, "", line});
    return tokens;
}
