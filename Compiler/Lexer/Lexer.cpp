#include "Lexer.h"
#include <cctype>

using namespace Tokens;

Lexer::Lexer()
    : transitions(Table<int>((int)Lexeme::LAST + 1, NUM_STATES, -1))
{
    InitTable();
}

std::unique_ptr<Token> Lexer::PeekNextToken(const std::string& program, int& index, bool excludeWhitespace, bool excludeComments)
{
    std::unique_ptr<Token> nextToken = GetNextToken(program, index);
    bool inLineComment = nextToken->type == Token::Type::LINE_COMMENT;
    bool inBlockComment = nextToken->type == Token::Type::BLOCK_COMMENT && nextToken->As<BlockComment>().open;

    while ((excludeWhitespace && (nextToken->type == Token::Type::WHITE_SPACE || nextToken->type == Token::Type::NEW_LINE)) ||
        (excludeComments && (nextToken->type == Token::Type::BLOCK_COMMENT || inLineComment || inBlockComment)))
    {
        index += nextToken->lexemeLength;
        nextToken = GetNextToken(program, index);

        if (inLineComment)
        {
            if(nextToken->type == Token::Type::NEW_LINE)
                inLineComment = false;
        }
        else
        {
            inLineComment = nextToken->type == Token::Type::LINE_COMMENT;
        }

        if (inBlockComment)
        {
            if (nextToken->type == Token::Type::BLOCK_COMMENT && !nextToken->As<BlockComment>().open)
                inBlockComment = false;
        }
        else
        {
            inBlockComment = nextToken->type == Token::Type::BLOCK_COMMENT && nextToken->As<BlockComment>().open;
        }
    }

    return std::move(nextToken);
}

std::unique_ptr<Token> Lexer::GetNextToken(const std::string& program, int& index, bool excludeWhitespace, bool excludeComments)
{
    auto token = PeekNextToken(program, index, excludeWhitespace, excludeComments);
    index += token->lexemeLength;
    return std::move(token);
}

std::unique_ptr<Token> Lexer::GetNextToken(const std::string& program, int index)
{
    if (index >= program.length())
        return std::move(std::make_unique<Token>(Token::Type::END_OF_FILE, " "));

    int state = 0;
    int lastAccState = -1;
    int lastIndex = -1;

    int i = index;

    for (;i < program.length() && state != -1; i++)
    {
        char currentChar = program[i];

        Lexer::Lexeme cat = CatChar(currentChar);
        state = transitions[state][(int)cat];

        if (Accepted(state))
        {
            lastAccState = state;
            lastIndex = i;
        }
    }

    if (lastAccState == -1)
    {
        return std::make_unique<Token>(i - index);
    }

    return std::move(GetTokenByFinalState(lastAccState, program.substr(index, lastIndex - index + 1)));
}

void Lexer::InitTable()
{
    #pragma region Integers
        transitions[0][(int)Lexeme::DIGIT] = 1;
        transitions[1][(int)Lexeme::DIGIT] = 1;
    #pragma endregion

    #pragma region Floats
        transitions[1][(int)Lexeme::FULLSTOP] = 2;
        transitions[2][(int)Lexeme::DIGIT] = 3;
        transitions[3][(int)Lexeme::DIGIT] = 3;
    #pragma endregion

    #pragma region Identifiers
        transitions[0][(int)Lexeme::LETTER] = 4;
        transitions[0][(int)Lexeme::HEX_LETTER] = 4;
        transitions[4][(int)Lexeme::LETTER] = 4;
        transitions[4][(int)Lexeme::HEX_LETTER] = 4;
        transitions[4][(int)Lexeme::DIGIT] = 4;
        transitions[4][(int)Lexeme::UNDERSCORE] = 4;
    #pragma endregion

    #pragma region Plus
        transitions[0][(int)Lexeme::PLUS] = 5;
    #pragma endregion

    #pragma region Minus
        transitions[0][(int)Lexeme::DASH] = 24;
    #pragma endregion

    #pragma region Arrow
        transitions[24][(int)Lexeme::GREATER_THAN] = 25;
    #pragma endregion

    #pragma region Multiplication
        transitions[0][(int)Lexeme::ASTERISK] = 9;
    #pragma endregion

    #pragma region Close Block Comment
        transitions[9][(int)Lexeme::FORWARD_SLASH] = 10;
    #pragma endregion

    #pragma region Division
        transitions[0][(int)Lexeme::FORWARD_SLASH] = 6;
        transitions[6][(int)Lexeme::FORWARD_SLASH] = 7;
        transitions[6][(int)Lexeme::ASTERISK] = 8;
    #pragma endregion

    #pragma region Relational
        transitions[0][(int)Lexeme::GREATER_THAN] = 11;
        transitions[0][(int)Lexeme::LESS_THAN] = 11;
        transitions[0][(int)Lexeme::EQUALS] = 12;
        transitions[0][(int)Lexeme::EXCLAMATION] = 13;
        transitions[11][(int)Lexeme::EQUALS] = 14;
        transitions[12][(int)Lexeme::EQUALS] = 14;
        transitions[13][(int)Lexeme::EQUALS] = 14;
    #pragma endregion

    #pragma region Punctuation
        transitions[0][(int)Lexeme::COMMA] = 15;
        transitions[0][(int)Lexeme::COLON] = 16;
        transitions[0][(int)Lexeme::SEMICOLON] = 17;
    #pragma endregion

    #pragma region Brackets
        transitions[0][(int)Lexeme::OPEN_PAREN] = 18;
        transitions[0][(int)Lexeme::CLOSE_PAREN] = 19;
        transitions[0][(int)Lexeme::OPEN_SQ_BRACK] = 20;
        transitions[0][(int)Lexeme::CLOSE_SQ_BRACK] = 21;
        transitions[0][(int)Lexeme::OPEN_CURLY_BRACK] = 22;
        transitions[0][(int)Lexeme::CLOSE_CURLY_BRACK] = 23;
    #pragma endregion

    #pragma region New Line
        transitions[0][(int)Lexeme::NEW_LINE] = 26;
    #pragma endregion

    #pragma region White space
        transitions[0][(int)Lexeme::WHITESPACE] = 34;
        transitions[34][(int)Lexeme::WHITESPACE] = 34;
    #pragma endregion

    #pragma region Colour Literal
        transitions[0][(int)Lexeme::HASHTAG] = 27;

        transitions[27][(int)Lexeme::HEX_LETTER] = 28;
        transitions[27][(int)Lexeme::DIGIT] = 28;

        transitions[28][(int)Lexeme::HEX_LETTER] = 29;
        transitions[28][(int)Lexeme::DIGIT] = 29;

        transitions[29][(int)Lexeme::HEX_LETTER] = 30;
        transitions[29][(int)Lexeme::DIGIT] = 30;

        transitions[30][(int)Lexeme::HEX_LETTER] = 31;
        transitions[30][(int)Lexeme::DIGIT] = 31;

        transitions[31][(int)Lexeme::HEX_LETTER] = 32;
        transitions[31][(int)Lexeme::DIGIT] = 32;

        transitions[32][(int)Lexeme::HEX_LETTER] = 33;
        transitions[32][(int)Lexeme::DIGIT] = 33;
    #pragma endregion

    #pragma region Builtin
        transitions[0][(int)Lexeme::UNDERSCORE] = 35;
        transitions[35][(int)Lexeme::UNDERSCORE] = 36;

        transitions[36][(int)Lexeme::UNDERSCORE] = 37;
        transitions[36][(int)Lexeme::HEX_LETTER] = 37;
        transitions[36][(int)Lexeme::LETTER] = 37;

        transitions[37][(int)Lexeme::UNDERSCORE] = 37;
        transitions[37][(int)Lexeme::HEX_LETTER] = 37;
        transitions[37][(int)Lexeme::LETTER] = 37;
    #pragma endregion

}

std::unique_ptr<Token> Lexer::GetTokenByFinalState(int state, const std::string& lexeme)
{
    switch (state)
    {
#define X(cls, state) case state: return std::move(cls::Create(lexeme)); 
        TOKEN_FINAL_STATE
#undef X
    }

    return std::move(std::make_unique<Token>(lexeme.length()));
}

Lexer::Lexeme Lexer::CatChar(char c)
{
    if (std::isalpha(c))
    {
        char lowerC = std::tolower(c);
        if (lowerC >= 'a' && lowerC <= 'f')
            return Lexeme::HEX_LETTER;
        return Lexeme::LETTER;
    }
    else if (std::isdigit(c))
    {
        return Lexeme::DIGIT;
    }
    else if (c != '\n' && std::isspace(c))
    {
        return Lexeme::WHITESPACE;
    }

    switch (c)
    {
    case '=':
        return Lexeme::EQUALS;
    case '>':
        return Lexeme::GREATER_THAN;
    case '<':
        return Lexeme::LESS_THAN;
    case '!':
        return Lexeme::EXCLAMATION;
    case '+':
        return Lexeme::PLUS;
    case '-':
        return Lexeme::DASH;
    case '*':
        return Lexeme::ASTERISK;
    case '/':
        return Lexeme::FORWARD_SLASH;
    case '(':
        return Lexeme::OPEN_PAREN;
    case ')':
        return Lexeme::CLOSE_PAREN;
    case '[':
        return Lexeme::OPEN_SQ_BRACK;
    case ']':
        return Lexeme::CLOSE_SQ_BRACK;
    case '{':
        return Lexeme::OPEN_CURLY_BRACK;
    case '}':
        return Lexeme::CLOSE_CURLY_BRACK;
    case '_':
        return Lexeme::UNDERSCORE;
    case ',':
        return Lexeme::COMMA;
    case '.':
        return Lexeme::FULLSTOP;
    case ':':
        return Lexeme::COLON;
    case ';':
        return Lexeme::SEMICOLON;
    case '\n':
        return Lexeme::NEW_LINE;
    case '#':
        return Lexeme::HASHTAG;
    }

    return Lexeme::OTHER;
}

