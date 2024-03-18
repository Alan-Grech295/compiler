#pragma once
#include <string>
#include <unordered_set>
#include <memory>
#include <sstream>

#include "Utils/Utils.h"

namespace Tokens
{
    struct Token
    {
        enum class Type
        {
            ERROR = 0,
            WHITE_SPACE,
            VAR_TYPE,
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
            BUILTIN,
            END_OF_FILE,
        };

        Token(int length)
            : type(Type::ERROR), lexemeLength(length)
        {}

        Token(Type type, const std::string& lexeme)
            : type(type), lexemeLength((uint32_t)lexeme.length())
        {}

        virtual ~Token() = default;

        template<typename T>
        T& As() 
        {
            return *static_cast<T*>(this);
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

        static Scope<Token> Create(const std::string& lexeme)
        {
            return CreateScope<Whitespace>(lexeme);
        }

    public:
        static const Token::Type TokenType = Token::Type::WHITE_SPACE;
    };

    struct IntegerLiteral : public Token
    {
        IntegerLiteral(const std::string& lexeme)
            : Token(Token::Type::INT_LITERAL, lexeme), value(std::stoi(lexeme))
        {}

        static Scope<Token> Create(const std::string& lexeme)
        {
            return CreateScope<IntegerLiteral>(lexeme);
        }
    public:
        int value;

        static const Token::Type TokenType = Token::Type::INT_LITERAL;
    };

    struct FloatLiteral : public Token
    {
        FloatLiteral(const std::string& lexeme)
            : Token(Token::Type::FLOAT_LITERAL, lexeme), value(std::stof(lexeme))
        {}

        static Scope<Token> Create(const std::string& lexeme)
        {
            return CreateScope<FloatLiteral>(lexeme);
        }
    public:
        float value;

        static const Token::Type TokenType = Token::Type::FLOAT_LITERAL;
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

        static Scope<Token> Create(const std::string& lexeme)
        {
            return CreateScope<ColourLiteral>(lexeme);
        }
    public:
        int value = 0;

        static const Token::Type TokenType = Token::Type::COLOUR_LITERAL;
    };

    struct BooleanLiteral : public Token
    {
        BooleanLiteral(const std::string& lexeme)
            : Token(Token::Type::BOOLEAN_LITERAL, lexeme), value(lexeme == "true")
        {
        }

        static Scope<Token> Create(const std::string& lexeme)
        {
            return CreateScope<BooleanLiteral>(lexeme);
        }
    public:
        bool value = false;

        static const Token::Type TokenType = Token::Type::BOOLEAN_LITERAL;
    };

    struct VarType : public Token
    {
        enum class Type
        {
            UNKNOWN = 0,
            FLOAT,
            INT,
            BOOL,
            COLOUR,
        };

        VarType(const std::string& lexeme)
            : Token(Token::Type::VAR_TYPE, lexeme)
        {
            if (lexeme == "float")
            {
                type = Type::FLOAT;
            }
            else if (lexeme == "int")
            {
                type = Type::INT;
            }
            else if (lexeme == "bool")
            {
                type = Type::BOOL;
            }
            else if (lexeme == "colour")
            {
                type = Type::COLOUR;
            }
        }

        static Scope<Token> Create(const std::string& lexeme)
        {
            return CreateScope<VarType>(lexeme);
        }
    public:
        Type type;

        static const Token::Type TokenType = Token::Type::VAR_TYPE;
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

        static Scope<Token> Create(const std::string& lexeme)
        {
            return CreateScope<MultiplicativeOp>(lexeme);
        }
    public:
        Type type;

        static const Token::Type TokenType = Token::Type::MULT_OP;
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

        static Scope<Token> Create(const std::string& lexeme)
        {
            return CreateScope<AdditiveOp>(lexeme);
        }
    public:
        Type type;

        static const Token::Type TokenType = Token::Type::ADD_OP;
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

        static Scope<Token> Create(const std::string& lexeme)
        {
            return CreateScope<RelationalOp>(lexeme);
        }
    public:
        Type type;

        static const Token::Type TokenType = Token::Type::REL_OP;
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

        static Scope<Token> Create(const std::string& lexeme)
        {
            return CreateScope<UnaryOp>(lexeme);
        }
    public:
        Type type;

        static const Token::Type TokenType = Token::Type::UNARY_OP;
    };

    struct Keyword : public Token
    {
#define KEYWORDS \
        X(float, FLOAT, VarType) \
        X(int, INT, VarType) \
        X(bool, BOOL, VarType) \
        X(colour, COLOUR, VarType) \
        X(true, TRUE, BooleanLiteral) \
        X(false, FALSE, BooleanLiteral) \
        X(and, AND, MultiplicativeOp) \
        X(or, OR, AdditiveOp) \
        X(not, NOT, UnaryOp) \
        X(let, LET, Keyword) \
        X(if, IF, Keyword) \
        X(else, ELSE, Keyword) \
        X(for, FOR, Keyword) \
        X(while, WHILE, Keyword) \
        X(return, RETURN, Keyword) \
        X(as, AS, Keyword) \
        X(fun, FUN, Keyword)

        enum class Type
        {
#define X(keyword, name, _) name,
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

        static Scope<Token> Create(const std::string& lexeme)
        {
#define X(keyword, name, cls) if(lexeme == #keyword) { return CreateScope<::Tokens::cls>(lexeme); }
            KEYWORDS
#undef X
            //return CreateScope<Keyword>(lexeme);
        }
    public:
        Type type;

        static const std::unordered_set<std::string> keywords;

        static const Token::Type TokenType = Token::Type::KEYWORD;
    };

    struct Identifier : public Token
    {
        Identifier(const std::string& lexeme)
            : Token(Token::Type::IDENTIFIER, lexeme), name(lexeme)
        {}

        static Scope<Token> Create(const std::string& lexeme)
        {
            if (Keyword::keywords.contains(lexeme))
                return Keyword::Create(lexeme);

            return CreateScope<Identifier>(lexeme);
        }
    public:
        std::string name;

        static const Token::Type TokenType = Token::Type::IDENTIFIER;
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

        static Scope<Token> Create(const std::string& lexeme)
        {
            return CreateScope<Punctuation>(lexeme);
        }
    public:
        Type type;

        static const Token::Type TokenType = Token::Type::PUNCTUATION;
    };

    struct NewLine : public Token
    {
        NewLine(const std::string& lexeme)
            : Token(Token::Type::NEW_LINE, lexeme)
        {
        }

        static Scope<Token> Create(const std::string& lexeme)
        {
            return CreateScope<NewLine>(lexeme);
        }

    public:
        static const Token::Type TokenType = Token::Type::NEW_LINE;
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

        static Scope<Token> Create(const std::string& lexeme)
        {
            return CreateScope<Bracket>(lexeme);
        }
    public:
        Type type;

        static const Token::Type TokenType = Token::Type::BRACKET;
    };

    struct Assignment : public Token
    {
        Assignment(const std::string& lexeme)
            : Token(Token::Type::ASSIGNMENT, lexeme)
        {
        }

        static Scope<Token> Create(const std::string& lexeme)
        {
            return CreateScope<Assignment>(lexeme);
        }

    public:
        static const Token::Type TokenType = Token::Type::ASSIGNMENT;
    };

    struct LineComment : public Token
    {
        LineComment(const std::string& lexeme)
            : Token(Token::Type::LINE_COMMENT, lexeme)
        {
        }

        static Scope<Token> Create(const std::string& lexeme)
        {
            return CreateScope<LineComment>(lexeme);
        }

    public:
        static const Token::Type TokenType = Token::Type::LINE_COMMENT;

    };

    struct BlockComment : public Token
    {
        BlockComment(const std::string& lexeme)
            : Token(Token::Type::BLOCK_COMMENT, lexeme), open(lexeme == "/*")
        {
        }

        static Scope<Token> Create(const std::string& lexeme)
        {
            return CreateScope<BlockComment>(lexeme);
        }

    public:
        bool open;

        static const Token::Type TokenType = Token::Type::BLOCK_COMMENT;
    };

    struct Builtin : public Token
    {
#define BUILTIN_TYPES \
        X(__width, WIDTH) \
        X(__height, HEIGHT) \
        X(__read, READ) \
        X(__random_int, RANDOM_INT) \
        X(__print, PRINT) \
        X(__delay, DELAY) \
        X(__write_box, WRITE_BOX) \
        X(__write, WRITE) 

        enum class Type
        {
#define X(keyword, name) name,
            BUILTIN_TYPES
#undef X
        };

        Builtin(const std::string& lexeme)
            : Token(Token::Type::BUILTIN, lexeme)
        {
#define X(keyword, name) if(lexeme == #keyword) { type = Type::name; }
            BUILTIN_TYPES
#undef X
        }

        static Scope<Token> Create(const std::string& lexeme)
        {
            if (builtinTypes.contains(lexeme))
            {
                return CreateScope<Builtin>(lexeme);
            }

            return CreateScope<Token>(lexeme.length());
        }

    public:
        Type type;
        static const std::unordered_set<std::string> builtinTypes;

        static const Token::Type TokenType = Token::Type::BUILTIN;
    };
}