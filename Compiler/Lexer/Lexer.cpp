#include "Lexer.h"
#include <cctype>

using namespace Tokens;

Lexer::Lexer()
    : transitions(Table<int>((int)Lexeme::LAST + 1, NUM_STATES, -1))
{
    InitTable();
}

Tokens::Token Lexer::GetNextToken(const std::string& program, int index)
{
    int state = 0;
    int lastAccState = -1;
    int lastIndex = -1;

    for (int i = index; i < program.length() && state != -1; i++)
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
        return Token(Token::Type::ERROR, "error");
    }

    return GetTokenByFinalState(lastAccState, program.substr(index, lastIndex - index + 1));
}

void Lexer::InitTable()
{
    // Identifiers
    transitions[0][(int)Lexeme::LETTER] = 1;
    transitions[0][(int)Lexeme::UNDERSCORE] = 1;
    transitions[1][(int)Lexeme::LETTER] = 1;
    transitions[1][(int)Lexeme::DIGIT] = 1;

    // Whitespace
    transitions[0][(int)Lexeme::WHITESPACE] = 2;
    transitions[2][(int)Lexeme::WHITESPACE] = 2;

    // Equals Sign
    transitions[0][(int)Lexeme::EQUALS] = 3;

    // Integers
    transitions[0][(int)Lexeme::DIGIT] = 4;
    transitions[4][(int)Lexeme::DIGIT] = 4;

    // Semicolon
    transitions[0][(int)Lexeme::SEMICOLON] = 5;
}

Token Lexer::GetTokenByFinalState(int state, const std::string& lexeme)
{
    switch (state)
    {
#define X(cls, state) case state: return cls(lexeme); 
        TOKEN_FINAL_STATE
#undef X
    }

    return Tokens::Token();
}

Lexer::Lexeme Lexer::CatChar(char c)
{
    if (std::isalpha(c))
        return Lexeme::LETTER;
    else if (std::isdigit(c))
        return Lexeme::DIGIT;
    else if (std::isspace(c))
        return Lexeme::WHITESPACE;

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
    }

    return Lexeme::OTHER;
}

