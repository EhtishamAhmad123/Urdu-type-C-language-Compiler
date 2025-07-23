#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

struct Token {
    string value;
    string type;
};

struct Semantic {
    string code;
    string addr;
    string syn;
    string inh;
};

vector<Token> tokens;
int current = 0;
int indentLevel = 0;
int tempCount = 0, labelCount = 0;

string newTemp() {
    return "t" + to_string(tempCount++);
}

string newLabel() {
    return "L" + to_string(labelCount++);
}

string gen(const string& s) {
    return s + "\n";
}

void printNode(const string& label, bool isLast = false) {
    static vector<bool> indentFlags;
    for (int i = 0; i < indentLevel; ++i) {
        cout << (indentFlags[i] ? "`   " : "    ");
    }
    cout << (isLast ? "+-- " : "+-- ") << label << endl;
    if (indentFlags.size() <= indentLevel)
        indentFlags.push_back(true);
    else
        indentFlags[indentLevel] = !isLast;
    indentLevel++;
}

void popIndent() { indentLevel--; }

Token token() {
    if (current < tokens.size()) return tokens[current];
    return {"EOF", "EOF"};
}

bool match(const string& type) {
    if (token().type == type) {
        printNode(token().value + " (" + token().type + ")", true);
        current++;
        return true;
    }
    return false;
}

bool expecting_token(const string& val, const string& type) {
    if (token().value == val && token().type == type) {
        printNode(val + " (" + type + ")", true);
        current++;
        return true;
    }
    cout << "Syntax Error: Expected '" << val << "' but found '" << token().value << "' (" << token().type << ")\n";
    exit(1);
}

Semantic parseStmt();
Semantic parseMag();
Semantic parseTerm();
Semantic parseTermDash(string inh, string codeSoFar);
Semantic parseMagDash(string inh, string codeSoFar);
Semantic parseFactor();
Semantic parseRvalue();
Semantic parseWhileStmt();

Semantic parseFactor() {
    printNode("Factor");
    Semantic factor;
    if (token().type == "NUMBER") {
        factor.addr = token().value;
        factor.code = "";
        match("NUMBER");
    } else if (token().type == "IDENTIFIER") {
        factor.addr = token().value;
        factor.code = "";
        match("IDENTIFIER");
    } else if (token().value == "(") {
        match("PUNCTUATION");
        factor = parseMag();
        expecting_token(")", "PUNCTUATION");
    } else {
        cout << "Syntax Error: Expected factor\n";
        exit(1);
    }
    popIndent();
    return factor;
}

Semantic parseTermDash(string inh, string codeSoFar) {
    printNode("TermDash");
    Semantic termDash;
    termDash.inh = inh;
    termDash.code = codeSoFar;

    if (token().value == "*" || token().value == "/") {
        string op = token().value;
        match("OPERATOR");
        Semantic factor = parseFactor();
        string temp = newTemp();
        string newCode = gen(temp + " = " + inh + " " + op + " " + factor.addr);
        Semantic next = parseTermDash(temp, "");
        termDash.syn = next.syn;
        termDash.code = codeSoFar + factor.code + newCode + next.code;
    } else {
        termDash.syn = inh;
    }
    popIndent();
    return termDash;
}

Semantic parseTerm() {
    printNode("Term");
    Semantic factor = parseFactor();
    Semantic termDash = parseTermDash(factor.addr, factor.code);
    Semantic term;
    term.addr = termDash.syn;
    term.code = termDash.code;
    popIndent();
    return term;
}

Semantic parseMagDash(string inh, string codeSoFar) {
    printNode("MagDash");
    Semantic magDash;
    magDash.inh = inh;
    magDash.code = codeSoFar;

    if (token().value == "+" || token().value == "-") {
        string op = token().value;
        match("OPERATOR");
        Semantic term = parseTerm();
        string temp = newTemp();
        string newCode = gen(temp + " = " + inh + " " + op + " " + term.addr);
        Semantic next = parseMagDash(temp, "");
        magDash.syn = next.syn;
        magDash.code = codeSoFar + term.code + newCode + next.code;
    } else {
        magDash.syn = inh;
    }
    popIndent();
    return magDash;
}

Semantic parseMag() {
    printNode("Mag");
    Semantic term = parseTerm();
    Semantic magDash = parseMagDash(term.addr, term.code);
    Semantic mag;
    mag.addr = magDash.syn;
    mag.code = magDash.code;
    popIndent();
    return mag;
}

Semantic parseRvalue() {
    printNode("Rvalue");
    Semantic left = parseMag();
    Semantic rvalue;

    if (token().type == "OPERATOR" && (token().value == "==" || token().value == "!=" || token().value == "<" || token().value == ">" || token().value == "<=" || token().value == ">=")) {
        string op = token().value;
        match("OPERATOR");
        Semantic right = parseMag();
        string temp = newTemp();
        rvalue.addr = temp;
        rvalue.code = left.code + right.code + gen(temp + " = " + left.addr + " " + op + " " + right.addr);
    } else {
        rvalue.addr = left.addr;
        rvalue.code = left.code;
    }
    popIndent();
    return rvalue;
}

Semantic parseWhileStmt() {
    printNode("WhileStmt");
    expecting_token("while", "KEYWORD");
    expecting_token("(", "PUNCTUATION");
    Semantic cond = parseRvalue();
    expecting_token(")", "PUNCTUATION");
    string Lstart = newLabel(), Lbody = newLabel(), Lexit = newLabel();
    Semantic body = parseStmt();
    Semantic whileStmt;
    whileStmt.code = gen(Lstart + ":") + cond.code + gen("if " + cond.addr + " goto " + Lbody) + gen("goto " + Lexit) + gen(Lbody + ":") + body.code + gen("goto " + Lstart) + gen(Lexit + ":");
    popIndent();
    return whileStmt;
}

Semantic parseStmt() {
    printNode("Stmt");
    Semantic stmt;
    if (token().value == "while") {
        stmt = parseWhileStmt();
    } else if (token().type == "IDENTIFIER") {
        string id = token().value;
        match("IDENTIFIER");
        expecting_token(":=", "OPERATOR");
        Semantic expr = parseMag();
        expecting_token("::", "PUNCTUATION");
        stmt.code = expr.code + gen(id + " := " + expr.addr);
    } else {
        stmt.code = "";
    }
    popIndent();
    return stmt;
}

void loadTokens(const string& filename) {
    ifstream file(filename);
    string val, typ;
    while (file >> val >> typ) {
        tokens.push_back({ val, typ });
    }
    file.close();
}

int main() {
    loadTokens("tokens.txt");
    cout << "Parse Tree:\n";
    Semantic result = parseStmt();
    cout << "\nGenerated Code:\n" << result.code;
    cout << "\nParsing completed successfully!\n";
    return 0;
}
