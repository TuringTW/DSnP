%{
#include <iostream>

using namespace std;

//-----------------------
//    Extern variables
//-----------------------
int callex (void);
int calparse();
extern "C" int calwrap();

//-----------------------
//    static functoins
//-----------------------
static void calerror(const char *str)
{
   cerr << "Error: " << str << endl;
}

%}
%union {
   int  iv;
};

%token <iv> NUMBER
%token ADD SUB MULT DIV LEFT_P RIGHT_P ENDL
%type <iv> EXPRESSION SUM_EXP PROD_EXP TERM

%%
ANSWER: EXPRESSION
      | ANSWER EXPRESSION

EXPRESSION: SUM_EXP ENDL { $$ = $1; cout << $$ << endl; }

SUM_EXP: SUM_EXP ADD PROD_EXP { $$ = $1 + $3; }
       | SUM_EXP SUB PROD_EXP { $$ = $1 - $3; }
       | PROD_EXP { $$ = $1; }

PROD_EXP: PROD_EXP MULT TERM { $$ = $1 * $3; }
        | PROD_EXP DIV TERM { $$ = $1 / $3; }
        | TERM { $$ = $1; }

TERM: NUMBER { $$ = $1; }
    | LEFT_P SUM_EXP RIGHT_P { $$ = $2; }
    | SUB TERM { $$ = -$2; }

%%

int main()
{
   calparse();
}
