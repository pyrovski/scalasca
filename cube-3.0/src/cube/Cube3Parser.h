
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

#ifndef PARSER3_HEADER_H
# define PARSER3_HEADER_H

/* "%code requires" blocks.  */

/* Line 35 of lalr1.cc  */
#line 1 "Cube3Parser.yy"
 /*** C/C++ Declarations ***/

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>
#include "services.h"
#include "ParseContext.h"
#include "Cartesian.h"




/* Line 35 of lalr1.cc  */
#line 59 "Cube3Parser.h"


#include <string>
#include <iostream>
#include "stack.hh"


/* Line 35 of lalr1.cc  */
#line 1 "[Bison:b4_percent_define_default]"

namespace cubeparser {

/* Line 35 of lalr1.cc  */
#line 73 "Cube3Parser.h"
  class position;
  class location;

/* Line 35 of lalr1.cc  */
#line 1 "[Bison:b4_percent_define_default]"

} // cubeparser

/* Line 35 of lalr1.cc  */
#line 83 "Cube3Parser.h"

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
#line 131 "Cube3Parser.h"

  /// A Bison parser.
  class Cube3Parser
  {
  public:
    /// Symbol semantic values.
#ifndef YYSTYPE
    union semantic_type
    {

/* Line 35 of lalr1.cc  */
#line 58 "Cube3Parser.yy"
 


/* Line 35 of lalr1.cc  */
#line 148 "Cube3Parser.h"
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
     ATTRIBUTE_VALUE = 260,
     ERROR = 261,
     OPENTAG_RIGHT = 262,
     CLOSETAG_RIGHT = 263,
     ATTR_OPEN = 264,
     DIM_OPEN = 265,
     XML_OPEN = 266,
     XML_CLOSE = 267,
     CUBE_OPEN3 = 268,
     CUBE_OPEN2 = 269,
     CUBE_CLOSE = 270,
     METRIC_OPEN = 271,
     METRIC_CLOSE = 272,
     REGION_OPEN = 273,
     REGION_CLOSE = 274,
     CNODE_OPEN = 275,
     CNODE_CLOSE = 276,
     MACHINE_OPEN = 277,
     MACHINE_CLOSE = 278,
     NODE_OPEN = 279,
     NODE_CLOSE = 280,
     PROCESS_OPEN = 281,
     PROCESS_CLOSE = 282,
     THREAD_OPEN = 283,
     THREAD_CLOSE = 284,
     MATRIX_OPEN = 285,
     MATRIX_CLOSE = 286,
     ROW_OPEN = 287,
     ROW_CLOSE = 288,
     CART_OPEN = 289,
     CART_CLOSE = 290,
     COORD_OPEN = 291,
     COORD_CLOSE = 292,
     DOC_OPEN = 293,
     DOC_CLOSE = 294,
     MIRRORS_OPEN = 295,
     MIRRORS_CLOSE = 296,
     MURL_OPEN = 297,
     MURL_CLOSE = 298,
     METRICS_OPEN = 299,
     METRICS_CLOSE = 300,
     DISP_NAME_OPEN = 301,
     DISP_NAME_CLOSE = 302,
     UNIQ_NAME_OPEN = 303,
     UNIQ_NAME_CLOSE = 304,
     DTYPE_OPEN = 305,
     DTYPE_CLOSE = 306,
     UOM_OPEN = 307,
     UOM_CLOSE = 308,
     VAL_OPEN = 309,
     VAL_CLOSE = 310,
     URL_OPEN = 311,
     URL_CLOSE = 312,
     DESCR_OPEN = 313,
     DESCR_CLOSE = 314,
     PROGRAM_OPEN = 315,
     PROGRAM_CLOSE = 316,
     NAME_OPEN = 317,
     NAME_CLOSE = 318,
     SYSTEM_OPEN = 319,
     SYSTEM_CLOSE = 320,
     RANK_OPEN = 321,
     RANK_CLOSE = 322,
     TOPOLOGIES_OPEN = 323,
     TOPOLOGIES_CLOSE = 324,
     SEVERITY_OPEN = 325,
     SEVERITY_CLOSE = 326,
     ANAME_VERSION = 327,
     ANAME_ENCODING = 328,
     ANAME_KEY = 329,
     ANAME_VALUE = 330,
     ANAME_ID = 331,
     ANAME_MOD = 332,
     ANAME_BEGIN = 333,
     ANAME_END = 334,
     ANAME_LINE = 335,
     ANAME_CALLEEID = 336,
     ANAME_NAME = 337,
     ANAME_NDIMS = 338,
     ANAME_SIZE = 339,
     ANAME_PERIODIC = 340,
     ANAME_THRDID = 341,
     ANAME_PROCID = 342,
     ANAME_NODEID = 343,
     ANAME_MACHID = 344,
     ANAME_METRICID = 345,
     ANAME_CNODEID = 346
   };

    };
    /// Token type.
    typedef token::yytokentype token_type;

    /// Build a parser object.
    Cube3Parser (class Driver& driver_yyarg, class ParseContext& parseContext_yyarg, class Cube3Scanner& cube3Lexer_yyarg, class cube::Cube& cube_yyarg);
    virtual ~Cube3Parser ();

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
    static const short int yypact_ninf_;

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
    static const unsigned short int yytable_[];
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
    class Cube3Scanner& cube3Lexer;
    class cube::Cube& cube;
  };

/* Line 35 of lalr1.cc  */
#line 1 "[Bison:b4_percent_define_default]"

} // cubeparser

/* Line 35 of lalr1.cc  */
#line 429 "Cube3Parser.h"



#endif /* ! defined PARSER3_HEADER_H */
