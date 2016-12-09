%{
#include <cstdlib>
#include <iostream>
#include "calYacc.hpp"

using namespace std;

extern "C" {
   int calwrap() { return 1; }
}

%}

DIGIT  [0-9]
NUMBER {DIGIT}+
WS     [ \t]+
ENDL   "\n"

%%
{NUMBER}    { callval.iv = atoi(caltext); return NUMBER; }
{WS}        {}
"+"         { return ADD; }
"-"         { return SUB; }
"*"         { return MULT; }
"/"         { return DIV; }
"("         { return LEFT_P; }
")"         { return RIGHT_P; }
{ENDL}      { return ENDL; }
%%

/*
int main()
{
   cout << "Hello!" << endl;
   yylex();
   return 0;
}
*/
