%{
/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <string>
#include <unistd.h>

#include <errno.h>
#include <limits.h>
#include <cstdlib>
#include <cstddef>

#define YYSTYPE std::string
#include "config_parser.h"

using namespace std;

extern long lineno;

/*--- Function prototypes -------------------------------------*/

extern void yyerror(const string& message);

%}

%option never-interactive
%option nounput

%x QUOTE

DIGIT           ([0-9])
SIGN            ([+-]?)
UINT            ({DIGIT}+)
INT             ({SIGN}{DIGIT}+)
EXP             (([eE]{INT})?)
REAL            ({SIGN}{DIGIT}+\.?{DIGIT}*{EXP})
RELATION        ("=="|"!="|"<"|">"|"<="|">=")

%%

   /*--- Whitespace characters --------------------------------*/

\n                      lineno++;

[ \t\v\f\r]+            ;   /* Ignored */

   /*--- Comments ---------------------------------------------*/

#[^\n]*                 ;   /* Ignore rest of the line */

   /*--- Relations --------------------------------------------*/

{RELATION}              {
                            yylval = yytext;
                            return SC_RELATION;
                        }

   /*--- Numbers ----------------------------------------------*/

{UINT}                  {
                           unsigned long value;
                           
                           // Convert string
                           value = strtol(yytext, (char **)NULL, 10);
                           
                           // Check for overflow
                           if (errno == ERANGE || value > UINT_MAX)
                              yyerror("Unsigned integer constant out of range!");

                           yylval = yytext;
                           return SC_UNSIGNED;
                        }

{INT}                  {
                          long value;
                           
                           // Convert string
                           value = strtoul(yytext, (char **)NULL, 10);
                           
                           // Check for overflow
                           if (errno == ERANGE || value < INT_MIN || value > INT_MAX)
                              yyerror("Integer constant out of range!");

                           yylval = yytext;
                           return SC_INTEGER;
                        }

{REAL}                  {
                           // double value;
                           
                           // Convert string (syntax check only)
                           /* value = */ strtod(yytext, (char **)NULL);
                           
                           // Check for overflow
                           if (errno == ERANGE)
                              yyerror("Floating point constant out of range!");

                           yylval = yytext;
                           return SC_REAL;
                        }
   /*--- Quoted strings ---------------------------------------*/

\"                      {
                          BEGIN(QUOTE);
                          return yytext[0];
                        }
<QUOTE>{
  [^"\n]+               {
                          yylval = yytext;
                          return SC_STRING;
                        }
  \n                    yyerror("String constant exceeds line!");
  \"                    {
                          BEGIN(INITIAL);
                          return yytext[0];
                        }
}

   /*--- Keywords ---------------------------------------------*/

SC_BALANCE         return SC_BALANCE;
SC_CUT             return SC_CUT;
SC_MESSAGE         return SC_MESSAGE;
SC_MODEL           return SC_MODEL;
SC_REGION          return SC_REGION;
SC_SCALE           return SC_SCALE;
SC_SIZE            return SC_SIZE;
SC_TAG             return SC_TAG;
SC_OPTION          return SC_OPTION;
SC_AGGREGATE_IDLE  return SC_AGGREGATE_IDLE;
SC_NOOP_THRESHOLD  return SC_NOOP_THRESHOLD;
SC_SKIP_THRESHOLD  return SC_SKIP_THRESHOLD;
SC_IDLE_OVERHEAD   return SC_IDLE_OVERHEAD;
   
   /*--- Default rule -----------------------------------------*/

.                       return yytext[0];

%%

/* dummy function to work around a common compiler warning,
   due to bison's output with '%option noyywrap' */
int yywrap()
{
    return 1;
}
