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
            : type(Type::ERROR), lexemeLength(length), startIndex(0)
        {}

        Token(Type type, const std::string& lexeme, uint32_t startIndex)
            : type(type), lexemeLength((uint32_t)lexeme.length()), startIndex(startIndex)
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
        uint32_t startIndex = 0;
    };

    struct Whitespace : public Token
    {
        Whitespace(const std::string& lexeme, uint32_t startIndex)
            : Token(Token::Type::WHITE_SPACE, lexeme, startIndex)
        {}

        static Scope<Token> Create(const std::string& lexeme, uint32_t startIndex)
        {
            return CreateScope<Whitespace>(lexeme, startIndex);
        }

    public:
        static const Token::Type TokenType = Token::Type::WHITE_SPACE;
    };

    struct IntegerLiteral : public Token
    {
        IntegerLiteral(const std::string& lexeme, uint32_t startIndex)
            : Token(Token::Type::INT_LITERAL, lexeme, startIndex), value(std::stoi(lexeme))
        {}

        static Scope<Token> Create(const std::string& lexeme, uint32_t startIndex)
        {
            return CreateScope<IntegerLiteral>(lexeme, startIndex);
        }
    public:
        int value;

        static const Token::Type TokenType = Token::Type::INT_LITERAL;
    };

    struct FloatLiteral : public Token
    {
        FloatLiteral(const std::string& lexeme, uint32_t startIndex)
            : Token(Token::Type::FLOAT_LITERAL, lexeme, startIndex), value(std::stof(lexeme))
        {}

        static Scope<Token> Create(const std::string& lexeme, uint32_t startIndex)
        {
            return CreateScope<FloatLiteral>(lexeme, startIndex);
        }
    public:
        float value;

        static const Token::Type TokenType = Token::Type::FLOAT_LITERAL;
    };

    struct ColourLiteral : public Token
    {
        ColourLiteral(const std::string& lexeme, uint32_t startIndex)
            : Token(Token::Type::COLOUR_LITERAL, lexeme, startIndex)
        {
            std::stringstream ss;
            ss << std::hex << lexeme.substr(1, 6);
            ss >> value;
        }

        static Scope<Token> Create(const std::string& lexeme, uint32_t startIndex)
        {
            return CreateScope<ColourLiteral>(lexeme, startIndex);
        }
    public:
        int value = 0;

        static const Token::Type TokenType = Token::Type::COLOUR_LITERAL;
    };

    struct BooleanLiteral : public Token
    {
        BooleanLiteral(const std::string& lexeme, uint32_t startIndex)
            : Token(Token::Type::BOOLEAN_LITERAL, lexeme, startIndex), value(lexeme == "true")
        {
        }

        static Scope<Token> Create(const std::string& lexeme, uint32_t startIndex)
        {
            return CreateScope<BooleanLiteral>(lexeme, startIndex);
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

        VarType(const std::string& lexeme, uint32_t startIndex)
            : Token(Token::Type::VAR_TYPE, lexeme, startIndex)
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

        static Scope<Token> Create(const std::string& lexeme, uint32_t startIndex)
        {
            return CreateScope<VarType>(lexeme, startIndex);
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
            MOD,
            AND,
        };

        MultiplicativeOp(const std::string& lexeme, uint32_t startIndex)
            : Token(Token::Type::MULT_OP, lexeme, startIndex)
        {
            if (lexeme == "*")
            {
                type = Type::MULTIPLY;
            }
            else if (lexeme == "/")
            {
                type = Type::DIVIDE;
            }
            else if (lexeme == "%")
            {
                type = Type::MOD;
            }
            else
            {
                type = Type::AND;
            }
        }

        static Scope<Token> Create(const std::string& lexeme, uint32_t startIndex)
        {
            return CreateScope<MultiplicativeOp>(lexeme, startIndex);
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

        AdditiveOp(const std::string& lexeme, uint32_t startIndex)
            : Token(Token::Type::ADD_OP, lexeme, startIndex)
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

        static Scope<Token> Create(const std::string& lexeme, uint32_t startIndex)
        {
            return CreateScope<AdditiveOp>(lexeme, startIndex);
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
            NOT_EQUAL,
            GREATER,
            LESS_THAN,
            GREATER_EQUAL,
            LESS_THAN_EQUAL,
        };

        RelationalOp(const std::string& lexeme, uint32_t startIndex)
            : Token(Token::Type::REL_OP, lexeme, startIndex)
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
            case '!':
                type = Type::NOT_EQUAL;
                break;
            }
        }

        static Scope<Token> Create(const std::string& lexeme, uint32_t startIndex)
        {
            return CreateScope<RelationalOp>(lexeme, startIndex);
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

        UnaryOp(const std::string& lexeme, uint32_t startIndex)
            : Token(Token::Type::UNARY_OP, lexeme, startIndex)
        {
            type = Type::NOT;
        }

        static Scope<Token> Create(const std::string& lexeme, uint32_t startIndex)
        {
            return CreateScope<UnaryOp>(lexeme, startIndex);
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

        Keyword(const std::string& lexeme, uint32_t startIndex)
            : Token(Token::Type::KEYWORD, lexeme, startIndex)
        {
#define X(keyword, name, _) if(lexeme == #keyword) { type = Type::name; return;}
            KEYWORDS
#undef X
        }

        static Scope<Token> Create(const std::string& lexeme, uint32_t startIndex)
        {
#define X(keyword, name, cls) if(lexeme == #keyword) { return CreateScope<::Tokens::cls>(lexeme, startIndex); }
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
        Identifier(const std::string& lexeme, uint32_t startIndex)
            : Token(Token::Type::IDENTIFIER, lexeme, startIndex), name(lexeme)
        {}

        static Scope<Token> Create(const std::string& lexeme, uint32_t startIndex)
        {
            if (Keyword::keywords.contains(lexeme))
                return Keyword::Create(lexeme, startIndex);

            return CreateScope<Identifier>(lexeme, startIndex);
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

        Punctuation(const std::string& lexeme, uint32_t startIndex)
            : Token(Token::Type::PUNCTUATION, lexeme, startIndex)
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

        static Scope<Token> Create(const std::string& lexeme, uint32_t startIndex)
        {
            return CreateScope<Punctuation>(lexeme, startIndex);
        }
    public:
        Type type;

        static const Token::Type TokenType = Token::Type::PUNCTUATION;
    };

    struct NewLine : public Token
    {
        NewLine(const std::string& lexeme, uint32_t startIndex)
            : Token(Token::Type::NEW_LINE, lexeme, startIndex)
        {
        }

        static Scope<Token> Create(const std::string& lexeme, uint32_t startIndex)
        {
            return CreateScope<NewLine>(lexeme, startIndex);
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

        Bracket(const std::string& lexeme, uint32_t startIndex)
            : Token(Token::Type::BRACKET, lexeme, startIndex)
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

        static Scope<Token> Create(const std::string& lexeme, uint32_t startIndex)
        {
            return CreateScope<Bracket>(lexeme, startIndex);
        }
    public:
        Type type;

        static const Token::Type TokenType = Token::Type::BRACKET;
    };

    struct Assignment : public Token
    {
        Assignment(const std::string& lexeme, uint32_t startIndex)
            : Token(Token::Type::ASSIGNMENT, lexeme, startIndex)
        {
        }

        static Scope<Token> Create(const std::string& lexeme, uint32_t startIndex)
        {
            return CreateScope<Assignment>(lexeme, startIndex);
        }

    public:
        static const Token::Type TokenType = Token::Type::ASSIGNMENT;
    };

    struct LineComment : public Token
    {
        LineComment(const std::string& lexeme, uint32_t startIndex)
            : Token(Token::Type::LINE_COMMENT, lexeme, startIndex)
        {
        }

        static Scope<Token> Create(const std::string& lexeme, uint32_t startIndex)
        {
            return CreateScope<LineComment>(lexeme, startIndex);
        }

    public:
        static const Token::Type TokenType = Token::Type::LINE_COMMENT;

    };

    struct BlockComment : public Token
    {
        BlockComment(const std::string& lexeme, uint32_t startIndex)
            : Token(Token::Type::BLOCK_COMMENT, lexeme, startIndex), open(lexeme == "/*")
        {
        }

        static Scope<Token> Create(const std::string& lexeme, uint32_t startIndex)
        {
            return CreateScope<BlockComment>(lexeme, startIndex);
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
        X(__clear, CLEAR) \
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

        Builtin(const std::string& lexeme, uint32_t startIndex)
            : Token(Token::Type::BUILTIN, lexeme, startIndex)
        {
#define X(keyword, name) if(lexeme == #keyword) { type = Type::name; }
            BUILTIN_TYPES
#undef X
        }

        static Scope<Token> Create(const std::string& lexeme, uint32_t startIndex)
        {
            if (builtinTypes.contains(lexeme))
            {
                return CreateScope<Builtin>(lexeme, startIndex);
            }

            return CreateScope<Token>(lexeme.length());
        }

    public:
        Type type;
        static const std::unordered_set<std::string> builtinTypes;

        static const Token::Type TokenType = Token::Type::BUILTIN;
    };
}