#pragma once
#include <string>

namespace Tokens
{
    struct Token
    {
        enum class Type
        {
            ERROR = 0,
            INT_LITERAL,
            FLOAT_LITERAL,
            IDENTIFIER,
            KEYWORD,
            PUNCTUATION,
            BRACKET,
            MULT_OP,
            ADD_OP,
            REL_OP,
            ASSIGNMENT,
            LINE_COMMENT,
            BLOCK_COMMENT,
            NEW_LINE,
        };

        Token()
            : type(Type::ERROR), lexemeLength(UINT32_MAX)
        {}

        Token(Type type, const std::string& lexeme)
            : type(type), lexemeLength(lexeme.length())
        {}

    public:
        Type type;
        uint32_t lexemeLength;
    };

    struct IntegerLiteral : public Token
    {
        IntegerLiteral(const std::string& lexeme)
            : Token(Token::Type::INT_LITERAL, lexeme), value(std::stoi(lexeme))
        {}
    public:
        int value;
    };

    struct FloatLiteral : public Token
    {
        FloatLiteral(const std::string& lexeme)
            : Token(Token::Type::FLOAT_LITERAL, lexeme), value(std::stof(lexeme))
        {}
    public:
        float value;
    };

    struct Identifier : public Token
    {
        Identifier(const std::string& lexeme)
            : Token(Token::Type::IDENTIFIER, lexeme), name(lexeme)
        {}
    public:
        std::string name;
    };

    struct Keyword : public Token
    {
    #define KEYWORDS \
        X(float, FLOAT) \
        X(int, INT) \
        X(bool, BOOL) \
        X(colour, COLOUR) \
        X(true, TRUE) \
        X(false, FALSE) \
        X(let, LET) \
        X(if, IF) \
        X(for, FOR) \
        X(while, WHILE) \
        X(return, RETURN) \
        X(fun, FUN)

        enum class Type
        {
#define X(keyword, name) name,
            KEYWORDS
#undef X
        };

        Keyword(const std::string& lexeme)
            : Token(Token::Type::KEYWORD, lexeme)
        {
#define X(keyword, name) if(lexeme == #keyword) { type = Type::name; return;}
            KEYWORDS
#undef X
        }
    public:
        Type type;
    };

    struct Punctuation : public Token
    {
        enum class Type
        {
            SEMICOLON,
            COLON,
            COMMA,
            ARROW
        };

        Punctuation(const std::string& lexeme)
            : Token(Token::Type::PUNCTUATION, lexeme)
        {
            if (lexeme == "->")
            {
                type = Type::ARROW;
                return;
            }

            switch (lexeme[0])
            {
            case ';':
                type = Type::SEMICOLON;
                break;
            case ':':
                type = Type::COLON;
                break;
            case ',':
                type = Type::COMMA;
                break;
            }
        }
    public:
        Type type;
    };

    struct NewLine : public Token
    {
        NewLine(const std::string& lexeme)
            : Token(Token::Type::NEW_LINE, lexeme)
        {
        }
    };

    struct Bracket : public Token
    {
        enum class Type
        {
            OPEN_PAREN,
            CLOSE_PAREN,
            OPEN_SQ_BRACK,
            CLOSE_SQ_BRACK,
            OPEN_CURLY_BRACK,
            CLOSE_CURLY_BRACK,
        };

        Bracket(const std::string& lexeme)
            : Token(Token::Type::BRACKET, lexeme)
        {
            switch (lexeme[0])
            {
            case '(':
                type = Type::OPEN_PAREN;
                break;
            case ')':
                type = Type::CLOSE_PAREN;
                break;
            case '[':
                type = Type::OPEN_SQ_BRACK;
                break;
            case ']':
                type = Type::CLOSE_SQ_BRACK;
                break;
            case '{':
                type = Type::OPEN_CURLY_BRACK;
                break;
            case '}':
                type = Type::CLOSE_CURLY_BRACK;
                break;
            }
        }
    public:
        Type type;
    };

    struct MultiplicativeOp : public Token
    {
        enum class Type
        {
            MULTIPLY,
            DIVIDE,
        };

        MultiplicativeOp(const std::string& lexeme)
            : Token(Token::Type::MULT_OP, lexeme)
        {
            if (lexeme == "*")
            {
                type = Type::MULTIPLY;
            }
            else
            {
                type = Type::DIVIDE;
            }
        }
    public:
        Type type;
    };

    struct AdditiveOp : public Token
    {
        enum class Type
        {
            ADD,
            SUBTRACT,
        };

        AdditiveOp(const std::string& lexeme)
            : Token(Token::Type::ADD_OP, lexeme)
        {
            if (lexeme == "+")
            {
                type = Type::ADD;
            }
            else
            {
                type = Type::SUBTRACT;
            }
        }
    public:
        Type type;
    };

    struct RelationalOp : public Token
    {
        enum class Type
        {
            EQUAL,
            GREATER,
            LESS_THAN,
            GREATER_EQUAL,
            LESS_THAN_EQUAL,
        };

        RelationalOp(const std::string& lexeme)
            : Token(Token::Type::REL_OP, lexeme)
        {
            switch (lexeme[0])
            {
            case '=':
                type = Type::EQUAL;
                break;
            case '>':
                type = Type::GREATER;
                if(lexeme.length() == 2)
                    type = Type::GREATER_EQUAL;
                break;
            case '<':
                type = Type::LESS_THAN;
                if (lexeme.length() == 2)
                    type = Type::LESS_THAN_EQUAL;
                break;
            }
        }
    public:
        Type type;
    };

    struct Assignment : public Token
    {
        Assignment(const std::string& lexeme)
            : Token(Token::Type::ASSIGNMENT, lexeme)
        {
        }
    };

    struct LineComment : public Token
    {
        LineComment(const std::string& lexeme)
            : Token(Token::Type::LINE_COMMENT, lexeme)
        {
        }
    };

    struct BlockComment : public Token
    {
        BlockComment(const std::string& lexeme)
            : Token(Token::Type::BLOCK_COMMENT, lexeme), open(lexeme == "/*")
        {
        }

    public:
        bool open;
    };
}