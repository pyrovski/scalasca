%{
/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <string>

#define YYSTYPE std::string
#include "parser.h"

using namespace std;


/*--- Global variables ----------------------------------------------------*/

extern long lineno;
extern long depth;

extern string   incFilename;
extern int      incLine;
YY_BUFFER_STATE incState;


/*--- Function prototypes -------------------------------------------------*/

extern void yyerror(const string& message);


%}


%option noyywrap

%x COMMENT
%x QUOTE
%x BRACKET


%%

   /*--- Whitespace characters --------------------------------------------*/

\n                      lineno++;

[ \t\v\f\r]+            ;   /* Ignored */


   /*--- Comments ---------------------------------------------------------*/

#\{                     BEGIN(COMMENT);
<COMMENT>{
  [^#\n]*               /* Ignore anything that's not a '#' */
  #+[^#}\n]*            /* Ignore '#'s not followed by a '}' */
  \n                    lineno++;
  #\}                   BEGIN(INITIAL);
}

#[^\n]*                 ;   /* Ignore rest of the line */


   /*--- Quoted strings ---------------------------------------------------*/

\"                      {
                          BEGIN(QUOTE);
                          return yytext[0];
                        }
<QUOTE>{
  [^"\n]+               {
                          yylval = yytext;
                          return STRING;
                        }
  \n                    yyerror("String constant exceeds line!");
  \"                    {
                          BEGIN(INITIAL);
                          return yytext[0];
                        }
}


   /*--- {...} text blocks ------------------------------------------------*/

\{                      {
                          depth++;
                          BEGIN(BRACKET);
                          return yytext[0];
                        }
<BRACKET>{
  [^\{\}\n]+            {
                          yylval = yytext;
                          return TEXT;
                        }
  \n                    {
                          lineno++;
                          yylval = yytext;
                          return TEXT;
                        }
  \{                    {
                          depth++;
                          yylval = yytext;
                          return TEXT;
                        }
  \}                    {
                          if (--depth <= 0) {
                            BEGIN(INITIAL);
                            return yytext[0];
                          }

                          yylval = yytext;
                          return TEXT;
                        }
}


   /*--- Keywords ---------------------------------------------------------*/

CALLBACKS               return CALLBACKS;
CLASS                   return CLASS;
CLEANUP                 return CLEANUP;
CONDITION               return CONDITION;
DATA                    return DATA;
DESCR                   return DESCR;
DIAGNOSIS               return DIAGNOSIS;
DOCNAME                 return DOCNAME;
HIDDEN                  return HIDDEN;
INCLUDE                 return INCLUDE;
INFO                    return INFO;
INIT                    return INIT;
NAME                    return NAME;
NODOCS                  return NODOCS;
PARENT                  return PARENT;
PATTERN                 return PATTERN;
PROLOG                  return PROLOG;
TYPE                    return TYPE;
UNIT                    return UNIT;


   /*--- Default rule -----------------------------------------------------*/

.                       return yytext[0];


   /*--- End-of-file rule (close include file) ----------------------------*/

<<EOF>>                 {
                           if (incFilename.empty())
                             yyterminate();

                           /* Close include file */
                           fclose(yyin);
                           yy_delete_buffer(YY_CURRENT_BUFFER);
                           yy_switch_to_buffer(incState);

                           lineno      = incLine;
                           incFilename = "";
                        }


%%


void include_file(const string& filename) {
  if (!incFilename.empty())
    yyerror("No nested includes allowed!");

  /* Set up state */
  incFilename = filename;
  incState    = YY_CURRENT_BUFFER;

  /* Open include file */
  yyin = fopen(incFilename.c_str(), "r");
  if (!yyin)
    yyerror("Could not open include file \"" + incFilename + "\"!");
  incLine = lineno;
  lineno  = 1;

  /* Switch input buffer */
  yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
}
