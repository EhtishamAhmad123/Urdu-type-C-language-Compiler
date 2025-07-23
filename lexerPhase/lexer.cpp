#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

using namespace std;

const int NUM_STATES = 53;
const int NUM_INPUTS = 25;
const int BUFFER_SIZE = 25;
int T[NUM_STATES][NUM_INPUTS];
int Advance[NUM_STATES][NUM_INPUTS];

char buffer1[BUFFER_SIZE + 1], buffer2[BUFFER_SIZE + 1];
char* bufPointer = buffer1;
bool useBuffer1 = true;
ifstream inputFile;

enum States { S = 0, A1 = 1, A2 = 2, A3 = 3, A4 = 4, A5 = 5, A6 = 6, A7 = 7, A8 = 8, A9 = 9, A10 = 10, A11 = 11, A12 = 12, A13 = 13, A14 = 14, A15 = 15, A16 = 16, A17 = 17, A18 = 18, A19 = 19, A20 = 20, A21 = 21, A22 = 22, A23 = 23, A24 = 24, A25 = 25, A26 = 26, A27 = 27, A28 = 28, A29 = 29, A30 = 30, A31 = 31, A32 = 32, A33 = 33, A34 = 34, A35 = 35, A36 = 36, A37 = 37, A38 = 38, A39 = 39, A40 = 40, A41 = 41, A42 = 42, A43 = 43, A44 = 44, A45 = 45, A46 = 46, A47 = 47, A48 = 48, A49 = 49 };

enum InputType { LETTER = 0, UNDERSCORE = 1, DIGIT = 2, PLUS = 3, MINUS = 4, DECIMAL = 5, EXPO = 6, P1 = 7, P2 = 8, P3 = 9, P4 = 10, P5 = 11, P6 = 12, P7 = 13, LESSTHAN = 14, GREATERTHAN = 15, EQUALTO = 16, EXCLAMATION = 17, PERCENT = 18, AND = 19, LINE = 20, MULTIPLY = 21, COLUMN = 22, DIVIDE = 23, SPACE = 24 };

bool Accept[NUM_STATES] = { 0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,0,1,1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,0,0,0,0 };

void cleanFile(const string& filename)
{
    ifstream inputFile(filename);
    if (!inputFile)
    {
        cout << "Error opening file!" << endl;
        return;
    }

    string outputFilename = "temp.txt";
    ofstream outputFile(outputFilename);
    if (!outputFile)
    {
        cout << "Error creating output file!" << endl;
        return;
    }

    char ch;
    bool inSingleComment = false, inMultiComment = false;
    bool lastWasSpace = false;

    while (inputFile.get(ch))
    {
        if (inSingleComment && ch == '\n')
        {
            inSingleComment = false;
            outputFile.put(ch);
            lastWasSpace = false;
            continue;
        }
        if (inMultiComment && ch == '*' && inputFile.peek() == '/')
        {
            inputFile.get(ch);
            inMultiComment = false;
            continue;
        }
        if (inSingleComment || inMultiComment)
            continue;

        if (ch == '/' && inputFile.peek() == '/')
        {
            inSingleComment = true;
            inputFile.get(ch);
            continue;
        }
        if (ch == '/' && inputFile.peek() == '*')
        {
            inMultiComment = true;
            inputFile.get(ch);
            continue;
        }

        if (isspace(ch))
        {
            if (!lastWasSpace)
            {
                outputFile.put(' ');
                lastWasSpace = true;
            }
        }
        else
        {
            outputFile.put(ch);
            lastWasSpace = false;
        }
    }
    inputFile.close();
    outputFile.close();

    cout << "ALL THE COMMENTS AND EXTRA SPACES ARE REMOVED FROM THE CODE: " << outputFilename << endl;
}

int charactermapping(char ch)
{
    if (isalpha(ch) && ch != 'E')
        return LETTER;
    if (ch == ' ')
        return SPACE;
    if (ch == '_')
        return UNDERSCORE;
    if (isdigit(ch))
        return DIGIT;
    if (ch == '+')
        return PLUS;
    if (ch == '-')
        return MINUS;
    if (ch == '.')
        return DECIMAL;
    if (ch == 'E')
        return EXPO;
    if (ch == '[')
        return P1;
    if (ch == ']')
        return P2;
    if (ch == ')')
        return P3;
    if (ch == '(')
        return P4;
    if (ch == '{')
        return P5;
    if (ch == '}')
        return P6;
    if (ch == ':')
        return P7;
    if (ch == '<')
        return LESSTHAN;
    if (ch == '>')
        return GREATERTHAN;
    if (ch == '=')
        return EQUALTO;
    if (ch == '!')
        return EXCLAMATION;
    if (ch == '%')
        return PERCENT;
    if (ch == '|')
        return LINE;
    if (ch == '*')
        return MULTIPLY;
    if (ch == '"')
        return COLUMN;
    if (ch == '/')
        return DIVIDE;
    return -1;
}

void loadCSVTable(const string& filename, int table[NUM_STATES][NUM_INPUTS])
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Error opening file: " << filename << endl;
        return;
    }
    string line;
    int row = 0;
    while (getline(file, line) && row < NUM_STATES)
    {
        stringstream ss(line);
        string cell;
        int col = 0;
        while (getline(ss, cell, '\t') && col < NUM_INPUTS)
        {
            table[row][col] = stoi(cell);
            col++;
        }
        row++;
    }

    file.close();
}

int getKeywordPosition(const string& token, const string& filename)
{
    ifstream keywordFile(filename);
    if (!keywordFile)
    {
        cout << "Error: Could not open " << filename << endl;
        return -1;
    }

    string keyword;
    int lineNumber = 1;
    while (getline(keywordFile, keyword))
    {
        if (!keyword.empty() && keyword.back() == ',')
        {
            keyword.pop_back();
        }
        if (keyword == token)
        {
            return lineNumber;
        }
        lineNumber++;
    }
    return -1;
}

bool loadBuffer()
{
    if (!inputFile.eof())
    {
        char* targetBuffer = useBuffer1 ? buffer1 : buffer2;
        inputFile.read(targetBuffer, BUFFER_SIZE);
        int charsRead = inputFile.gcount();
        targetBuffer[charsRead] = '\0';

        if (charsRead > 0)
        {
            bufPointer = targetBuffer;
            useBuffer1 = !useBuffer1;
            return true;
        }
    }
    return false;
}

char getNextChar()
{
    if (*bufPointer == '\0')
    {
        if (!loadBuffer())
            return EOF;
    }
    char ch = *bufPointer;
    bufPointer++;
    if (*bufPointer == '\0' && !useBuffer1)
    {
        if (!loadBuffer())
            return EOF;
    }
    return ch;
}

void lexer()
{
    int state = S;
    string token = "";
    char ch = getNextChar();
    bool capturingString = false;
    string stringLiteral = "";

    fstream literalFile("Lexer_22L6989_Literaltable.txt", ios::out | ios::app);
    fstream symbolTable("Lexer_22L6989_SymbolTable.txt", ios::out | ios::app);
    fstream errorFile("Lexer_22L6989_Error.txt", ios::out | ios::app);

    if (!literalFile || !symbolTable || !errorFile)
    {
        cout << "Error opening files for writing!" << endl;
        return;
    }
    map<string, int> identifierMap;
    map<string, int> literalMap;
    int idCounter = 1;
    int literalCounter = 1;

    while (ch != EOF)
    {
        int charType = charactermapping(ch);
        if (charType == -1)
        {
            ch = getNextChar();
            continue;
        }

        if (ch == '"')
        {
            if (!capturingString)
            {
                capturingString = true;
                stringLiteral = "";
            }
            else
            {
                capturingString = false;
                if (literalMap.find(stringLiteral) == literalMap.end())
                {
                    literalMap[stringLiteral] = literalCounter++;
                    literalFile << "<" << stringLiteral << ", " << literalMap[stringLiteral] << ">" << endl;
                }
                cout << "<literal, " << literalMap[stringLiteral] << ">  ";
            }
            ch = getNextChar();
            continue;
        }

        if (capturingString)
        {
            stringLiteral += ch;  // Capture inside quotes
            ch = getNextChar();
            continue;
        }

        while (!Accept[state] && state != -1)
        {
            //cout << "The current state is " << state << " The current chartype is: " << charType << endl;
            int newState = T[state][charType];
            //cout << "The current state is: " << T[state][charType] << endl;
            if (newState == -1)
                break;
            //cout << "The current Advance is: " << Advance[state][charType] << endl;
            if (Advance[state][charType] == 1)
            {
                token += ch;
                //cout << "The current token is: " << token << endl;
                ch = getNextChar();
                //cout << "The current character is: " << ch << endl;
                if (ch == ' ' && newState == 31)
                    newState++;
                //if (ch == EOF)
                  //  break;
                charType = charactermapping(ch);
                //cout << "The current char type is " << charType << endl;
                if (charType == -1 && (int)ch != -1)
                {   
                    cout << "Error: Invalid token -> " << ch << endl;
                    errorFile << "Error: Invalid token -> " << ch << endl;
                    charType = 24;
                }
            }
            state = newState;
            //cout << "The new state now is: " << state << endl;
        }

        if (Accept[state])
        {
            if (state == 4)
            {  // Check only keywords
                int position = getKeywordPosition(token, "keyword.txt");
                if (position != -1)
                {
                    cout << "<Keyword, " << position << ">  ";
                }
                else
                {
                    cout << "Error: Invalid token -> " << token << endl;
                    errorFile << token << endl;
                }
            }
            else if (state == 3)
            {  // Identifiers
                if (identifierMap.find(token) == identifierMap.end())
                {
                    identifierMap[token] = idCounter++;
                    symbolTable << "<" << token << ", " << identifierMap[token] << ">" << endl;
                }
                cout << "<id, " << identifierMap[token] << ">  ";
            }
            else
            {
                cout << "<" << token << ">  ";
            }
            token = "";
            state = S;
        }
        else
        {
            if (ch != ' ')
            {
                cout << "Error: Invalid Token at '" << ch << "'" << endl;
                errorFile << "Error: Invalid Token at '" << ch << "'" << endl;
            }

            ch = getNextChar();
            if (ch == EOF)
                break;
            state = S;
            token = "";
        }
    }
}

int main()
{
    ofstream clearSymbolTable("Lexer_22L6989_SymbolTable.txt", ios::trunc);
    ofstream clearLiteralTable("Lexer_22L6989_Literaltable.txt", ios::trunc);
    ofstream clearErrorFile("Lexer_22L6989_Error.txt", ios::trunc); // Clear error file
    clearSymbolTable.close();
    clearLiteralTable.close();
    clearErrorFile.close();
    loadCSVTable("Lexer_22L6989_TransitionTable.csv", T);
    /*cout << "Tables successfully loaded from CSV files!" << endl;
    cout << "Transition Table: " << endl;
    for (int i = 0; i < NUM_STATES; i++) {
        for (int j = 0; j < NUM_INPUTS; j++)
        {
            cout << T[i][j] << " ";
        }
        cout << endl << endl;
    }
    cout << endl;*/
    loadCSVTable("Lexer_22L6989_AdvanceTable.csv", Advance);
    /*cout << "Tables successfully loaded from CSV files!" << endl;
    cout << "Transition Table: " << endl;
    for (int i = 0; i < NUM_STATES; i++) {
        for (int j = 0; j < NUM_INPUTS; j++)
        {
            cout << T[i][j] << " ";
        }
        cout << endl << endl;
    }
    cout << endl;
    */
    string filename = "Lexer_22L6989_Source.txt";
    //cout << "Enter filename: ";
    //cin >> filename;
    cleanFile(filename);
    string output = "Updated_Lexer_22L6989_Source.txt";
    inputFile.open(output);
    if (!inputFile)
    {
        cout << "Error opening file!" << endl;
        return 1;
    }

    cout << "\n------------Welcome To My Lexer!--------\n";
    loadBuffer(); // Load first buffer
    lexer();
    cout << "\n============ End Of Lexer ==============\n";

    inputFile.close();
    return 0;
}