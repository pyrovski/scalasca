
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison LALR(1) parsers in C++
   
      Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007, 2008 Free Software
   Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C++ LALR(1) parser skeleton written by Akim Demaille.  */

#ifndef PARSER2_HEADER_H
# define PARSER2_HEADER_H

/* "%code requires" blocks.  */

/* Line 35 of lalr1.cc  */
#line 1 "Cube2Parser.yy"
 /*** C/C++ Declarations ***/

#include <cmath>
#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "ParseContext.h"




/* Line 35 of lalr1.cc  */
#line 60 "Cube2Parser.h"


#include <string>
#include <iostream>
#include "stack.hh"


/* Line 35 of lalr1.cc  */
#line 1 "[Bison:b4_percent_define_default]"

namespace cubeparser {

/* Line 35 of lalr1.cc  */
#line 74 "Cube2Parser.h"
  class position;
  class location;

/* Line 35 of lalr1.cc  */
#line 1 "[Bison:b4_percent_define_default]"

} // cubeparser

/* Line 35 of lalr1.cc  */
#line 84 "Cube2Parser.h"

#include "location.hh"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
do {							\
  if (N)						\
    {							\
      (Current).begin = (Rhs)[1].begin;			\
      (Current).end   = (Rhs)[N].end;			\
    }							\
  else							\
    {							\
      (Current).begin = (Current).end = (Rhs)[0].end;	\
    }							\
} while (false)
#endif


/* Line 35 of lalr1.cc  */
#line 1 "[Bison:b4_percent_define_default]"

namespace cubeparser {

/* Line 35 of lalr1.cc  */
#line 132 "Cube2Parser.h"

  /// A Bison parser.
  class Cube2Parser
  {
  public:
    /// Symbol semantic values.
#ifndef YYSTYPE
    union semantic_type
    {

/* Line 35 of lalr1.cc  */
#line 61 "Cube2Parser.yy"
 



/* Line 35 of lalr1.cc  */
#line 150 "Cube2Parser.h"
    };
#else
    typedef YYSTYPE semantic_type;
#endif
    /// Symbol locations.
    typedef location location_type;
    /// Tokens.
    struct token
    {
      /* Tokens.  */
   enum yytokentype {
     END = 0,
     FFALSE = 258,
     TTRUE = 259,
     CLOSETAG_RIGHT = 260,
     XML_CLOSE = 261,
     OPENTAG_RIGHT = 262,
     XML_OPEN = 263,
     ATTR_OPEN = 264,
     DIM_OPEN = 265,
     CUBE_OPEN2 = 266,
     CUBE_CLOSE = 267,
     METRIC_OPEN = 268,
     METRIC_CLOSE = 269,
     CNODE_OPEN = 270,
     CNODE_CLOSE = 271,
     MACHINE_OPEN = 272,
     MACHINE_CLOSE = 273,
     NODE_OPEN = 274,
     NODE_CLOSE = 275,
     PROCESS_OPEN = 276,
     PROCESS_CLOSE = 277,
     THREAD_OPEN = 278,
     THREAD_CLOSE = 279,
     CART_OPEN = 280,
     CART_CLOSE = 281,
     COORD_OPEN = 282,
     COORD_CLOSE = 283,
     MATRIX_OPEN = 284,
     MATRIX_CLOSE = 285,
     ROW_OPEN = 286,
     ROW_CLOSE = 287,
     MODULE_OPEN = 288,
     MODULE_CLOSE = 289,
     REGION_OPEN = 290,
     REGION_CLOSE = 291,
     CSITE_OPEN = 292,
     CSITE_CLOSE = 293,
     BEGIN_OPEN = 294,
     BEGIN_CLOSE = 295,
     END_OPEN = 296,
     END_CLOSE = 297,
     MOD_OPEN = 298,
     MOD_CLOSE = 299,
     LINE_OPEN = 300,
     LINE_CLOSE = 301,
     CALLEE_OPEN = 302,
     CALLEE_CLOSE = 303,
     DOC_OPEN = 304,
     DOC_CLOSE = 305,
     MIRRORS_OPEN = 306,
     MIRRORS_CLOSE = 307,
     MURL_OPEN = 308,
     MURL_CLOSE = 309,
     BEHAVIOR_OPEN = 310,
     BEHAVIOR_CLOSE = 311,
     NAME_OPEN = 312,
     NAME_CLOSE = 313,
     DTYPE_OPEN = 314,
     DTYPE_CLOSE = 315,
     UOM_OPEN = 316,
     UOM_CLOSE = 317,
     VAL_OPEN = 318,
     VAL_CLOSE = 319,
     URL_OPEN = 320,
     URL_CLOSE = 321,
     DESCR_OPEN = 322,
     DESCR_CLOSE = 323,
     PROGRAM_OPEN = 324,
     PROGRAM_CLOSE = 325,
     LOCATIONS_OPEN = 326,
     LOCATIONS_CLOSE = 327,
     RANK_OPEN = 328,
     RANK_CLOSE = 329,
     TOPOLOGIES_OPEN = 330,
     TOPOLOGIES_CLOSE = 331,
     SEVERITY_OPEN = 332,
     SEVERITY_CLOSE = 333,
     ANAME_VERSION = 334,
     ANAME_ENCODING = 335,
     ANAME_KEY = 336,
     ANAME_VALUE = 337,
     ANAME_ID = 338,
     ANAME_CSITEID = 339,
     ANAME_LOCID = 340,
     ANAME_NDIMS = 341,
     ANAME_SIZE = 342,
     ANAME_PERIODIC = 343,
     ANAME_METRICID = 344,
     ANAME_CNODEID = 345,
     ATTRIBUTE_VALUE = 346,
     ERROR = 347
   };

    };
    /// Token type.
    typedef token::yytokentype token_type;

    /// Build a parser object.
    Cube2Parser (class Driver& driver_yyarg, class ParseContext& parseContext_yyarg, class Cube2Scanner& cube2Lexer_yyarg, class cube::Cube& cube_yyarg);
    virtual ~Cube2Parser ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

  private:
    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Generate an error message.
    /// \param state   the state where the error occurred.
    /// \param tok     the lookahead token.
    virtual std::string yysyntax_error_ (int yystate, int tok);

#if YYDEBUG
    /// \brief Report a symbol value on the debug stream.
    /// \param yytype       The token type.
    /// \param yyvaluep     Its semantic value.
    /// \param yylocationp  Its location.
    virtual void yy_symbol_value_print_ (int yytype,
					 const semantic_type* yyvaluep,
					 const location_type* yylocationp);
    /// \brief Report a symbol on the debug stream.
    /// \param yytype       The token type.
    /// \param yyvaluep     Its semantic value.
    /// \param yylocationp  Its location.
    virtual void yy_symbol_print_ (int yytype,
				   const semantic_type* yyvaluep,
				   const location_type* yylocationp);
#endif


    /// State numbers.
    typedef int state_type;
    /// State stack type.
    typedef stack<state_type>    state_stack_type;
    /// Semantic value stack type.
    typedef stack<semantic_type> semantic_stack_type;
    /// location stack type.
    typedef stack<location_type> location_stack_type;

    /// The state stack.
    state_stack_type yystate_stack_;
    /// The semantic value stack.
    semantic_stack_type yysemantic_stack_;
    /// The location stack.
    location_stack_type yylocation_stack_;

    /// Internal symbol numbers.
    typedef unsigned char token_number_type;
    /* Tables.  */
    /// For a state, the index in \a yytable_ of its portion.
    static const short int yypact_[];
    static const signed char yypact_ninf_;

    /// For a state, default rule to reduce.
    /// Unless\a  yytable_ specifies something else to do.
    /// Zero means the default is an error.
    static const unsigned char yydefact_[];

    static const short int yypgoto_[];
    static const short int yydefgoto_[];

    /// What to do in a state.
    /// \a yytable_[yypact_[s]]: what to do in state \a s.
    /// - if positive, shift that token.
    /// - if negative, reduce the rule which number is the opposite.
    /// - if zero, do what YYDEFACT says.
    static const unsigned char yytable_[];
    static const signed char yytable_ninf_;

    static const short int yycheck_[];

    /// For a state, its accessing symbol.
    static const unsigned char yystos_[];

    /// For a rule, its LHS.
    static const unsigned char yyr1_[];
    /// For a rule, its RHS length.
    static const unsigned char yyr2_[];

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
    /// For a symbol, its name in clear.
    static const char* const yytname_[];
#endif

#if YYERROR_VERBOSE
    /// Convert the symbol name \a n to a form suitable for a diagnostic.
    virtual std::string yytnamerr_ (const char *n);
#endif

#if YYDEBUG
    /// A type to store symbol numbers and -1.
    typedef short int rhs_number_type;
    /// A `-1'-separated list of the rules' RHS.
    static const rhs_number_type yyrhs_[];
    /// For each rule, the index of the first RHS symbol in \a yyrhs_.
    static const unsigned short int yyprhs_[];
    /// For each rule, its source line number.
    static const unsigned short int yyrline_[];
    /// For each scanner token number, its symbol number.
    static const unsigned short int yytoken_number_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r);
    /// Print the state stack on the debug stream.
    virtual void yystack_print_ ();

    /* Debugging.  */
    int yydebug_;
    std::ostream* yycdebug_;
#endif

    /// Convert a scanner token number \a t to a symbol number.
    token_number_type yytranslate_ (int t);

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg        Why this token is reclaimed.
    /// \param yytype       The symbol type.
    /// \param yyvaluep     Its semantic value.
    /// \param yylocationp  Its location.
    inline void yydestruct_ (const char* yymsg,
			     int yytype,
			     semantic_type* yyvaluep,
			     location_type* yylocationp);

    /// Pop \a n symbols the three stacks.
    inline void yypop_ (unsigned int n = 1);

    /* Constants.  */
    static const int yyeof_;
    /* LAST_ -- Last index in TABLE_.  */
    static const int yylast_;
    static const int yynnts_;
    static const int yyempty_;
    static const int yyfinal_;
    static const int yyterror_;
    static const int yyerrcode_;
    static const int yyntokens_;
    static const unsigned int yyuser_token_number_max_;
    static const token_number_type yyundef_token_;

    /* User arguments.  */
    class Driver& driver;
    class ParseContext& parseContext;
    class Cube2Scanner& cube2Lexer;
    class cube::Cube& cube;
  };

/* Line 35 of lalr1.cc  */
#line 1 "[Bison:b4_percent_define_default]"

} // cubeparser

/* Line 35 of lalr1.cc  */
#line 432 "Cube2Parser.h"



#endif /* ! defined PARSER2_HEADER_H */
