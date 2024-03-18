#include <iostream>
#include <vector>
#include <fstream>

#include "Lexer/Lexer.h"
#include "Parser/Parser.h"


int main()
{
    Lexer lexer{};

    std::ifstream file;
    std::string line;
    file.open("src/example.txt");
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

    Parser parser{ };
    Scope<ASTProgramNode> programAST;
    try
    {
        programAST = parser.Parse(fileText);
    }
    catch (Parser::SyntaxErrorException e)
    {
        std::cout << e.what() << std::endl;
    }

    int a = 0;
}