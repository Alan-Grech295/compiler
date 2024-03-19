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
    std::string fileTextWithLines;
    if (file.is_open())
    {
        int lineNo = 1;
        while (std::getline(file, line))
        {
            fileText += line + "\n";
            fileTextWithLines += std::to_string(lineNo) + ". " + line + "\n";
            lineNo++;
        }
        file.close();
    }

    std::cout << fileTextWithLines << std::endl;

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