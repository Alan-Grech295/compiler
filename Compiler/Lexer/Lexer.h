#pragma once
#include <string>
#include <initializer_list>
#include <array>
#include <memory>

#include "../Utils/Table.h"
#include "Tokens.h"

using namespace Tokens;

class Lexer
{
public:
#define LEXEMES \
        X(HEX_LETTER) \
        X(LETTER) \
        X(DIGIT) \
        X(WHITESPACE) \
                    \
        X(EQUALS) \
        X(GREATER_THAN) \
        X(LESS_THAN) \
        X(EXCLAMATION) \
                    \
        X(PLUS) \
        X(DASH) \
        X(ASTERISK) \
        X(FORWARD_SLASH) \
                    \
        X(OPEN_PAREN) \
        X(CLOSE_PAREN) \
        X(OPEN_SQ_BRACK) \
        X(CLOSE_SQ_BRACK) \
        X(OPEN_CURLY_BRACK) \
        X(CLOSE_CURLY_BRACK) \
                        \
        X(UNDERSCORE) \
        X(COMMA) \
        X(FULLSTOP) \
        X(COLON) \
        X(SEMICOLON)  \
        X(HASHTAG)  \
                    \
        X(NEW_LINE) \


    enum class Lexeme
    {
#define X(name) name,
        LEXEMES
#undef X
        OTHER,
        // Used to keep track of the number of lexemes
        LAST = OTHER
    };

#define TOKEN_FINAL_STATE \
        X(IntegerLiteral, 1) \
        X(FloatLiteral, 3) \
        X(ColourLiteral, 33) \
        X(Identifier, 4) \
        X(AdditiveOp, 5) \
        X(AdditiveOp, 24) \
        X(MultiplicativeOp, 6) \
        X(MultiplicativeOp, 9) \
        X(LineComment, 7) \
        X(BlockComment, 8) \
        X(BlockComment, 10) \
        X(RelationalOp, 11) \
        X(RelationalOp, 14) \
        X(Assignment, 12) \
        X(Punctuation, 15) \
        X(Punctuation, 16) \
        X(Punctuation, 17) \
        X(Punctuation, 25) \
        X(Bracket, 18) \
        X(Bracket, 19) \
        X(Bracket, 20) \
        X(Bracket, 21) \
        X(Bracket, 22) \
        X(Bracket, 23) \
        X(Whitespace, 34) \
        X(Builtin, 37) \
        X(NewLine, 26) 

public:
    Lexer();

    Table<int>::RefCol operator[](int x)
    {
        return transitions[x];
    }

    Scope<Token> GetNextToken(const std::string& program, int& index, bool excludeWhitespace, bool excludeComments);

    Scope<Token> PeekNextToken(const std::string& program, int& index, bool excludeWhitespace, bool excludeComments);

    Scope<Token> GetNextToken(const std::string& program, int index);

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

    Scope<Token> GetTokenByFinalState(int state, const std::string& lexeme, uint32_t startIndex);

    Lexeme CatChar(char c);

private:
    const int NUM_STATES = 38;
    Table<int> transitions;
#define X(cls, state) + 1
    static const int NUM_FINAL_STATES = 0 TOKEN_FINAL_STATE;
#undef X
#define X(cls, state) state,
    std::array<int, NUM_FINAL_STATES> acceptedStates = { TOKEN_FINAL_STATE };
#undef X
};