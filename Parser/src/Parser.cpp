#include <set>
#include <iostream>
#include "Token.h"
#include "Node.h"

using std::set;
using std::cout;

static vector<Token>::iterator current;

static Function* parseFunction();
static vector<Statement*> parseBlock();
static Variable* parseVariable();
static For* parseFor();
static If* parseIf();
static Print* parsePrint();
static Return* parseReturn();
static Break* parseBreak();
static Continue* parseContinue();
static ExpressionStatement* parseExpressionStatement();
static Expression* parseExpression();
static Expression* parseAssignment();
static Expression* parseOr();
static Expression* parseAnd();
static Expression* parseRelational();
static Expression* parseArithmetic1();
static Expression* parseArithmetic2();
static Expression* parseUnary();
static Expression* parseOperand();
static Expression* parseNullLiteral();
static Expression* parseBooleanLiteral();
static Expression* parseNumberLiteral();
static Expression* parseStringLiteral();
static Expression* parseListLiteral();
static Expression* parseMapLiteral();
static Expression* parseIdentifier();
static Expression* parseInnerExpression();
static Expression* parsePostfix(Expression*);
static Expression* parseCall(Expression*);
static Expression* parseElement(Expression*);
static void skipCurrent();
static void skipCurrent(Kind);
static bool skipCurrentIf(Kind);

Program* parse(vector<Token> tokens) {
    Program* result = new Program();
    current = tokens.begin();
    while(current->kind != Kind::EndOfTokenList) {
        switch(current->kind) {
            case Kind::Function: {
                result->functions.push_back(parseFunction());
                break;
            }
            default:
                cout << *current << "잘못된 구문입니다.";
                exit(1);
        }
    }
    return result;
}

Function* parseFunction() {
    Function* result = new Function();
    skipCurrent(Kind::Function);
    result->name = current->str;
    skipCurrent(Kind::Identifier);
    skipCurrent(Kind::LeftParen);
    if(current->kind != Kind::RightParen) {
        do {
            result->parameters.push_back(current->str);
            skipCurrent(Kind::Identifier);
        } while(skipCurrentIf(Kind::Comma));
    }
    skipCurrent(Kind::RightParen);
    skipCurrent(Kind::LeftBrace);
    result->block = parseBlock();
    skipCurrent(Kind::RightBrace);
    return result;
}

vector<Statement*> parseBlock() {
    vector<Statement*> result;
    while(current->kind != Kind::RightBrace) {
        switch(current->kind) {
        default:                result.push_back(parseExpressionStatement()); break;
        case Kind::Variable:    result.push_back(parseVariable());            break;
        case Kind::For:         result.push_back(parseFor());                 break;
        case Kind::If:          result.push_back(parseIf());                  break;
        case Kind::Print:
        case Kind::PrintLine:   result.push_back(parsePrint());               break;
        case Kind::Return:      result.push_back(parseReturn());              break;
        case Kind::Break:       result.push_back(parseBreak());               break;
        case Kind::Continue:    result.push_back(parseContinue());            break;
        case Kind::EndOfTokenList:
            cout << *current << " 잘못된 구문입니다.";
            exit(1);
        }
    }
    return result;
}

Variable* parseVariable() {
    Variable* result = new Variable();
    skipCurrent(Kind::Variable);
    result->name = current->str;
    skipCurrent(Kind::Identifier);
    skipCurrent(Kind::Assignment);
    result->expression = parseExpression();
    if(result->expression == nullptr) {
        cout << "변수 선언에 초기화식이 없습니다.";
        exit(1);
    }
    skipCurrent(Kind::Semicolon);
    return result;
}

For* parseFor() {
    For* result = new For();
    skipCurrent(Kind::For);
    result->variable = new Variable();
    result->variable->name = current->str;
    skipCurrent(Kind::Identifier);
    skipCurrent(Kind::Assignment);
    result->variable->expression = parseExpression();
    if(result->variable->expression == nullptr) {
        cout << "for문에 초기화식이 없습니다.";
        exit(1);
    }
    skipCurrent(Kind::Comma);
    result->condition = parseExpression();
    if(result->condition == nullptr) {
        cout << "for문에 조건식이 없습니다.";
        exit(1);
    }
    skipCurrent(Kind::Comma);
    result->expression = parseExpression();
    if(result->expression == nullptr) {
        cout << "for문에 증감식이 없습니다.";
        exit(1);
    }
    skipCurrent(Kind::LeftBrace);
    result->block = parseBlock();
    skipCurrent(Kind::RightBrace);
    return result;
}

If* parseIf() {
    If* result = new If();
    skipCurrent(Kind::If);
    do {
        Expression* condition = parseExpression();
        if(condition == nullptr) {
            cout << "if문에 조건식이 없습니다.";
            exit(1);
        }
        result->conditions.push_back(condition);
        skipCurrent(Kind::LeftBrace);
        result->blocks.push_back(parseBlock());
        skipCurrent(Kind::RightBrace);
    } while(skipCurrentIf(Kind::Elif));
    if(skipCurrentIf(Kind::Else)) {
        skipCurrent(Kind::LeftBrace);
        result->elseBlock = parseBlock();
        skipCurrent(Kind::RightBrace);
    }
    return result;
}

Print* parsePrint() {
    Print* result = new Print();
    result->lineFeed = current->kind == Kind::PrintLine;
    skipCurrent();
    if(current->kind != Kind::Semicolon) {
        do result->arguments.push_back(parseExpression());
        while(skipCurrentIf(Kind::Comma));
    }
    skipCurrent(Kind::Semicolon);
    return result;
}

Return* parseReturn() {
    Return* result = new Return();
    skipCurrent(Kind::Return);
    result->expression = parseExpression();
    if(result->expression == nullptr) {
        cout << "return문에 식이 없습니다.";
        exit(1);
    }
    skipCurrent(Kind::Semicolon);
    return result;
}

Break* parseBreak() {
    Break* result = new Break();
    skipCurrent(Kind::Break);
    skipCurrent(Kind::Semicolon);
    return result;
}

Continue* parseContinue() {
    Continue* result = new Continue();
    skipCurrent(Kind::Continue);
    skipCurrent(Kind::Semicolon);
    return result;
}

ExpressionStatement* parseExpressionStatement() {
    ExpressionStatement* result = new ExpressionStatement();
    result->expression = parseExpression();
    skipCurrent(Kind::Semicolon);
    return result;
}

Expression* parseExpression() {
    return parseAssignment();
}

Expression* parseAssignment() {
    Expression* result = parseOr();
    if(current->kind != Kind::Assignment)
        return result;
    skipCurrent(Kind::Assignment);
    if(GetVariable* getVariable = dynamic_cast<GetVariable*>(result)) {
        SetVariable* result = new SetVariable();
        result->name = getVariable->name;
        result->value = parseAssignment();
        return result;
    }
    if(GetElement* getElement = dynamic_cast<GetElement*>(result)) {
        SetElement* result = new SetElement();
        result->sub = getElement->sub;
        result->index = getElement->index;
        result->value = parseAssignment();
        return result;
    }
    cout << "잘못된 대입 연산 식입니다.";
    exit(1);
}

Expression* parseOr() {
    Expression* result = parseAnd();
    while(skipCurrentIf(Kind::LogicalOr)) {
        Or* temp = new Or();
        temp->lhs = result;
        temp->rhs = parseAnd();
        result = temp;
    }
    return result;
}

Expression* parseAnd() {
    Expression* result = parseRelational();
    while(skipCurrentIf(Kind::LogicalAnd)) {
        And* temp = new And();
        temp->lhs = result;
        temp->rhs = parseRelational();
        result = temp;
    }
    return result;
}

Expression* parseRelational() {
    set<Kind> operators = {
        Kind::Equal,
        Kind::NotEqual,
        Kind::LessThan,
        Kind::GreaterThan,
        Kind::LessOrEqual,
        Kind::GreaterOrEqual,
    };
    Expression* result = parseArithmetic1();
    while(operators.count(current->kind)) {
        Relational* temp = new Relational();
        temp->kind = current->kind;
        skipCurrent();
        temp->lhs = result;
        temp->rhs = parseArithmetic1();
        result = temp;
    }
    return result;
}

Expression* parseArithmetic1() {
    set<Kind> operators = {
        Kind::Add,
        Kind::Subtract
    };
    Expression* result = parseArithmetic2();
    while(operators.count(current->kind)) {
        Arithmetic* temp = new Arithmetic();
        temp->kind = current->kind;
        skipCurrent();
        temp->lhs = result;
        temp->rhs = parseArithmetic2();
        result = temp;
    }
    return result;
}

Expression* parseArithmetic2() {
    set<Kind> operators = {
        Kind::Multiply,
        Kind::Divide,
        Kind::Modulo
    };
    Expression* result = parseUnary();
    while(operators.count(current->kind)) {
        Arithmetic* temp = new Arithmetic();
        temp->kind = current->kind;
        skipCurrent();
        temp->lhs = result;
        temp->rhs = parseUnary();
        result = temp;
    }
    return result;
}

Expression* parseUnary() {
    set<Kind> operators = {
        Kind::Add,
        Kind::Subtract
    };
    if(operators.count(current->kind)) {
        Unary* result = new Unary();
        result->kind = current->kind;
        skipCurrent();
        result->sub = parseUnary();
        return result;
    }
    return parseOperand();
}

Expression* parseOperand() {
    Expression* result = nullptr;
    switch(current->kind) {
    case Kind::NullLiteral:   result = parseNullLiteral();     break;
    case Kind::TrueLiteral:
    case Kind::FalseLiteral:  result = parseBooleanLiteral();  break;
    case Kind::NumberLiteral: result = parseNumberLiteral();   break;
    case Kind::StringLiteral: result = parseStringLiteral();   break;
    case Kind::LeftBraket:    result = parseListLiteral();     break;
    case Kind::LeftBrace:     result = parseMapLiteral();      break;
    case Kind::Identifier:    result = parseIdentifier();      break;
    case Kind::LeftParen:     result = parseInnerExpression(); break;
    default:                  cout << "잘못된 식입니다.";      exit(1);
    }
    return parsePostfix(result);
}

Expression* parseNullLiteral() {
    skipCurrent(Kind::NullLiteral);
    NullLiteral* result = new NullLiteral();
    return result;
}

Expression* parseBooleanLiteral() {
    BooleanLiteral* result = new BooleanLiteral();
    result->value = current->kind == Kind::TrueLiteral;
    skipCurrent();
    return result;
}

Expression* parseNumberLiteral() {
    NumberLiteral* result = new NumberLiteral();
    result->value = stod(current->str);
    skipCurrent(Kind::NumberLiteral);
    return result;
}

Expression* parseStringLiteral() {
    StringLiteral* result = new StringLiteral();
    result->value = current->str;
    skipCurrent(Kind::StringLiteral);
    return result;
}

Expression* parseListLiteral() {
    ArrayLiteral* result = new ArrayLiteral();
    skipCurrent(Kind::LeftBraket);
    if(current->kind != Kind::RightBraket) {
        do result->values.push_back(parseExpression());
        while(skipCurrentIf(Kind::Comma));
    }
    skipCurrent(Kind::RightBraket);
    return result;
}

Expression* parseMapLiteral() {
    MapLiteral* result = new MapLiteral();
    skipCurrent(Kind::LeftBrace);
    if(current->kind != Kind::RightBrace) {
        do {
            string name = current->str;
            skipCurrent(Kind::StringLiteral);
            skipCurrent(Kind::Colon);
            Expression* value = parseExpression();
            result->values[name] = value;
        } while(skipCurrentIf(Kind::Comma));
    }
    skipCurrent(Kind::RightBrace);
    return result;
}

Expression* parseIdentifier() {
    GetVariable* result = new GetVariable();
    result->name = current->str;
    skipCurrent(Kind::Identifier);
    return result;
}

Expression* parseInnerExpression() {
    skipCurrent(Kind::LeftParen);
    Expression* result = parseExpression();
    skipCurrent(Kind::RightParen);
    return result;
}

Expression* parsePostfix(Expression* sub) {
    while(true) {
        switch(current->kind) {
        case Kind::LeftParen:  sub = parseCall(sub);    break;
        case Kind::LeftBraket: sub = parseElement(sub); break;
        default: return sub;
        }
    }
}

Expression* parseCall(Expression* sub) {
    Call* result = new Call();
    result->sub = sub;
    skipCurrent(Kind::LeftParen);
    if(current->kind != Kind::RightParen) {
        do result->arguments.push_back(parseExpression());
        while(skipCurrentIf(Kind::Comma));
    }
    skipCurrent(Kind::RightParen);
    return result;
}

Expression* parseElement(Expression* sub) {
    GetElement* result = new GetElement();
    result->sub = sub;
    skipCurrent(Kind::LeftBraket);
    result->index = parseExpression();
    skipCurrent(Kind::RightBraket);
    return result;
}

void skipCurrent() {
    ++current;
}

void skipCurrent(Kind kind) {
    if(current->kind != kind) {
        cout << toString(kind) << " 토큰이 필요합니다.";
        exit(1);
    }
    ++current;
}

bool skipCurrentIf(Kind token_kind) {
    if(current->kind != token_kind)
        return false;
    ++current;
    return true;
}
