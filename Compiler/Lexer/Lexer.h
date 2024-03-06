#pragma once
#include <string>
#include "../Utils/Table.h"
#include <initializer_list>
#include <array>

class Lexer
{
public:
#define LEXEMES \
        X(UNDERSCORE) \
        X(LETTER) \
        X(DIGIT) \
        X(WHITESPACE) \
        X(EQUAL) \
        X(SEMICOLON) 

    enum class Lexeme
    {
#define X(name) name,
        LEXEMES
#undef X
        OTHER,
        // Used to keep track of the number of lexemes
        LAST = OTHER
    };

    struct Token
    {
        // Token Name, Final State
#define TOKENS \
        X(IDENTIFIER, 1) \
        X(INTEGER, 4) \
        X(WHITESPACE, 2) \
        X(EQUALS, 3) \
        X(SEMICOLON, 5) 

        enum class Type
        {
            ERROR = 0,
#define X(name, state) name,
            TOKENS
#undef X
        };

        Token();
        Token(Type type, const std::string& lexeme);

    public:
        Type type;
        std::string lexeme;
    };

public:
    Lexer();

    Table<int>::RefCol operator[](int x)
    {
        return transitions[x];
    }

    Token GetNextToken(const std::string& program, int index);

private:
    void InitTable();

    inline bool Accepted(int state)
    {
        for (int a : acceptedStates)
        {
            if (a == state)
                return true;
        }

        return false;
    }

    Token GetTokenByFinalState(int state, const std::string& lexeme);

    Lexeme CatChar(char c);

private:
    const int NUM_STATES = 6;
    Table<int> transitions;
    std::array<int, 5> acceptedStates = {
#define X(name, state) state,
        TOKENS
#undef X
    };
};