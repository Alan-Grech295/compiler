#pragma once
#include <string>
#include <unordered_set>
#include <memory>
#include <sstream>

namespace Tokens
{
    struct Token
    {
        enum class Type
        {
            ERROR = 0,
            WHITE_SPACE,
            TYPE,
            INT_LITERAL,
            FLOAT_LITERAL,
            COLOUR_LITERAL,
            BOOLEAN_LITERAL,
            IDENTIFIER,
            KEYWORD,
            PUNCTUATION,
            BRACKET,
            MULT_OP,
            ADD_OP,
            REL_OP,
            UNARY_OP,
            ASSIGNMENT,
            LINE_COMMENT,
            BLOCK_COMMENT,
            NEW_LINE, 
            END_OF_FILE,
        };

        Token(int length)
            : type(Type::ERROR), lexemeLength(length)
        {}

        Token(Type type, const std::string& lexeme)
            : type(type), lexemeLength(lexeme.length())
        {}

        virtual ~Token() = default;

        template<typename T>
        T& As() 
        {
            return *(T*)this;
        }
    public:
        Type type;
        uint32_t lexemeLength;
    };

    struct Whitespace : public Token
    {
        Whitespace(const std::string& lexeme)
            : Token(Token::Type::WHITE_SPACE, lexeme)
        {}

        static std::unique_ptr<Token> Create(const std::string& lexeme)
        {
            return std::make_unique<Whitespace>(lexeme);
        }
    };

    struct IntegerLiteral : public Token
    {
        IntegerLiteral(const std::string& lexeme)
            : Token(Token::Type::INT_LITERAL, lexeme), value(std::stoi(lexeme))
        {}

        static std::unique_ptr<Token> Create(const std::string& lexeme)
        {
            return std::make_unique<IntegerLiteral>(lexeme);
        }
    public:
        int value;
    };

    struct FloatLiteral : public Token
    {
        FloatLiteral(const std::string& lexeme)
            : Token(Token::Type::FLOAT_LITERAL, lexeme), value(std::stof(lexeme))
        {}

        static std::unique_ptr<Token> Create(const std::string& lexeme)
        {
            return std::make_unique<FloatLiteral>(lexeme);
        }
    public:
        float value;
    };

    struct ColourLiteral : public Token
    {
        ColourLiteral(const std::string& lexeme)
            : Token(Token::Type::COLOUR_LITERAL, lexeme)
        {
            std::stringstream ss;
            ss << std::hex << lexeme.substr(1, 6);
            ss >> value;
        }

        static std::unique_ptr<Token> Create(const std::string& lexeme)
        {
            return std::make_unique<ColourLiteral>(lexeme);
        }
    public:
        int value = 0;
    };

    struct BooleanLiteral : public Token
    {
        BooleanLiteral(const std::string& lexeme)
            : Token(Token::Type::BOOLEAN_LITERAL, lexeme), value(lexeme == "true")
        {
        }

        static std::unique_ptr<Token> Create(const std::string& lexeme)
        {
            return std::make_unique<BooleanLiteral>(lexeme);
        }
    public:
        bool value = false;
    };

    struct Type : public Token
    {
        enum class Name
        {
            FLOAT,
            INT,
            BOOL,
            COLOUR,
        };

        Type(const std::string& lexeme)
            : Token(Token::Type::TYPE, lexeme)
        {
            if (lexeme == "float")
            {
                name = Name::FLOAT;
            }
            else if (lexeme == "int")
            {
                name = Name::INT;
            }
            else if (lexeme == "bool")
            {
                name = Name::BOOL;
            }
            else if (lexeme == "colour")
            {
                name = Name::COLOUR;
            }
        }

        static std::unique_ptr<Token> Create(const std::string& lexeme)
        {
            return std::make_unique<Type>(lexeme);
        }
    public:
        Name name;
    };

    struct MultiplicativeOp : public Token
    {
        enum class Type
        {
            MULTIPLY,
            DIVIDE,
            AND,
        };

        MultiplicativeOp(const std::string& lexeme)
            : Token(Token::Type::MULT_OP, lexeme)
        {
            if (lexeme == "*")
            {
                type = Type::MULTIPLY;
            }
            else if (lexeme == "/")
            {
                type = Type::DIVIDE;
            }
            else
            {
                type = Type::AND;
            }
        }

        static std::unique_ptr<Token> Create(const std::string& lexeme)
        {
            return std::make_unique<MultiplicativeOp>(lexeme);
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
            OR,
        };

        AdditiveOp(const std::string& lexeme)
            : Token(Token::Type::ADD_OP, lexeme)
        {
            if (lexeme == "+")
            {
                type = Type::ADD;
            }
            else if (lexeme == "-")
            {
                type = Type::SUBTRACT;
            }
            else
            {
                type = Type::OR;
            }
        }

        static std::unique_ptr<Token> Create(const std::string& lexeme)
        {
            return std::make_unique<AdditiveOp>(lexeme);
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
                if (lexeme.length() == 2)
                    type = Type::GREATER_EQUAL;
                break;
            case '<':
                type = Type::LESS_THAN;
                if (lexeme.length() == 2)
                    type = Type::LESS_THAN_EQUAL;
                break;
            }
        }

        static std::unique_ptr<Token> Create(const std::string& lexeme)
        {
            return std::make_unique<RelationalOp>(lexeme);
        }
    public:
        Type type;
    };

    struct UnaryOp : public Token
    {
        enum class Type
        {
            NOT,
        };

        UnaryOp(const std::string& lexeme)
            : Token(Token::Type::UNARY_OP, lexeme)
        {
            type = Type::NOT;
        }

        static std::unique_ptr<Token> Create(const std::string& lexeme)
        {
            return std::make_unique<UnaryOp>(lexeme);
        }
    public:
        Type type;
    };

    struct Keyword : public Token
    {
#define KEYWORDS \
        X(float, FLOAT, Type) \
        X(int, INT, Type) \
        X(bool, BOOL, Type) \
        X(colour, COLOUR, Type) \
        X(true, TRUE, BooleanLiteral) \
        X(false, FALSE, BooleanLiteral) \
        X(and, AND, MultiplicativeOp) \
        X(or, OR, AdditiveOp) \
        X(not, NOT, UnaryOp) \
        X(let, LET, Keyword) \
        X(if, IF, Keyword) \
        X(for, FOR, Keyword) \
        X(while, WHILE, Keyword) \
        X(return, RETURN, Keyword) \
        X(as, AS, Keyword) \
        X(fun, FUN, Keyword)

        enum class Type
        {
#define X(keyword, name) name,
            KEYWORDS
#undef X
        };

        Keyword(const std::string& lexeme)
            : Token(Token::Type::KEYWORD, lexeme)
        {
#define X(keyword, name, _) if(lexeme == #keyword) { type = Type::name; return;}
            KEYWORDS
#undef X
        }

        static std::unique_ptr<Token> Create(const std::string& lexeme)
        {
#define X(keyword, name, cls) if(lexeme == #keyword) { return std::make_unique<::Tokens::cls>(lexeme); }
            KEYWORDS
#undef X
            //return std::make_unique<Keyword>(lexeme);
        }
    public:
        Type type;

        static const std::unordered_set<std::string> keywords;
    };

    struct Identifier : public Token
    {
        Identifier(const std::string& lexeme)
            : Token(Token::Type::IDENTIFIER, lexeme), name(lexeme)
        {}

        static std::unique_ptr<Token> Create(const std::string& lexeme)
        {
            if (Keyword::keywords.contains(lexeme))
                return Keyword::Create(lexeme);

            return std::make_unique<Identifier>(lexeme);
        }
    public:
        std::string name;
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

        static std::unique_ptr<Token> Create(const std::string& lexeme)
        {
            return std::make_unique<Punctuation>(lexeme);
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

        static std::unique_ptr<Token> Create(const std::string& lexeme)
        {
            return std::make_unique<NewLine>(lexeme);
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

        static std::unique_ptr<Token> Create(const std::string& lexeme)
        {
            return std::make_unique<Bracket>(lexeme);
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

        static std::unique_ptr<Token> Create(const std::string& lexeme)
        {
            return std::make_unique<Assignment>(lexeme);
        }
    };

    struct LineComment : public Token
    {
        LineComment(const std::string& lexeme)
            : Token(Token::Type::LINE_COMMENT, lexeme)
        {
        }

        static std::unique_ptr<Token> Create(const std::string& lexeme)
        {
            return std::make_unique<LineComment>(lexeme);
        }
    };

    struct BlockComment : public Token
    {
        BlockComment(const std::string& lexeme)
            : Token(Token::Type::BLOCK_COMMENT, lexeme), open(lexeme == "/*")
        {
        }

        static std::unique_ptr<Token> Create(const std::string& lexeme)
        {
            return std::make_unique<BlockComment>(lexeme);
        }

    public:
        bool open;
    };
}