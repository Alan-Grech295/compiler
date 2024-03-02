#include <iostream>
#include <vector>
#include "Lexer/Lexer.h"

int main()
{
	Lexer lexer{};

	std::string program = "_xyz=23; y= 100;   z=4 ;";

	int programIndex = 0;
	std::vector<Lexer::Token> tokens{};

	while (programIndex < program.length())
	{
		auto token = lexer.GetNextToken(program, programIndex);
		tokens.push_back(token);
		programIndex += token.lexeme.length();
	}
}