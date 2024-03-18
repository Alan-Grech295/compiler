#include "Parser.h"

using namespace Tokens;

Parser::Parser()
{
}

Scope<ASTProgramNode> Parser::Parse(const std::string& program)
{
    programIndex = 0;
    this->program = program;
    return CreateScope<ASTProgramNode>(std::move(ParseBlock(true)));
}

Scope<ASTBlockNode> Parser::ParseBlock(bool root)
{
    auto blockNode = CreateScope<ASTBlockNode>();

    auto token = PeekNextToken();

    if (!root)
    {
        ASSERT(CHECK_SUB_TYPE(token, Bracket, type == Bracket::Type::OPEN_CURLY_BRACK));
        JumpToken(token->lexemeLength);
        token = PeekNextToken();
    }

    while ((root && token->type != Token::Type::END_OF_FILE) || (!root && !CHECK_SUB_TYPE(token, Bracket, type == Bracket::Type::CLOSE_CURLY_BRACK)))
    {
        blockNode->AddStatement(std::move(ParseStatement()));
        token = PeekNextToken();
    }

    JumpToken(token->lexemeLength);

    return std::move(blockNode);
}

Scope<ASTNode> Parser::ParseStatement()
{
    auto token = PeekNextToken();

    switch (token->type)
    {
        case Token::Type::KEYWORD:
        {
            Keyword& keywordToken = token->As<Keyword>();
            switch (keywordToken.type)
            {
                case Keyword::Type::LET:
                {
                    auto variable = ParseVariableDeclaration();
                    token = GetNextToken();
                    ASSERT(CHECK_SUB_TYPE(token, Punctuation, type == Punctuation::Type::SEMICOLON));
                    return std::move(variable);
                }

                case Keyword::Type::IF:
                {
                    return std::move(ParseIfStatement());
                }

                case Keyword::Type::RETURN:
                {
                    auto retStatement = ParseReturnStatement();
                    token = GetNextToken();
                    ASSERT(CHECK_SUB_TYPE(token, Punctuation, type == Punctuation::Type::SEMICOLON));
                    return std::move(retStatement);
                }

                case Keyword::Type::FUN:
                {
                    return std::move(ParseFunctionDecl());
                }
            }
            break;
        }

        case Token::Type::IDENTIFIER:
            auto assignment = ParseAssignment();
            token = GetNextToken();
            ASSERT(CHECK_SUB_TYPE(token, Punctuation, type == Punctuation::Type::SEMICOLON));
            return std::move(assignment);
    }


    return Scope<ASTNode>();
}

Scope<ASTVarDeclNode> Parser::ParseVariableDeclaration()
{
    auto nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Keyword, type == Keyword::Type::LET));

    nextToken = GetNextToken();
    ASSERT(nextToken->type == Token::Type::IDENTIFIER);

    std::string identifierName = nextToken->As<Identifier>().name;

    nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Punctuation, type == Punctuation::Type::COLON));

    nextToken = GetNextToken();
    ASSERT(nextToken->type == Token::Type::VAR_TYPE);

    Scope<ASTIdentifierNode> identifier = CreateScope<ASTIdentifierNode>(identifierName, nextToken->As<VarType>().type);

    nextToken = GetNextToken();
    ASSERT(nextToken->type == Token::Type::ASSIGNMENT);

    Scope<ASTExpressionNode> expression = ParseExpression();

    return CreateScope<ASTVarDeclNode>(std::move(identifier), std::move(expression));
}

Scope<ASTAssignmentNode> Parser::ParseAssignment()
{
    auto nextToken = GetNextToken();
    ASSERT(nextToken->type == Token::Type::IDENTIFIER);

    auto identifier = CreateScope<ASTIdentifierNode>(nextToken->As<Identifier>().name);

    nextToken = GetNextToken();
    ASSERT(nextToken->type == Token::Type::ASSIGNMENT);

    auto expr = ParseExpression();

    return CreateScope<ASTAssignmentNode>(std::move(identifier), std::move(expr));
}

Scope<ASTExpressionNode> Parser::ParseExpression(bool subExpr)
{
    auto curExpr = ParseSimpleExpression();

    auto nextToken = PeekNextToken();
    while (nextToken->type == Token::Type::REL_OP)
    {
        JumpToken(nextToken->lexemeLength);
        RelationalOp::Type relType = nextToken->As<RelationalOp>().type;
        auto nextExpr = ParseSimpleExpression();
        curExpr = CreateScope<ASTBinaryOpNode>(relType, std::move(curExpr), std::move(nextExpr));
        nextToken = PeekNextToken();
    }

    ASSERT(!(subExpr ^ CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::CLOSE_PAREN)));
    if (subExpr)
        JumpToken(nextToken->lexemeLength);

    return curExpr;
}

Scope<ASTExpressionNode> Parser::ParseSimpleExpression()
{
    auto curTerm = ParseTerm();

    auto nextToken = PeekNextToken();
    while (nextToken->type == Token::Type::ADD_OP)
    {
        JumpToken(nextToken->lexemeLength);
        AdditiveOp::Type additiveType = nextToken->As<AdditiveOp>().type;
        auto nextTerm = ParseTerm();
        curTerm = CreateScope<ASTBinaryOpNode>(additiveType, std::move(curTerm), std::move(nextTerm));
        nextToken = PeekNextToken();
    }

    return curTerm;
}

Scope<ASTExpressionNode> Parser::ParseTerm()
{
    auto curFactor = ParseFactor();

    auto nextToken = PeekNextToken();
    while (nextToken->type == Token::Type::MULT_OP)
    {
        JumpToken(nextToken->lexemeLength);
        MultiplicativeOp::Type multType = nextToken->As<MultiplicativeOp>().type;
        auto nextFactor = ParseFactor();
        curFactor = CreateScope<ASTBinaryOpNode>(multType, std::move(curFactor), std::move(nextFactor));
        nextToken = PeekNextToken();
    }

    return curFactor;
}

Scope<ASTExpressionNode> Parser::ParseFactor()
{
    auto nextToken = GetNextToken();
    switch (nextToken->type)
    {
        // Literals
    case Token::Type::INT_LITERAL:
        return CreateScope<ASTIntLiteralNode>(nextToken->As<IntegerLiteral>().value);
    case Token::Type::FLOAT_LITERAL:
        return CreateScope<ASTFloatLiteralNode>(nextToken->As<FloatLiteral>().value);
    case Token::Type::BOOLEAN_LITERAL:
        return CreateScope<ASTBooleanLiteralNode>(nextToken->As<BooleanLiteral>().value);
    case Token::Type::COLOUR_LITERAL:
        return CreateScope<ASTColourLiteralNode>(nextToken->As<ColourLiteral>().value);

        // Identifier
    case Token::Type::IDENTIFIER:
    {
        Identifier& identifierNode = nextToken->As<Identifier>();
        return CreateScope<ASTIdentifierNode>(identifierNode.name);
    }

        // Sub expression
    case Token::Type::BRACKET:
        if (nextToken->As<Bracket>().type == Bracket::Type::OPEN_PAREN)
        {
            return ParseExpression(true);
        }
        break;

        // Unary Operators
    case Token::Type::UNARY_OP:
        return CreateScope<ASTNotNode>(std::move(ParseExpression()));

    case Token::Type::ADD_OP:
        if (nextToken->As<AdditiveOp>().type == AdditiveOp::Type::SUBTRACT)
        {
            return CreateScope<ASTNegateNode>(std::move(ParseExpression()));
        }
        break;
    }

    throw SyntaxErrorException(program, programIndex);
}

Scope<ASTReturnNode> Parser::ParseReturnStatement()
{
    auto nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Keyword, type == Keyword::Type::RETURN));

    auto expr = ParseExpression();

    return CreateScope<ASTReturnNode>(std::move(expr));
}

Scope<ASTFunctionNode> Parser::ParseFunctionDecl()
{
    auto nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Keyword, type == Keyword::Type::FUN));

    nextToken = GetNextToken();
    ASSERT(nextToken->type == Token::Type::IDENTIFIER);

    std::string funName = nextToken->As<Identifier>().name;

    nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::OPEN_PAREN));

    std::vector<ASTFunctionNode::Param> params;

    nextToken = PeekNextToken();
    while (!CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::CLOSE_PAREN))
    {
        params.push_back(ParseParam());
        nextToken = GetNextToken();
        ASSERT(CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::CLOSE_PAREN) || 
               CHECK_SUB_TYPE(nextToken, Punctuation, type == Punctuation::Type::COMMA));
    }

    JumpToken(nextToken->lexemeLength);

    nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Punctuation, type == Punctuation::Type::ARROW));

    nextToken = GetNextToken();
    ASSERT(nextToken->type == Token::Type::VAR_TYPE);

    auto retType = nextToken->As<VarType>().type;

    auto blockNode = ParseBlock();

    return CreateScope<ASTFunctionNode>(funName, params, retType, std::move(blockNode));
}

ASTFunctionNode::Param Parser::ParseParam()
{
    ASTFunctionNode::Param param{};

    auto nextToken = GetNextToken();
    ASSERT(nextToken->type == Token::Type::IDENTIFIER);
    param.Name = nextToken->As<Identifier>().name;

    nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Punctuation, type == Punctuation::Type::COLON));

    nextToken = GetNextToken();
    ASSERT(nextToken->type == Token::Type::VAR_TYPE);

    param.Type = nextToken->As<VarType>().type;

    return param;
}

Scope<ASTDecisionNode> Parser::ParseIfStatement()
{
    auto nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Keyword, type == Keyword::Type::IF));

    nextToken = PeekNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::OPEN_PAREN));

    auto expr = ParseExpression();

    auto trueBlock = ParseBlock();

    Scope<ASTBlockNode> falseBlock = nullptr;

    nextToken = PeekNextToken();

    if (CHECK_SUB_TYPE(nextToken, Keyword, type == Keyword::Type::ELSE))
    {
        JumpToken(nextToken->lexemeLength);
        falseBlock = ParseBlock();
    }

    return CreateScope<ASTDecisionNode>(std::move(expr), std::move(trueBlock), std::move(falseBlock));
}
