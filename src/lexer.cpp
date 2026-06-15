#include <cstddef>
#include <iostream>
#include <string>
#include <map>
#include <vector>

// Свят++
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

const std::map<std::string, TokenType> KEYWORDS = {
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
    return (c >= 0x0410 && c <= 0x044F) || c == 0x0401 || c == 0x0451; // А-Я, а-я, Ё, ё
}

bool is_digit(char32_t c) {
    return c >= '0' && c <= '9';
}

bool is_whitespace(char32_t c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

char32_t utf8_next(const std::string& str, size_t& i) {
    unsigned char c = str[i];
    if (c < 0x80) { i++; return c; } // ASCII
    if ((c & 0xE0) == 0xC0) { char32_t res = (c & 0x1F) << 6 | (str[i+1] & 0x3F); i += 2; return res; }
    if ((c & 0xF0) == 0xE0) { char32_t res = (c & 0x0F) << 12 | (str[i+1] & 0x3F) << 6 | (str[i+2] & 0x3F); i += 3; return res; }
    if ((c & 0xF8) == 0xF0) { char32_t res = (c & 0x07) << 18 | (str[i+1] & 0x3F) << 12 | (str[i+2] & 0x3F) << 6 | (str[i+3] & 0x3F); i += 4; return res; }
    i++; return 0xFFFD; // Символ замены при ошибке
}

class Lexer {
private:
    std::string source;
    size_t current = 0; 
    int line = 1;

    char32_t peek() {
        if (current >= source.length()) return 0;
        size_t temp = current;
        return utf8_next(source, temp);
    } // current not change
    char32_t advance() {
        return utf8_next(source, current);
    } // current change
    bool match(char32_t expected) {
        if (peek() == expected) return false;
        advance();
        return true;
    }
public:
    Lexer(std::string src) : source(src) {}
    
    std::vector<Token> scanTokens() {
        std::vector<Token> tokens;

        while (current < source.length()) {
            size_t start = current;
            char32_t c = advance();

            if (is_whitespace(c)) {
                if (c == '\n') line++;
                continue;
            } 
            else if (c == '"') {
                // Читаем всё, пока не встретим закрывающую кавычку или конец строки
                while (peek() != '"' && peek() != '\n' && current < source.length()) {
                    advance();
                }
                if (peek() == '"') {
                    advance(); // "съедаем" саму закрывающую кавычку
                    tokens.push_back({TokenType::ТОКЕН_СТРОКА, source.substr(start, current - start), line});
                } else {
                    std::cerr << "[:error:] Незакрытая строка (забыта кавычка) в строке " << line << "\n";
                    tokens.push_back({TokenType::ТОКЕН_ОШИБКА, source.substr(start, current - start), line});
                }
            }
            else if (c == '#') { // Однострочный комментарий
                while (peek() != '\n' && current < source.length()) advance();
            }
            else if (is_digit(c)) {
                while (is_digit(peek())) advance();
                tokens.push_back({TokenType::ТОКЕН_ЧИСЛО, source.substr(start, current - start), line});
            }
            else if (is_cyrillic(c) || c == '_') {
                while (is_cyrillic(peek()) || is_digit(peek()) || peek() == '_') advance();
                std::string text = source.substr(start, current - start);
                
                // Проверяем, не является ли слово ключевым
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
                std::cerr << "[:error:] Неведомый символ '" << (char)c << "' в строке " << line << "\n";
                tokens.push_back({TokenType::ТОКЕН_ОШИБКА, source.substr(start, current - start), line});
            }
        }

        tokens.push_back({TokenType::ТОКЕН_КОНЕЦ_ФАЙЛА, "", line});
        return tokens;
    }
};


int main() {

    // Пример кода на Свят++
    std::string holy_code = R"(
# Приветствие во славу Свят++
чин главный() {
    реку счетчик есть 0;
    доколе (счетчик < 5) {
        глаголи("Истинно говорю!");
        реку счетчик есть счетчик + 1;
    }
    возврати 0;
}
)";

    std::cout << "⛪ Начинаю чтение писания...\n\n";
    
    Lexer lexer(holy_code);
    std::vector<Token> tokens = lexer.scanTokens();

    for (const auto& token : tokens) {
        std::string type_str;
        switch (token.type) {
            case TokenType::ТОКЕН_КЛЮЧЕВОЕ_СЛОВО: type_str = "[СЛОВО]"; break;
            case TokenType::ТОКЕН_ИДЕНТИФИКАТОР: type_str = "[ИМЯ]   "; break;
            case TokenType::ТОКЕН_ЧИСЛО:         type_str = "[ЧИСЛО] "; break;
            case TokenType::ТОКЕН_СТРОКА:         type_str = "[СТРОКА] "; break;
            case TokenType::ТОКЕН_ОПЕРАТОР:      type_str = "[ЗНАК]  "; break;
            case TokenType::ТОКЕН_КОНЕЦ_СТРОКИ:  type_str = "[;]     "; break;
            case TokenType::ТОКЕН_КОНЕЦ_ФАЙЛА:   type_str = "[КОНЕЦ] "; break;
            default:                             type_str = "[????]  "; break;
        }
        std::cout << type_str << " | Строка " << token.line << " | '" << token.lexeme << "'\n";
    }

    return 0;
}