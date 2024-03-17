#include <iostream>
#include <vector>
#include <fstream>

#include "Lexer/Lexer.h"


int main()
{
    Lexer lexer{};

    std::ifstream file;
    std::string line;
    file.open("src/example - full.txt");
    std::string fileText;
    if (file.is_open())
    {
        while (std::getline(file, line))
        {
            fileText += line + "\n";
        }
        file.close();
    }

    std::cout << fileText << std::endl;

    int programIndex = 0;
    std::vector<std::unique_ptr<Tokens::Token>> tokens{};

    while (programIndex < fileText.length())
    {
        auto token = lexer.GetNextToken(fileText, programIndex, true, true);
        programIndex += token->lexemeLength;
        tokens.push_back(std::move(token));
    }

    int a = 0;
}