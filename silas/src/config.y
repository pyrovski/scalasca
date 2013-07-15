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


#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "HypBalance.h"
#include "HypCutMessage.h"
#include "HypScale.h"
#include "Model.h"
#include "ModelFactory.h"

using namespace std;
using namespace silas;


#define YYSTYPE std::string
#define YYDEBUG 1


/*--- Global variables ----------------------------------------------------*/

extern FILE* yyin;

long lineno = 1;

extern HypBalance* balancer;
extern HypCutMessage* message_surgeon;
extern HypScale* rescaler;

extern Model* model;

/*--- Function prototypes -------------------------------------------------*/

void yyerror(const string& message);
void parse_config(const std::string& filename);

extern int  yylex();

%}

%token SC_AGGREGATE_IDLE SC_BALANCE SC_IDLE_OVERHEAD SC_CUT SC_NOOP_THRESHOLD SC_OPTION SC_STRING SC_NUMBER SC_UNSIGNED SC_INTEGER SC_REAL SC_MESSAGE SC_MODEL SC_REGION SC_SCALE SC_SKIP_THRESHOLD SC_SIZE SC_TAG SC_RELATION

%%

Config              :   Model Hypothesis
                    ;

Model               :   SC_MODEL String  
                    {
                        model = silas::ModelFactory::create_model($2);
                        if (!model) yyerror("Undefined model!");
                    }   ModelOptions
                    ;

ModelOptions        :   ModelOption
                    |   ModelOptions ModelOption
                    ;

ModelOption         :   SC_OPTION String String
                    {
                        if (!model) yyerror("Uninitialized model!");

                        if ($3 == "true")
                            model->set_flag($2);
                        else
                            model->set_option($2, $3);
                    }
                    |   SC_AGGREGATE_IDLE
                    {
                        if (!model) yyerror("Uninitialized model!");
                        model->set_flag("aggregate idle");
                    }
                    |   SC_NOOP_THRESHOLD Number
                    {
                        if (!model) yyerror("Uninitialized model!");
                        model->set_option("noop threshold", $2);
                    }
                    |   SC_SKIP_THRESHOLD Number
                    {
                        if (!model) yyerror("Uninitialized model!");
                        model->set_option("skip threshold", $2);
                    }
                    |   SC_IDLE_OVERHEAD Number
                    {
                        if (!model) yyerror("Uninitialized model!");
                        model->set_option("idle overhead", $2);
                    }
                    |   /* epsilon */
                    ;

Hypothesis          :   HypothesisItem
                    |   Hypothesis HypothesisItem
                    ;

HypothesisItem      :   Balance
                    |   Cut
                    |   Scale
                    |   /* epsilon */
                    ;

Region              :   SC_REGION String
                    {
                        $$ = $2;
                    }
Balance             :   SC_BALANCE Region
                    {
                        balancer->register_region($2);
                    }
                    ;

Cut                 :   SC_CUT Region
                    {
                        /*
                        surgeon->register_region($2);
                        */
                    }
                    |   SC_CUT SC_MESSAGE MessageOption
                    ;

MessageOption       :   SC_SIZE Relation SC_UNSIGNED
                    {
                        unsigned int value = strtol($3.c_str(), NULL, 0);
                        message_surgeon->register_message_size($2, value);
                    }
                    |   SC_TAG Relation SC_UNSIGNED
                    {
                        unsigned int value = strtol($3.c_str(), NULL, 0);

                        message_surgeon->register_message_tag($2, value);
                    }
                    |   /* epsilon */
                    ;

Scale               :   SC_SCALE Region Number
                    {
                        double value = strtod($3.c_str(), NULL);
                        
                        rescaler->register_region($2.c_str(),value);
                    }
                    ;

Relation            :   SC_RELATION
                    {
                        $$ = $1;
                    }

String              :   '"' SC_STRING '"'
                    {
                        $$ = $2;
                    }
                    ;

Number              :   SC_UNSIGNED
                    {
                       $$ = $1;
                    }
                    |   SC_INTEGER
                    {
                       $$ = $1;
                    }
                    |   SC_REAL
                    {
                       $$ = $1;
                    }
                    ;

%%


void yyerror(const string& message)
{
  std::cerr << "Line " << lineno << ": " << message << std::endl; 

  exit(EXIT_FAILURE);
}

void parse_config(const std::string& filename)
{
  /* Open input file */
  yyin = fopen(filename.c_str(), "r");
  if (!yyin) {
    fprintf(stderr, "Could not open file \"%s\".\n", filename.c_str());
    exit(EXIT_FAILURE);
  }

  /* Parse input file */
  yyparse();

  /* Close input file */
  fclose(yyin);
}
