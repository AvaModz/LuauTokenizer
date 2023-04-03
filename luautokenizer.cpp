#include <iostream>
#include <vector>
#include <string>

// Token types
enum TokenType {
    UNKNOWN,
    KEYWORD,
    IDENTIFIER,
    OPERATOR,
    NUMERIC_LITERAL,
    COMMENT
};

// Token structure
struct Token {
    TokenType type;
    std::string value;
    int line;
};

class Tokenizer {
public:
    Tokenizer(const std::string& sourceCode) 
        : m_sourceCode(sourceCode), m_currentChar(m_sourceCode[0]), m_position(0), m_line(1)
    {
    }

    std::vector<Token> tokenize()
    {
        std::vector<Token> tokens;

        while (m_currentChar != '\0') {
            try {
                switch (m_currentChar) {
                    case ' ':
                    case '\t':
                        skipWhitespace();
                        break;
                    case '\n':
                        consumeChar();
                       	m_line++;
                        break;
                    case '-':
                        consumeChar();
                        if (m_currentChar == '-') {
                            tokens.push_back(scanComment());
                        }
                        else {
                            tokens.push_back(scanOperator('-'));
                        }
                        break;
                    case '+':
                    case '*':
                    case '/':
                    case '^':
                    case '%':
                    case '#':
                        tokens.push_back(scanOperator());
                        break;
                    case '>':
                    case '<':
                    case '=':
                    case '~':
                        tokens.push_back(scanOperator());
                        break;
                    case '(':
                    case ')':
                    case '{':
                    case '}':
                    case ';':
                    case ',':
                        tokens.push_back(scanSymbol());
                        break;
                    case '[':
                        tokens.push_back(scanSymbol('['));
                        break;
                    case ']':
                        tokens.push_back(scanSymbol(']'));
                        break;
                    case '.':
                        tokens.push_back(scanDot());
                        break;
                    default:
                        if (isdigit(m_currentChar)) {
                            tokens.push_back(scanNumericLiteral());
                        }
                        else if (isalpha(m_currentChar) || m_currentChar == '_') {
                            tokens.push_back(scanIdentifier());
                        }
                        else {
                            throw std::runtime_error("Unexpected character");
                        }
                        break;
                }
            }
            catch (std::exception& e) {
                std::cerr << "Error on line " << m_line << ": " << e.what() << std::endl;
                skipUntilNextLine();
            }
        }

        return tokens;
    }

private:
    std::string m_sourceCode;
    char m_currentChar;
    int m_position;
    int m_line;

    char getNextChar()
    {
        return m_sourceCode[m_position + 1];
    }

    void consumeChar()
    {
        m_position++;
        m_currentChar = m_position < m_sourceCode.size() ? m_sourceCode[m_position] : '\0';
    }

    void skipWhitespace()
    {
        while (isspace(m_currentChar)) {
            consumeChar();
        }
    }

    void skipUntilNextLine()
    {
        while (m_currentChar != '\n' && m_currentChar != '\0') {
            consumeChar();
        }
    }

    Token scanComment()
    {
        std::string commentValue;
        commentValue += m_currentChar;
        consumeChar();

        while (m_currentChar != '\n' && m_currentChar != '\0') {
            commentValue += m_currentChar;
            consumeChar();
        }

        return { COMMENT, commentValue, m_line };
    }

    Token scanOperator(char firstChar = '\0')
    {
        std::string operatorValue;
        if (firstChar != '\0') {
            operatorValue += firstChar;
        }
        operatorValue += m_currentChar;
        consumeChar();

    if ((firstChar == '\0' && (m_currentChar == '<' || m_currentChar == '>' || m_currentChar == '=' || m_currentChar == '~')) || 
        (firstChar != '\0' && m_currentChar == '=')) {
        operatorValue += m_currentChar;
        consumeChar();
    }

    return { OPERATOR, operatorValue, m_line };
}

Token scanSymbol(char symbolChar = '\0')
{
    std::string symbolValue;
    if (symbolChar != '\0') {
        symbolValue += symbolChar;
    }
    else {
        symbolValue += m_currentChar;
        consumeChar();
    }

    return { UNKNOWN, symbolValue, m_line };
}

Token scanDot()
{
    std::string dotValue;
    dotValue += m_currentChar;
    consumeChar();

    if (m_currentChar == '.') {
        dotValue += m_currentChar;
        consumeChar();

        if (m_currentChar == '.') {
            dotValue += m_currentChar;
            consumeChar();

            return { UNKNOWN, dotValue, m_line };
        }

        throw std::runtime_error("Unexpected character");
    }

    return { UNKNOWN, dotValue, m_line };
}

Token scanNumericLiteral()
{
    std::string numValue;
    bool hasDecimal = false;

    while (isdigit(m_currentChar) || m_currentChar == '.') {
        if (m_currentChar == '.') {
            if (hasDecimal) {
                throw std::runtime_error("Invalid numeric literal");
            }
            hasDecimal = true;
        }

        numValue += m_currentChar;
        consumeChar();
    }

    return { NUMERIC_LITERAL, numValue, m_line };
}

Token scanIdentifier()
{
    std::string idValue;

    while (isalnum(m_currentChar) || m_currentChar == '_') {
        idValue += m_currentChar;
        consumeChar();
    }

    // Check if identifier is a keyword
    if (idValue == "and" || idValue == "break" || idValue == "do" || idValue == "else" || idValue == "elseif" ||
        idValue == "end" || idValue == "false" || idValue == "for" || idValue == "function" || idValue == "if" ||
        idValue == "in" || idValue == "local" || idValue == "nil" || idValue == "not" || idValue == "or" ||
        idValue == "repeat" || idValue == "return" || idValue == "then" || idValue == "true" || idValue == "until" ||
        idValue == "while") {
        return { KEYWORD, idValue, m_line };
    }

    return { IDENTIFIER, idValue, m_line };
}

