#include "Lexer.h"
#include <cctype>

Lexer::Token::Token()
    : type(Type::ERROR), lexeme(std::string())
{
}

Lexer::Token::Token(Type type, const std::string& lexeme)
    : type(type), lexeme(lexeme)
{
}

Lexer::Lexer()
    : transitions(Table<int>((int)Lexeme::LAST + 1, NUM_STATES, -1))
{
    InitTable();
}

Lexer::Token Lexer::GetNextToken(const std::string& program, int index)
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
    transitions[0][(int)Lexeme::EQUAL] = 3;

    // Integers
    transitions[0][(int)Lexeme::DIGIT] = 4;
    transitions[4][(int)Lexeme::DIGIT] = 4;

    // Semicolon
    transitions[0][(int)Lexeme::SEMICOLON] = 5;
}

Lexer::Token Lexer::GetTokenByFinalState(int state, const std::string& lexeme)
{
    switch (state)
    {
        case 1:
            return Token(Token::Type::IDENTIFIER, lexeme);
        case 2:
            return Token(Token::Type::WHITESPACE, lexeme);
        case 3:
            return Token(Token::Type::EQUALS, lexeme);
        case 4:
            return Token(Token::Type::INTEGER, lexeme);
        case 5:
            return Token(Token::Type::SEMICOLON, lexeme);
    }

    return Token();
}

Lexer::Lexeme Lexer::CatChar(char c)
{
    if (std::isalpha(c))
        return Lexeme::LETTER;
    else if (std::isdigit(c))
        return Lexeme::DIGIT;
    else if (c == '_')
        return Lexeme::UNDERSCORE;
    else if (std::isspace(c))
        return Lexeme::WHITESPACE;
    else if (c == ';')
        return Lexeme::SEMICOLON;
    else if (c == '=')
        return Lexeme::EQUAL;

    return Lexeme::OTHER;
}
