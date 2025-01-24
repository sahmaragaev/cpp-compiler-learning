#include <cstdio>
#include <cstring>
#include <cctype>
#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

//-------------------- Lexer --------------------

enum TokenType
{
    TOKEN_INT,
    TOKEN_PRINT,
    TOKEN_IDENT,
    TOKEN_NUMBER,
    TOKEN_ASSIGN, // '='
    TOKEN_PLUS,   // '+'
    TOKEN_MINUS,  // '-'
    TOKEN_STAR,   // '*'
    TOKEN_SLASH,  // '/'
    TOKEN_LPAREN, // '('
    TOKEN_RPAREN, // ')'
    TOKEN_SEMI,   // ';'
    TOKEN_EOF,
    TOKEN_ERROR
};

struct Token
{
    TokenType type;
    string text;
};

// Simple global variables to track input
static const char *g_input = nullptr;
static size_t g_pos = 0;

static Token getNextToken()
{
    while (isspace((unsigned char)g_input[g_pos]))
    {
        g_pos++;
    }

    if (g_input[g_pos] == '\0')
    {
        return Token{TOKEN_EOF, ""};
    }

    // Identifiers or keywords
    if (isalpha((unsigned char)g_input[g_pos]))
    {
        size_t start = g_pos;
        while (isalnum((unsigned char)g_input[g_pos]))
            g_pos++;
        string text = string(g_input + start, g_pos - start);
        if (text == "int")
            return Token{TOKEN_INT, text};
        if (text == "print")
            return Token{TOKEN_PRINT, text};
        return Token{TOKEN_IDENT, text};
    }

    // Numbers
    if (isdigit((unsigned char)g_input[g_pos]))
    {
        size_t start = g_pos;
        while (isdigit((unsigned char)g_input[g_pos]))
            g_pos++;
        string text = string(g_input + start, g_pos - start);
        return Token{TOKEN_NUMBER, text};
    }

    // Single char tokens
    char c = g_input[g_pos++];
    switch (c)
    {
    case '=':
        return Token{TOKEN_ASSIGN, "="};
    case '+':
        return Token{TOKEN_PLUS, "+"};
    case '-':
        return Token{TOKEN_MINUS, "-"};
    case '*':
        return Token{TOKEN_STAR, "*"};
    case '/':
        return Token{TOKEN_SLASH, "/"};
    case '(':
        return Token{TOKEN_LPAREN, "("};
    case ')':
        return Token{TOKEN_RPAREN, ")"};
    case ';':
        return Token{TOKEN_SEMI, ";"};
    default:
        return Token{TOKEN_ERROR, string(1, c)};
    }
}

//-------------------- Parser and AST --------------------

struct ExpressionAST
{
    // Base class for expressions
    virtual ~ExpressionAST() {}
};

struct NumberExpressionAST : public ExpressionAST
{
    int value;
    NumberExpressionAST(int v) : value(v) {}
};

struct VariableExpressionAST : public ExpressionAST
{
    string name;
    VariableExpressionAST(const string &n) : name(n) {}
};

struct BinaryExpressionAST : public ExpressionAST
{
    char op;
    ExpressionAST *lhs;
    ExpressionAST *rhs;
    BinaryExpressionAST(char op, ExpressionAST *lhs, ExpressionAST *rhs) : op(op), lhs(lhs), rhs(rhs) {}
};

struct StatementAST
{
    virtual ~StatementAST() {}
};

struct VarDeclareAST : public StatementAST
{
    string name;
    ExpressionAST *init;
    VarDeclareAST(const string &n, ExpressionAST *i) : name(n), init(i) {}
};

struct AssignAST : public StatementAST
{
    string name;
    ExpressionAST *expr;
    AssignAST(const string &n, ExpressionAST *e) : name(n), expr(e) {}
};

struct PrintAST : public StatementAST
{
    ExpressionAST *expr;
    PrintAST(ExpressionAST *e) : expr(e) {}
};

// The grammar:
// program := { declare | assign | printStatement } EOF
// declare := "int" IDENT [ "=" expr ] ";"
// assign := IDENT "=" expression ";"
// printStatement := "print" "(" expr ")" ";"
// expression := term { ("+"|"-") term }
// term := factor { ("*"|"/") factor }
// factor := IDENT | NUMBER | "(" expression ")"

static Token g_curToken;
static void getNextTokenGlobal() { g_curToken = getNextToken(); }

class Parser
{
public:
    Parser() {}

    vector<StatementAST *> parseProgram()
    {
        vector<StatementAST *> statements;
        while (g_curToken.type != TOKEN_EOF && g_curToken.type != TOKEN_ERROR)
        {
            // Lookahead
            if (g_curToken.type == TOKEN_INT)
            {
                statements.push_back(parseDeclaration());
            }
            else if (g_curToken.type == TOKEN_PRINT)
            {
                statements.push_back(parsePrint());
            }
            else if (g_curToken.type == TOKEN_IDENT)
            {
                statements.push_back(parseAssignment());
            }
            else
            {
                cerr << "Unexpected token at top-level\n";
                break;
            }
        }
        return statements;
    }

private:
    StatementAST *parseDeclaration()
    {
        // We already have 'int'
        getNextTokenGlobal(); // eat 'int'
        if (g_curToken.type != TOKEN_IDENT)
        {
            cerr << "Expected identifier in declaration\n";
            return nullptr;
        }
        string varName = g_curToken.text;
        getNextTokenGlobal(); // eat ident

        ExpressionAST *init = nullptr;
        if (g_curToken.type == TOKEN_ASSIGN)
        {
            getNextTokenGlobal(); // eat '='
            init = parseExpression();
        }

        if (g_curToken.type != TOKEN_SEMI)
        {
            cerr << "Expected ';' after declaration\n";
            return nullptr;
        }
        getNextTokenGlobal(); // eat ';'
        return new VarDeclareAST(varName, init);
    }

    StatementAST *parseAssignment()
    {
        // current token is ident
        string varName = g_curToken.text;
        getNextTokenGlobal(); // eat ident
        if (g_curToken.type != TOKEN_ASSIGN)
        {
            cerr << "Expected '=' in assignment\n";
            return nullptr;
        }
        getNextTokenGlobal(); // eat '='
        ExpressionAST *val = parseExpression();
        if (g_curToken.type != TOKEN_SEMI)
        {
            cerr << "Expected ';' after assignment\n";
            return nullptr;
        }
        getNextTokenGlobal(); // eat ';'
        return new AssignAST(varName, val);
    }

    StatementAST *parsePrint()
    {
        getNextTokenGlobal(); // eat 'print'
        if (g_curToken.type != TOKEN_LPAREN)
        {
            cerr << "Expected '('\n";
            return nullptr;
        }
        getNextTokenGlobal(); // eat '('
        ExpressionAST *expr = parseExpression();
        if (g_curToken.type != TOKEN_RPAREN)
        {
            cerr << "Expected ')'\n";
            return nullptr;
        }
        getNextTokenGlobal(); // eat ')'
        if (g_curToken.type != TOKEN_SEMI)
        {
            cerr << "Expected ';'\n";
            return nullptr;
        }
        getNextTokenGlobal(); // eat ';'
        return new PrintAST(expr);
    }

    ExpressionAST *parseExpression()
    {
        ExpressionAST *lhs = parseTerm();
        while (g_curToken.type == TOKEN_PLUS || g_curToken.type == TOKEN_MINUS)
        {
            char op = g_curToken.text[0];
            getNextTokenGlobal(); // eat op
            ExpressionAST *rhs = parseTerm();
            lhs = new BinaryExpressionAST(op, lhs, rhs);
        }
        return lhs;
    }

    ExpressionAST *parseTerm()
    {
        ExpressionAST *lhs = parseFactor();
        while (g_curToken.type == TOKEN_STAR || g_curToken.type == TOKEN_SLASH)
        {
            char op = g_curToken.text[0];
            getNextTokenGlobal(); // eat op
            ExpressionAST *rhs = parseFactor();
            lhs = new BinaryExpressionAST(op, lhs, rhs);
        }
        return lhs;
    }

    ExpressionAST *parseFactor()
    {
        if (g_curToken.type == TOKEN_IDENT)
        {
            string name = g_curToken.text;
            getNextTokenGlobal(); // eat ident
            return new VariableExpressionAST(name);
        }
        else if (g_curToken.type == TOKEN_NUMBER)
        {
            int val = stoi(g_curToken.text);
            getNextTokenGlobal(); // eat number
            return new NumberExpressionAST(val);
        }
        else if (g_curToken.type == TOKEN_LPAREN)
        {
            getNextTokenGlobal(); // eat '('
            ExpressionAST *expr = parseExpression();
            if (g_curToken.type != TOKEN_RPAREN)
            {
                cerr << "Expected ')' in factor\n";
                return nullptr;
            }
            getNextTokenGlobal(); // eat ')'
            return expr;
        }
        else
        {
            cerr << "Unexpected token in factor\n";
            return nullptr;
        }
    }
};

//-------------------- Semantic Checks --------------------
// For this I will be checking if the variables are being declared before used

static map<string, bool> g_variables;

struct CodeGen
{
    // We will "generate" code by printing out C code.
    // For a real compiler, I will use a proper IR (intermediate representation) or assembly.

    void genProgram(const vector<StatementAST *> &statements)
    {
        // Output a simple C program
        cout << "#include <stdio.h>\n";
        cout << "int main() {\n";

        for (auto stmt : statements)
        {
            genStmt(stmt);
        }

        cout << "return 0;\n";
        cout << "}\n";
    }

    void genStmt(StatementAST *stmt)
    {
        if (auto decl = dynamic_cast<VarDeclareAST *>(stmt))
        {
            g_variables[decl->name] = true;
            cout << "int " << decl->name;
            if (decl->init)
            {
                cout << " = ";
                genExpr(decl->init);
            }
            cout << ";\n";
        }
        else if (auto assign = dynamic_cast<AssignAST *>(stmt))
        {
            if (g_variables.find(assign->name) == g_variables.end())
            {
                cerr << "Error: variable " << assign->name << " not declared.\n";
                return;
            }
            cout << assign->name << " = ";
            genExpr(assign->expr);
            cout << ";\n";
        }
        else if (auto print = dynamic_cast<PrintAST *>(stmt))
        {
            cout << "printf(\"%d\\n\",";
            genExpr(print->expr);
            cout << ");\n";
        }
    }

    void genExpr(ExpressionAST *expr)
    {
        if (auto num = dynamic_cast<NumberExpressionAST *>(expr))
        {
            cout << num->value;
        }
        else if (auto var = dynamic_cast<VariableExpressionAST *>(expr))
        {
            if (g_variables.find(var->name) == g_variables.end())
            {
                cerr << "Error: variable " << var->name << " not declared.\n";
                return;
            }
            cout << var->name;
        }
        else if (auto bin = dynamic_cast<BinaryExpressionAST *>(expr))
        {
            // put parentheses around
            cout << "(";
            genExpr(bin->lhs);
            cout << " " << bin->op << " ";
            genExpr(bin->rhs);
            cout << ")";
        }
    }
};

//-------------------- Main --------------------

int main()
{
    string inputStr;
    {
        string line;
        while (getline(cin, line))
        {
            inputStr += line + "\n";
        }
    }

    g_input = inputStr.c_str();
    g_pos = 0;
    getNextTokenGlobal();

    Parser parser;
    vector<StatementAST *> statements = parser.parseProgram();

    CodeGen codegen;
    codegen.genProgram(statements);

    for (auto s : statements)
    {
        delete s;
    }

    return 0;
}
