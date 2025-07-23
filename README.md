# Urdu-type-C-language-Compiler
Compiler Construction Project – Phase 1, 2 & 3
This repository contains the full implementation of a Mini Urdu Compiler developed in C++ using custom-built data structures and design. The project is divided into three main phases:
1. # Lexical Analyzer (Scanner)
2. # Syntax Analyzer (LL(1) Parser)
3. # Intermediate Code Generator (Translation Scheme)
Each phase builds upon the previous one to simulate a complete front-end of a compiler.

# Tools & Constraints
- Language: C++
- Constraints: No STL/Standard Library data structures allowed (custom-built only)
- Input: Text file with source code using our defined language
- Output: Token stream, syntax validation result, and intermediate code
- Platform: Windows 10 Pro (stable), can be compiled via CLI using g++
- 
# Phase 1 – Lexical Analyzer
# Objective
To tokenize a custom source code into meaningful tokens using a table-driven approach.

# Features
- Supports:
  - Identifiers (must include at least one `_`)
  - Signed/unsigned numbers, floats, exponentials
  - String and char literals
  - Custom keywords (like `Adadi`, `Ashriya`, `Mantiqi`, `Wapas`, `Agar`, etc.)
  - Operators & punctuations
- Generates:
  - `tokens.txt` with type and value
  - `error.txt` with unrecognized tokens

# Example Input (Custom Language)
Adadi _num = 10 ::
Ashriya num_ = -10.5E+12 ::
Mantiqi _flag = True ::
Agar (_flag)
output<- "Ok" ::


# Phase 2 – Syntax Analyzer

# Objective: 
Implement an LL(1) predictive parser for the provided grammar.

# Supports parsing of:
Function definitions
Declarations and assignments
Arithmetic and relational expressions
Loops (for, while)
Conditionals (Agar, Wagarna)
Compound statements

# Implementation Steps:
Removed left recursion
Applied left factoring
Calculated FIRST and FOLLOW sets
Built LL(1) parsing table
Handled syntax errors and recovery


# Phase 3 – Translation Scheme (Intermediate Code Generator)

# Objective: 
Generate intermediate representation (IR) or three-address code from the valid parsed syntax.

# Covers translation for:
Assignment expressions
Arithmetic and relational expressions
Conditional statements (if-else)
While loops
Compound blocks

# Output Format:
A file named IR_Code.txt is generated, containing intermediate instructions, such as:
t1 = 10
_flag = True
if _flag goto L1
goto L2
L1:
print "Ok"
