#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

struct Token 
{
    string value;
    string type;
};

vector<Token> tokens;
int current = 0;
int indentLevel = 0;

void printNode(const string& label, bool isLast = false) 
{
    static vector<bool> indentFlags;

    for (int i = 0; i < indentLevel; ++i) 
    {
        cout << (indentFlags[i] ? "`   " : "    ");
    }

    cout << (isLast ? "+-- " : "+-- ") << label << endl;

    if (indentFlags.size() <= indentLevel)
        indentFlags.push_back(true);
    else
        indentFlags[indentLevel] = !isLast;

    indentLevel++;
}

void popIndent() 
{
    indentLevel--;
}

Token token() 
{
    if (current < tokens.size())
        return tokens[current];
    return { "EOF", "EOF" };
}

bool match(const string& type) 
{
    if (token().type == type) 
    {
        printNode(token().value + " (" + token().type + ")", true);
        current++;
        return true;
    }
    return false;
}

bool expecting_token(const string& val, const string& type) 
{
    if (token().value == val && token().type == type) 
    {
        printNode(val + " (" + type + ")", true);
        current++;
        return true;
    }
    cout << "Syntax Error: Expected '" << val << "' but found '" << token().value << "' (" << token().type << ")\n";
    exit(1);
}

void parseExpr();
void parseTermDash();
void parseMagDash();
void parseRvalueDash();
void parseStmt();
void parseStmtDash();
void parseOpenDash();
void parseOpen();
void parseCompStmt();
void parseStmtListDash();
void parseArgList();
void parseArg();
void parseArgListDash();
void parseIdentList();
void parseIdentListDash();
void parseDeclaration();
void parseType();

void parseFactor() 
{
    printNode("Factor");
    if (match("NUMBER") || match("IDENTIFIER")) 
    {
        popIndent();
        return;
    }
    else if (token().value == "(") 
    {
        match("PUNCTUATION");
        parseExpr();
        expecting_token(")", "PUNCTUATION");
    }
    else 
    {
        cout << "Syntax Error: Expected factor\n";
        exit(1);
    }
    popIndent();
}

void parseTerm() 
{
    printNode("Term");
    parseFactor();
    parseTermDash();
    popIndent();
}

void parseTermDash() 
{
    printNode("TermDash");
    if (token().value == "*" || token().value == "/") 
    {
        match("OPERATOR");
        parseFactor();
        parseTermDash();
    }
    else 
    {
        printNode("null", true);
    }
    popIndent();
}

void parseMag() 
{
    printNode("Mag");
    parseTerm();
    parseMagDash();
    popIndent();
}

void parseMagDash() 
{
    printNode("MagDash");
    if (token().value == "+" || token().value == "-") {
        match("OPERATOR");
        parseTerm();
        parseMagDash();
    }
    else 
    {
        printNode("null", true);
    }
    popIndent();
}

void parseRvalue() 
{
    printNode("Rvalue");
    parseMag();
    parseRvalueDash();
    popIndent();
}

void parseRvalueDash() 
{
    printNode("RvalueDash");
    if (token().type == "OPERATOR" && (token().value == "==" || token().value == "!=" || token().value == "<" || token().value == ">" || token().value == "<=" || token().value == ">=" || token().value == "<>")) 
    {
        match("OPERATOR");
        parseMag();
        parseRvalueDash();
    }
    else 
    {
        printNode("null", true);
    }
    popIndent();
}

void parseExpr() 
{
    printNode("Expr");
    if (token().type == "IDENTIFIER") 
    {
        Token lookahead = tokens[current + 1];
        if (lookahead.value == ":=") 
        {
            printNode("Assignment");
            match("IDENTIFIER");
            expecting_token(":=", "OPERATOR");
            parseExpr();
        }
        else 
        {
            parseRvalue();
        }
    }
    else 
    {
        parseRvalue();
    }
    popIndent();
}

void parseOptExpr() 
{
    printNode("OptExpr");
    if (token().type == "IDENTIFIER" || token().type == "NUMBER") 
    {
        parseExpr();
    }
    else 
    {
        printNode("null", true);
    }
    popIndent();
}

void parseForStmt() 
{
    printNode("ForStmt");
    expecting_token("for", "KEYWORD");
    expecting_token("(", "PUNCTUATION");
    parseExpr();
    expecting_token(":::", "OPERATOR");
    parseOptExpr();
    expecting_token(":", "PUNCTUATION");
    parseOptExpr();
    expecting_token(")", "PUNCTUATION");
    parseStmt();
    popIndent();
}

void parseWhileStmt() 
{
    printNode("WhileStmt");
    expecting_token("while", "KEYWORD");
    expecting_token("(", "PUNCTUATION");
    parseExpr();
    expecting_token(")", "PUNCTUATION");
    parseStmt();
    popIndent();
}

void parseMatch() 
{
    printNode("Match");
    if (token().value == "Agar")
    {
        expecting_token("Agar", "KEYWORD");
        expecting_token("(", "PUNCTUATION");
        parseExpr();
        expecting_token(")", "PUNCTUATION");
        parseMatch();
        expecting_token("Wagarna", "KEYWORD");
        parseMatch();
    }
    else 
    {
        match("other");
    }
    popIndent();
}

void parseOpen() 
{
    printNode("Open");
    expecting_token("Agar", "KEYWORD");
    expecting_token("(", "PUNCTUATION");
    parseExpr();
    expecting_token(")", "PUNCTUATION");
    parseOpenDash();
    popIndent();
}

void parseOpenDash() 
{
    printNode("OpenDash");
    if (token().value == "match") 
    {
        parseMatch();
        expecting_token("Wagarna", "KEYWORD");
        parseOpen();
    }
    else 
    {
        parseStmt();
    }
    popIndent();
}

void parseStmtDash() 
{
    printNode("StmtDash");
    if (token().value == "match") 
    {
        parseMatch();
        expecting_token("Wagarna", "KEYWORD");
        parseMatch();
    }
    else 
    {
        parseOpenDash();
    }
    popIndent();
}

void parseStmt() 
{
    printNode("Stmt");
    if (token().value == "for")
        parseForStmt();
    else if (token().value == "while")
        parseWhileStmt();
    else if (token().value == "Agar") 
    {
        expecting_token("Agar", "KEYWORD");
        expecting_token("(", "PUNCTUATION");
        parseExpr();
        expecting_token(")", "PUNCTUATION");
        parseStmtDash();
    }
    else if (token().type == "IDENTIFIER")
    {
        parseExpr();
        expecting_token("::", "PUNCTUATION");
    }
    else if (token().value == "::") 
    {
        match("PUNCTUATION");
    }
    else if (token().value == "{")
    {
        parseCompStmt();
    }
    else if (token().type == "KEYWORD") 
    {
        parseDeclaration();
    }
    else if (match("other"))
    {
        return;
    }
    else 
    {
        cout << "Syntax Error: Invalid statement\n";
        exit(1);
    }
    popIndent();
}

void parseStmtList()
{
    printNode("StmtList");
    parseStmt();
    parseStmtListDash();
    popIndent();
}

void parseStmtListDash() 
{
    printNode("StmtListDash");
    if (token().value != "}")
    {
        parseStmt();
        parseStmtListDash();
    }
    else 
    {
        printNode("null", true);
    }
    popIndent();
}

void parseCompStmt() 
{
    printNode("CompStmt");
    expecting_token("{", "PUNCTUATION");
    parseStmtList();
    expecting_token("}", "PUNCTUATION");
    popIndent();
}

void parseType() 
{
    printNode("Type");
    if (!match("KEYWORD")) 
    {
        cout << "Syntax Error: Expected Type\n";
        exit(1);
    }
    popIndent();
}

void parseIdentList() 
{
    printNode("IdentList");
    if (!match("IDENTIFIER")) 
    {
        cout << "Syntax Error: Expected identifier in IdentList\n";
        exit(1);
    }
    parseIdentListDash();
    popIndent();
}

void parseIdentListDash() 
{
    printNode("IdentListDash");
    if (token().value == "," && match("PUNCTUATION")) 
    {
        parseIdentList();
    }
    else 
    {
        printNode("null", true);
    }
    popIndent();
}

void parseDeclaration() 
{
    printNode("Declaration");
    parseType();
    parseIdentList();
    expecting_token("::", "PUNCTUATION");
    popIndent();
}

void parseArg() 
{
    printNode("Arg");
    parseType();
    match("IDENTIFIER");
    popIndent();
}

void parseArgListDash() 
{
    printNode("ArgListDash");
    if (token().value == "," && match("PUNCTUATION"))
    {
        parseArg();
        parseArgListDash();
    }
    else 
    {
        printNode("null", true);
    }
    popIndent();
}

void parseArgList() 
{
    printNode("ArgList");
    parseArg();
    parseArgListDash();
    popIndent();
}

void parseFunction() 
{
    printNode("Function");
    parseType();
    match("IDENTIFIER");
    expecting_token("(", "PUNCTUATION");
    parseArgList();
    expecting_token(")", "PUNCTUATION");
    parseCompStmt();
    popIndent();
}

void loadTokens(const string& filename) 
{
    ifstream file(filename);
    string val, typ;
    while (file >> val >> typ) 
    {
        tokens.push_back({ val, typ });
    }
    file.close();
}

int main() 
{
    loadTokens("Token.txt");
    cout << "Parse Tree:\n";
    parseFunction();
    cout << "\nParsing completed successfully!\n";
    return 0;
}
