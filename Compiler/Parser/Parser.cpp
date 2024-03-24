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

                case Keyword::Type::WHILE:
                {
                    return std::move(ParseWhileLoop());
                }

                case Keyword::Type::FOR:
                {
                    return std::move(ParseForLoop());
                }
            }
            break;
        }

        case Token::Type::IDENTIFIER:
        {
            auto assignment = ParseAssignment();
            token = GetNextToken();
            ASSERT(CHECK_SUB_TYPE(token, Punctuation, type == Punctuation::Type::SEMICOLON));
            return std::move(assignment);
        }

        case Token::Type::BUILTIN:
        {
            Builtin& builtinToken = token->As<Builtin>();
            switch (builtinToken.type)
            {
                case Builtin::Type::PRINT:
                {
                    auto print = ParsePrint();
                    token = GetNextToken();
                    ASSERT(CHECK_SUB_TYPE(token, Punctuation, type == Punctuation::Type::SEMICOLON));
                    return std::move(print);
                }

                case Builtin::Type::DELAY:
                {
                    auto delay = ParseDelay();
                    token = GetNextToken();
                    ASSERT(CHECK_SUB_TYPE(token, Punctuation, type == Punctuation::Type::SEMICOLON));
                    return std::move(delay);
                }

                case Builtin::Type::WRITE:
                {
                    auto write = ParseWrite();
                    token = GetNextToken();
                    ASSERT(CHECK_SUB_TYPE(token, Punctuation, type == Punctuation::Type::SEMICOLON));
                    return std::move(write);
                }

                case Builtin::Type::WRITE_BOX:
                {
                    auto writeBox = ParseWriteBox();
                    token = GetNextToken();
                    ASSERT(CHECK_SUB_TYPE(token, Punctuation, type == Punctuation::Type::SEMICOLON));
                    return std::move(writeBox);
                }
            }
        }
    }


    throw SyntaxErrorException(program, programIndex, __LINE__);
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

    auto varType = nextToken->As<VarType>().type;

    Scope<ASTExpressionNode> expression;
    int arraySize = -1;

    nextToken = GetNextToken();
    ASSERT(nextToken->type == Token::Type::ASSIGNMENT || CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::OPEN_SQ_BRACK));
    if(nextToken->type == Token::Type::BRACKET)
    {
        nextToken = GetNextToken();
        ASSERT(nextToken->type == Token::Type::INT_LITERAL || CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::CLOSE_SQ_BRACK));
        if (nextToken->type == Token::Type::INT_LITERAL)
        {
            arraySize = nextToken->As<IntegerLiteral>().value;
            nextToken = GetNextToken();
            ASSERT(CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::CLOSE_SQ_BRACK));
        }

        nextToken = GetNextToken();
        ASSERT(nextToken->type == Token::Type::ASSIGNMENT);

        nextToken = GetNextToken();
        ASSERT(CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::OPEN_SQ_BRACK));

        Scope<ASTArraySetNode> arraySetNode;

        if (arraySize == -1)
        {
            arraySetNode = CreateScope<ASTArraySetNode>();
            arraySize = 0;
            while (!CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::CLOSE_SQ_BRACK))
            {
                arraySetNode->AddLiterial(std::move(ParseLiteral()));
                arraySize++;
                nextToken = GetNextToken();
                ASSERT(CHECK_SUB_TYPE(nextToken, Punctuation, type == Punctuation::Type::COMMA) ||
                       CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::CLOSE_SQ_BRACK));
            }
        }
        else
        {
            arraySetNode = CreateScope<ASTArraySetNode>(std::move(ParseLiteral()), arraySize);
            nextToken = GetNextToken();
            ASSERT(CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::CLOSE_SQ_BRACK));
        }

        expression = std::move(arraySetNode);
    }
    else
    {
        expression = ParseExpression(); 
    }

    Scope<ASTIdentifierNode> identifier = CreateScope<ASTIdentifierNode>(identifierName, varType, arraySize);

    return CreateScope<ASTVarDeclNode>(std::move(identifier), std::move(expression));
}

Scope<ASTAssignmentNode> Parser::ParseAssignment()
{
    auto identifier = ParseIdentifier();

    auto nextToken = GetNextToken();
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

    if (CHECK_SUB_TYPE(nextToken, Keyword, type == Keyword::Type::AS))
    {
        JumpToken(nextToken->lexemeLength);
        nextToken = GetNextToken();
        ASSERT(nextToken->type == Token::Type::VAR_TYPE);
        curExpr = CreateScope<ASTCastNode>(nextToken->As<VarType>().type, std::move(curExpr));
        nextToken = PeekNextToken();
    }

    ASSERT(!subExpr || (subExpr && CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::CLOSE_PAREN)));
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
    case Token::Type::FLOAT_LITERAL:
    case Token::Type::BOOLEAN_LITERAL:
    case Token::Type::COLOUR_LITERAL:
        UndoToken(nextToken->startIndex);
        return ParseLiteral();

    case Token::Type::BUILTIN:
    {
        UndoToken(nextToken->startIndex);
        switch (nextToken->As<Builtin>().type)
        {
        case Builtin::Type::WIDTH:
        case Builtin::Type::HEIGHT:
        case Builtin::Type::READ:
            return ParseLiteral();
        case Builtin::Type::RANDOM_INT:
            return ParseRandInt();
        }
        break;
    }

        // Identifier
    case Token::Type::IDENTIFIER:
    {
        auto after = PeekNextToken();
        UndoToken(nextToken->startIndex);
        if (CHECK_SUB_TYPE(after, Bracket, type == Bracket::Type::OPEN_PAREN))
        {
            return ParseFunctionCall();
        }
        else
        {
            return ParseIdentifier();
        }
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

    throw SyntaxErrorException(program, programIndex, __LINE__);
}

Scope<ASTExpressionNode> Parser::ParseLiteral()
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

    case Token::Type::BUILTIN:
    {
        UndoToken(nextToken->startIndex);
        switch (nextToken->As<Builtin>().type)
        {
        case Builtin::Type::WIDTH:
            return ParseWidth();
        case Builtin::Type::HEIGHT:
            return ParseHeight();
        case Builtin::Type::READ:
            return ParseRead();
        }
    }
    }

    throw SyntaxErrorException(program, programIndex, __LINE__);
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

Scope<ASTIdentifierNode> Parser::ParseIdentifier()
{
    auto nextToken = GetNextToken();
    ASSERT(nextToken->type == Token::Type::IDENTIFIER);
    std::string identifierName = nextToken->As<Identifier>().name;

    nextToken = PeekNextToken();
    
    if (CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::OPEN_SQ_BRACK))
    {
        JumpToken(nextToken->lexemeLength);
        auto expr = ParseExpression();
        nextToken = GetNextToken();
        ASSERT(CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::CLOSE_SQ_BRACK));
        return CreateScope<ASTArrayIndexNode>(identifierName, std::move(expr));
    }

    return CreateScope<ASTIdentifierNode>(identifierName);
}

Scope<ASTWhileNode> Parser::ParseWhileLoop()
{
    auto nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Keyword, type == Keyword::Type::WHILE));

    nextToken = PeekNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::OPEN_PAREN));

    auto expr = ParseExpression();

    auto blockNode = ParseBlock();

    return CreateScope<ASTWhileNode>(std::move(expr), std::move(blockNode));
}

Scope<ASTForNode> Parser::ParseForLoop()
{
    auto nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Keyword, type == Keyword::Type::FOR));

    nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::OPEN_PAREN));

    nextToken = PeekNextToken();
    Scope<ASTVarDeclNode> varDecl = nullptr;
    if (!CHECK_SUB_TYPE(nextToken, Punctuation, type == Punctuation::Type::SEMICOLON))
    {
        varDecl = ParseVariableDeclaration();
    }

    nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Punctuation, type == Punctuation::Type::SEMICOLON));

    Scope<ASTExpressionNode> expr = ParseExpression();

    nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Punctuation, type == Punctuation::Type::SEMICOLON));

    nextToken = PeekNextToken();
    Scope<ASTAssignmentNode> assignment = nullptr;
    if (!CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::CLOSE_PAREN))
    {
        assignment = ParseAssignment();
    }

    nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::CLOSE_PAREN));

    auto blockNode = ParseBlock();

    return CreateScope<ASTForNode>(std::move(varDecl), std::move(expr), std::move(assignment), std::move(blockNode));
}

Scope<ASTWidthNode> Parser::ParseWidth()
{
    auto nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Builtin, type == Builtin::Type::WIDTH));

    return CreateScope<ASTWidthNode>();
}

Scope<ASTHeightNode> Parser::ParseHeight()
{
    auto nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Builtin, type == Builtin::Type::HEIGHT));

    return CreateScope<ASTHeightNode>();
}

Scope<ASTReadNode> Parser::ParseRead()
{
    auto nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Builtin, type == Builtin::Type::READ));

    auto x = ParseExpression();

    nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Punctuation, type == Punctuation::Type::COMMA));

    auto y = ParseExpression();

    return CreateScope<ASTReadNode>(std::move(x), std::move(y));
}

Scope<ASTRandIntNode> Parser::ParseRandInt()
{
    auto nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Builtin, type == Builtin::Type::RANDOM_INT));

    auto max = ParseExpression();

    return CreateScope<ASTRandIntNode>(std::move(max));
}

Scope<ASTPrintNode> Parser::ParsePrint()
{
    auto nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Builtin, type == Builtin::Type::PRINT));

    auto expr = ParseExpression();

    return CreateScope<ASTPrintNode>(std::move(expr));
}

Scope<ASTDelayNode> Parser::ParseDelay()
{
    auto nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Builtin, type == Builtin::Type::DELAY));

    auto expr = ParseExpression();

    return CreateScope<ASTDelayNode>(std::move(expr));
}

Scope<ASTWriteNode> Parser::ParseWrite()
{
    auto nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Builtin, type == Builtin::Type::WRITE));

    auto x = ParseExpression();

    nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Punctuation, type == Punctuation::Type::COMMA));

    auto y = ParseExpression();

    nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Punctuation, type == Punctuation::Type::COMMA));

    auto colour = ParseExpression();

    return CreateScope<ASTWriteNode>(std::move(x), std::move(y), std::move(colour));
}

Scope<ASTWriteBoxNode> Parser::ParseWriteBox()
{
    auto nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Builtin, type == Builtin::Type::WRITE_BOX));

    auto x = ParseExpression();

    nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Punctuation, type == Punctuation::Type::COMMA));

    auto y = ParseExpression();

    nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Punctuation, type == Punctuation::Type::COMMA));

    auto w = ParseExpression();

    nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Punctuation, type == Punctuation::Type::COMMA));

    auto h = ParseExpression();

    nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Punctuation, type == Punctuation::Type::COMMA));

    auto colour = ParseExpression();

    return CreateScope<ASTWriteBoxNode>(std::move(x), std::move(y), std::move(w), std::move(h), std::move(colour));
}

Scope<ASTFuncCallNode> Parser::ParseFunctionCall()
{
    auto nextToken = GetNextToken();
    ASSERT(nextToken->type == Token::Type::IDENTIFIER);

    Scope<ASTFuncCallNode> funcCall = CreateScope<ASTFuncCallNode>(nextToken->As<Identifier>().name);

    nextToken = GetNextToken();
    ASSERT(CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::OPEN_PAREN));

    nextToken = PeekNextToken();
    while (!CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::CLOSE_PAREN))
    {
        funcCall->AddArg(std::move(ParseExpression()));
        nextToken = GetNextToken();
        ASSERT(CHECK_SUB_TYPE(nextToken, Bracket, type == Bracket::Type::CLOSE_PAREN) ||
               CHECK_SUB_TYPE(nextToken, Punctuation, type == Punctuation::Type::COMMA));
    }

    return funcCall;
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
