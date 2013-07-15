
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++
   
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

// Take the name prefix into account.
#define yylex   cubeparserlex

/* First part of user declarations.  */


/* Line 311 of lalr1.cc  */
#line 43 "Cube3Parser.c"


#include "Cube3Parser.h"

/* User implementation prologue.  */

/* Line 317 of lalr1.cc  */
#line 157 "Cube3Parser.yy"


#include "Driver.h"
#include "Cube3Scanner.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stdlib.h>

using namespace std;

#include "Metric.h"
#include "Cnode.h"

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex cube3Lexer.lex

// Workaround for Sun Studio C++ compilers on Solaris
#if defined(__SVR4) &&  defined(__SUNPRO_CC)
  #include <ieeefp.h>

  #define isinf(x)  (fpclass(x) == FP_NINF || fpclass(x) == FP_PINF)
  #define isnan(x)  isnand(x)
#endif



/* Line 317 of lalr1.cc  */
#line 83 "Cube3Parser.c"

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* Enable debugging if requested.  */
#if YYDEBUG

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_REDUCE_PRINT(Rule)
# define YY_STACK_PRINT()

#endif /* !YYDEBUG */

#define yyerrok		(yyerrstatus_ = 0)
#define yyclearin	(yychar = yyempty_)

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


/* Line 380 of lalr1.cc  */
#line 1 "[Bison:b4_percent_define_default]"

namespace cubeparser {

/* Line 380 of lalr1.cc  */
#line 152 "Cube3Parser.c"
#if YYERROR_VERBOSE

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  Cube3Parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              /* Fall through.  */
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

#endif

  /// Build a parser object.
  Cube3Parser::Cube3Parser (class Driver& driver_yyarg, class ParseContext& parseContext_yyarg, class Cube3Scanner& cube3Lexer_yyarg, class cube::Cube& cube_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      driver (driver_yyarg),
      parseContext (parseContext_yyarg),
      cube3Lexer (cube3Lexer_yyarg),
      cube (cube_yyarg)
  {
  }

  Cube3Parser::~Cube3Parser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  Cube3Parser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    switch (yytype)
      {
         default:
	  break;
      }
  }


  void
  Cube3Parser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif

  void
  Cube3Parser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
  
	default:
	  break;
      }
  }

  void
  Cube3Parser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  Cube3Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  Cube3Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  Cube3Parser::debug_level_type
  Cube3Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  Cube3Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif

  int
  Cube3Parser::parse ()
  {
    /// Lookahead and lookahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    /* State.  */
    int yyn;
    int yylen = 0;
    int yystate = 0;

    /* Error handling.  */
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the lookahead.
    semantic_type yylval;
    /// Location of the lookahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location_type yyerror_range[2];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    YYCDEBUG << "Starting parse" << std::endl;


    /* User initialization code.  */
    
/* Line 553 of lalr1.cc  */
#line 40 "Cube3Parser.yy"
{
    // initialize the initial location object
    yylloc.begin.filename = yylloc.end.filename = &driver.streamname;
}

/* Line 553 of lalr1.cc  */
#line 339 "Cube3Parser.c"

    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_ = state_stack_type (0);
    yysemantic_stack_ = semantic_stack_type (0);
    yylocation_stack_ = location_stack_type (0);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;

    /* Accept?  */
    if (yystate == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without lookahead.  */
    yyn = yypact_[yystate];
    if (yyn == yypact_ninf_)
      goto yydefault;

    /* Read a lookahead token.  */
    if (yychar == yyempty_)
      {
	YYCDEBUG << "Reading a token: ";
	yychar = yylex (&yylval, &yylloc);
      }


    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yyn == 0 || yyn == yytable_ninf_)
	goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted.  */
    yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
	  case 2:

/* Line 678 of lalr1.cc  */
#line 196 "Cube3Parser.yy"
    { 
	if (parseContext.versionSeen)
	  error(yylloc,"Multiple version attributes defines!"); 
	else parseContext.versionSeen = true;
	parseContext.version = parseContext.str.str(); 
}
    break;

  case 3:

/* Line 678 of lalr1.cc  */
#line 203 "Cube3Parser.yy"
    { 
	if (parseContext.encodingSeen)
	  error(yylloc,"Multiple encoding attributes defines!"); 
	else parseContext.encodingSeen = true;
	parseContext.encoding = parseContext.str.str(); 
}
    break;

  case 4:

/* Line 678 of lalr1.cc  */
#line 210 "Cube3Parser.yy"
    { 
	if (parseContext.keySeen)
	  error(yylloc,"Multiple key attributes defines!"); 
	else parseContext.keySeen = true;
	parseContext.key = parseContext.str.str(); 
}
    break;

  case 5:

/* Line 678 of lalr1.cc  */
#line 217 "Cube3Parser.yy"
    { 
	if (parseContext.valueSeen)
	  error(yylloc,"Multiple value attributes defines!"); 
	else parseContext.valueSeen = true;	
	parseContext.value = parseContext.str.str(); 
}
    break;

  case 6:

/* Line 678 of lalr1.cc  */
#line 224 "Cube3Parser.yy"
    {  
	if (parseContext.modSeen)
	  error(yylloc,"Multiple module attributes defines!"); 
	else parseContext.modSeen = true;
	parseContext.mod = parseContext.str.str(); }
    break;

  case 7:

/* Line 678 of lalr1.cc  */
#line 232 "Cube3Parser.yy"
    {  
    if (parseContext.cartNameSeen)
      error(yylloc,"Multiple topology names  defines!"); 
    else parseContext.cartNameSeen = true;
    parseContext.cartName = parseContext.str.str(); 
}
    break;

  case 8:

/* Line 678 of lalr1.cc  */
#line 242 "Cube3Parser.yy"
    {
	if (parseContext.dimNameSeen) 
	  error(yylloc,"Multiple ndim attributes defines!"); 
	else parseContext.dimNameSeen = true;
	  parseContext.dimNameVec.push_back(parseContext.str.str()); 
}
    break;

  case 9:

/* Line 678 of lalr1.cc  */
#line 253 "Cube3Parser.yy"
    { 
	if (parseContext.idSeen) 
	  error(yylloc,"Multiple id attributes defines!"); 
	else parseContext.idSeen = true;
	if (parseContext.longAttribute<0) error(yylloc,"Ids must be non-negative!"); 
	parseContext.id = (int)parseContext.longAttribute; 
}
    break;

  case 10:

/* Line 678 of lalr1.cc  */
#line 262 "Cube3Parser.yy"
    { 
	if (parseContext.calleeidSeen) 
	  error(yylloc,"Multiple callee id attributes defines!"); 
	else parseContext.calleeidSeen = true;
	if (parseContext.longAttribute<0) 
	  error(yylloc,"Callee ids of regions must be non-negative!"); 
	parseContext.calleeid = (int)parseContext.longAttribute; 
}
    break;

  case 11:

/* Line 678 of lalr1.cc  */
#line 272 "Cube3Parser.yy"
    { 
	if (parseContext.thrdidSeen) 
	  error(yylloc,"Multiple thread id attributes defines!"); 
	else parseContext.thrdidSeen = true;
	if (parseContext.longAttribute<0) error(yylloc,"Thread ids must be non-negative!"); 
	parseContext.thrdid = (int)parseContext.longAttribute; 
}
    break;

  case 12:

/* Line 678 of lalr1.cc  */
#line 281 "Cube3Parser.yy"
    { 
	if (parseContext.procidSeen) 
	  error(yylloc,"Multiple process id attributes defines!"); 
	else parseContext.procidSeen = true;
	if (parseContext.longAttribute<0) error(yylloc,"Process ids must be non-negative!"); 
	parseContext.procid = (int)parseContext.longAttribute; 
}
    break;

  case 13:

/* Line 678 of lalr1.cc  */
#line 290 "Cube3Parser.yy"
    { 
	if (parseContext.nodeidSeen) 
	  error(yylloc,"Multiple node id attributes defines!"); 
	else parseContext.nodeidSeen = true;
	if (parseContext.longAttribute<0) error(yylloc,"Node ids must be non-negative!"); 
	parseContext.nodeid = (int)parseContext.longAttribute; 
}
    break;

  case 14:

/* Line 678 of lalr1.cc  */
#line 299 "Cube3Parser.yy"
    { 
	if (parseContext.machidSeen) 
	  error(yylloc,"Multiple machine id attributes defines!"); 
	else parseContext.machidSeen = true;
	if (parseContext.longAttribute<0) error(yylloc,"Machine ids must be non-negative!"); 
	parseContext.machid = (int)parseContext.longAttribute; 
}
    break;

  case 15:

/* Line 678 of lalr1.cc  */
#line 308 "Cube3Parser.yy"
    { 
	if (parseContext.metricidSeen) 
	  error(yylloc,"Multiple metric id attributes defines!"); 
	else parseContext.metricidSeen = true;
	if (parseContext.longAttribute<0) error(yylloc,"Metric ids must be non-negative!"); 
	parseContext.metricid = parseContext.longAttribute; 
}
    break;

  case 16:

/* Line 678 of lalr1.cc  */
#line 317 "Cube3Parser.yy"
    { 
	if (parseContext.cnodeidSeen) 
	  error(yylloc,"Multiple cnode id attributes defines!"); 
	else parseContext.cnodeidSeen = true;
	if (parseContext.longAttribute<0) error(yylloc,"Cnode ids must be non-negative!"); 
	parseContext.cnodeid = (int)parseContext.longAttribute; 
}
    break;

  case 17:

/* Line 678 of lalr1.cc  */
#line 328 "Cube3Parser.yy"
    { 
	if (parseContext.beginSeen) 
	  error(yylloc,"Multiple begin attributes defines!"); 
	else parseContext.beginSeen = true;
	parseContext.beginln = parseContext.longAttribute; }
    break;

  case 18:

/* Line 678 of lalr1.cc  */
#line 336 "Cube3Parser.yy"
    { 
	if (parseContext.endSeen) 
	  error(yylloc,"Multiple end attributes defines!"); 
	else parseContext.endSeen = true;
	parseContext.endln = parseContext.longAttribute; }
    break;

  case 19:

/* Line 678 of lalr1.cc  */
#line 344 "Cube3Parser.yy"
    { 
	if (parseContext.lineSeen) 
	  error(yylloc,"Multiple line attributes defines!"); 
	else parseContext.lineSeen = true;
	parseContext.line = parseContext.longAttribute; }
    break;

  case 20:

/* Line 678 of lalr1.cc  */
#line 353 "Cube3Parser.yy"
    {
	if (parseContext.ndimsSeen) 
	  error(yylloc,"Multiple ndims attributes defines!"); 
	else parseContext.ndimsSeen = true;
	if (parseContext.longAttribute<=0)
	   error(yylloc,"Topology dimensions must be positive numbers!");
	parseContext.ndims = parseContext.longAttribute;
}
    break;

  case 21:

/* Line 678 of lalr1.cc  */
#line 365 "Cube3Parser.yy"
    { 
	if (parseContext.sizeSeen) 
	  error(yylloc,"Multiple size attributes defines!"); 
	else parseContext.sizeSeen = true;
	if (parseContext.longAttribute<=0)
	  error(yylloc,"Dimension sizes must be positive numbers!");
	parseContext.dimVec.push_back(parseContext.longAttribute);
}
    break;

  case 22:

/* Line 678 of lalr1.cc  */
#line 377 "Cube3Parser.yy"
    { 
	if (parseContext.periodicSeen) 
	  error(yylloc,"Multiple periodic attributes defines!"); 
	else parseContext.periodicSeen = true;
	parseContext.periodicVec.push_back(false);
}
    break;

  case 23:

/* Line 678 of lalr1.cc  */
#line 383 "Cube3Parser.yy"
    { 
	if (parseContext.periodicSeen) 
	  error(yylloc,"Multiple periodic attributes defined!");
	else parseContext.periodicSeen = true;
	parseContext.periodicVec.push_back(true);
}
    break;

  case 24:

/* Line 678 of lalr1.cc  */
#line 394 "Cube3Parser.yy"
    {
	if (parseContext.murlSeen) 
	  error(yylloc,"Multiple murl tags defined!"); 
	else parseContext.murlSeen = true;
	parseContext.murl = parseContext.str.str(); }
    break;

  case 25:

/* Line 678 of lalr1.cc  */
#line 400 "Cube3Parser.yy"
    { 
	if (parseContext.dispnameSeen) 
	  error(yylloc,"Multiple disp_name tags defined!"); 
	else parseContext.dispnameSeen = true;
	parseContext.disp_name = parseContext.str.str(); }
    break;

  case 26:

/* Line 678 of lalr1.cc  */
#line 406 "Cube3Parser.yy"
    { 
	if (parseContext.uniqnameSeen) 
	  error(yylloc,"Multiple uniq_name tags defined!"); 
	else parseContext.uniqnameSeen = true;
	parseContext.uniq_name = parseContext.str.str(); }
    break;

  case 27:

/* Line 678 of lalr1.cc  */
#line 412 "Cube3Parser.yy"
    { 
	if (parseContext.dtypeSeen) 
	  error(yylloc,"Multiple dtype tags defined!"); 
	else parseContext.dtypeSeen = true;
	parseContext.dtype = parseContext.str.str(); }
    break;

  case 28:

/* Line 678 of lalr1.cc  */
#line 419 "Cube3Parser.yy"
    {
	if (parseContext.uomSeen) 
	  error(yylloc,"Multiple uom tags defined!"); 
	else parseContext.uomSeen = true;
	parseContext.uom = parseContext.str.str(); }
    break;

  case 29:

/* Line 678 of lalr1.cc  */
#line 426 "Cube3Parser.yy"
    {
	if (parseContext.valSeen) 
	  error(yylloc,"Multiple val tags defined!"); 
	else parseContext.valSeen = true;
	parseContext.val = parseContext.str.str(); }
    break;

  case 30:

/* Line 678 of lalr1.cc  */
#line 433 "Cube3Parser.yy"
    {
	if (parseContext.urlSeen) 
	  error(yylloc,"Multiple url tags defined!");
	else parseContext.urlSeen = true;
	parseContext.url = parseContext.str.str(); }
    break;

  case 33:

/* Line 678 of lalr1.cc  */
#line 443 "Cube3Parser.yy"
    {
	if (parseContext.descrSeen) 
	  error(yylloc,"Multiple descr tags defined!");
	else parseContext.descrSeen = true;
	parseContext.descr = parseContext.str.str(); }
    break;

  case 34:

/* Line 678 of lalr1.cc  */
#line 450 "Cube3Parser.yy"
    {
	if (parseContext.nameSeen) 
	  error(yylloc,"Multiple name tags defined!"); 
	else parseContext.nameSeen = true;
	parseContext.name = parseContext.str.str(); }
    break;

  case 35:

/* Line 678 of lalr1.cc  */
#line 457 "Cube3Parser.yy"
    {
	if (parseContext.rankSeen) 
	  error(yylloc,"Multiple rank tags defined!"); 
	else parseContext.rankSeen = true;
	if (parseContext.longValues.size()==0)
	  error(yylloc,"No rank is given in a rank tag!");
	if (parseContext.longValues.size()>1)
	  error(yylloc,"Multiple ranks are given in a rank tag!");
	int rank = (int)parseContext.longValues[0];
	if (rank<0) 
	  error(yylloc,"Ranks must be non-negative!");
	parseContext.rank = rank; 
}
    break;

  case 37:

/* Line 678 of lalr1.cc  */
#line 480 "Cube3Parser.yy"
    {
	//check for attributes
	if (!parseContext.versionSeen)
 	  error(yylloc,"Missing version attribute!");
	else parseContext.versionSeen = false;
	if (!parseContext.encodingSeen)
	  error(yylloc,"Missing encoding attribute!");
	else parseContext.encodingSeen = false;
	int valid = strcmp(parseContext.version.c_str(),"1.0");
	if (valid!=0) error(yylloc,"XML version is expected to be 1.0!");
	valid = strcmp(parseContext.encoding.c_str(),"UTF-8");
	if (valid!=0) error(yylloc,"XML encoding is expected to be UTF-8!");
}
    break;

  case 43:

/* Line 678 of lalr1.cc  */
#line 512 "Cube3Parser.yy"
    {
	parseContext.cubeVersion = 3;
}
    break;

  case 44:

/* Line 678 of lalr1.cc  */
#line 515 "Cube3Parser.yy"
    {
	return 1;
}
    break;

  case 51:

/* Line 678 of lalr1.cc  */
#line 535 "Cube3Parser.yy"
    {
	//check for attributes
	if (!parseContext.keySeen)
	  error(yylloc,"Missing key attribute!"); 
	else parseContext.keySeen = false;
	if (!parseContext.valueSeen)
	  error(yylloc,"Missing value attribute!"); 
	else parseContext.valueSeen = false;
	cube.def_attr(services::escapeFromXML(parseContext.key),services::escapeFromXML(parseContext.value));
}
    break;

  case 62:

/* Line 678 of lalr1.cc  */
#line 575 "Cube3Parser.yy"
    {parseContext.murlSeen = false; cube.def_mirror(services::escapeFromXML(parseContext.murl));}
    break;

  case 63:

/* Line 678 of lalr1.cc  */
#line 576 "Cube3Parser.yy"
    {parseContext.murlSeen = false; cube.def_mirror(services::escapeFromXML(parseContext.murl));}
    break;

  case 68:

/* Line 678 of lalr1.cc  */
#line 593 "Cube3Parser.yy"
    {
	//check for attributes
	parseContext.idSeen = false;
	//check for tags
	if (parseContext.dispnameSeen) parseContext.dispnameSeen = false;
	else parseContext.disp_name = "";
	if (parseContext.uniqnameSeen) parseContext.uniqnameSeen = false;
	else parseContext.uniq_name = "";
	if (parseContext.dtypeSeen) parseContext.dtypeSeen = false;
	else parseContext.dtype = "";
	if (parseContext.uomSeen) parseContext.uomSeen = false;
	else parseContext.uom = "";
	if (parseContext.valSeen) parseContext.valSeen = false;
	else parseContext.val = "";
	if (parseContext.urlSeen) parseContext.urlSeen = false;
	else parseContext.url = "";
	if (parseContext.descrSeen) parseContext.descrSeen = false;
	else parseContext.descr = "";

	while ((int)parseContext.metricVec.size()<=parseContext.id) 
		parseContext.metricVec.push_back(NULL);
	if (parseContext.metricVec[parseContext.id]!=NULL)
	  error(yylloc,"Re-declared metric!");
	parseContext.currentMetric = 
		cube.def_met(
			services::escapeFromXML(parseContext.disp_name),
			services::escapeFromXML(parseContext.uniq_name),
			services::escapeFromXML(parseContext.dtype),
			services::escapeFromXML(parseContext.uom),
			services::escapeFromXML(parseContext.val),
			services::escapeFromXML(parseContext.url),
			services::escapeFromXML(parseContext.descr),
			parseContext.currentMetric,
			parseContext.id);
	parseContext.metricVec[parseContext.id] = parseContext.currentMetric;
}
    break;

  case 80:

/* Line 678 of lalr1.cc  */
#line 651 "Cube3Parser.yy"
    {
	if (parseContext.currentMetric==NULL)
	  error(yylloc,"Metric definitions are not correctly nested!");
	parseContext.currentMetric = 
		(parseContext.currentMetric)->get_parent();
}
    break;

  case 84:

/* Line 678 of lalr1.cc  */
#line 672 "Cube3Parser.yy"
    {
	//check for the attributes
	if (!parseContext.idSeen)
	  error(yylloc,"Missing id attribute!"); 
	else parseContext.idSeen = false;
	if (!parseContext.modSeen)
		parseContext.mod = "";
	else parseContext.modSeen = false;
	if (!parseContext.beginSeen)
		parseContext.beginln = -1;
	else parseContext.beginSeen = false;
	if (!parseContext.endSeen)
		parseContext.endln = -1;
	else parseContext.endSeen = false;

	//check for tags
	if (parseContext.nameSeen) parseContext.nameSeen = false;
	else parseContext.name = "";
	if (parseContext.urlSeen) parseContext.urlSeen = false;
	else parseContext.url = "";
	if (parseContext.descrSeen) parseContext.descrSeen = false;
	else parseContext.descr = "";
	
	while ((int)parseContext.regionVec.size()<=parseContext.id) 
		parseContext.regionVec.push_back(NULL);
	if (parseContext.regionVec[parseContext.id]!=NULL)
	  error(yylloc,"Re-declared region!"); 
	parseContext.regionVec[parseContext.id] = 
		cube.def_region(
			services::escapeFromXML(parseContext.name),
			parseContext.beginln,
			parseContext.endln,
			parseContext.url,
			services::escapeFromXML(parseContext.descr),
			services::escapeFromXML(parseContext.mod),
			parseContext.id);
}
    break;

  case 100:

/* Line 678 of lalr1.cc  */
#line 741 "Cube3Parser.yy"
    {
	if (parseContext.currentCnode==NULL)
	  error(yylloc,"Cnode definitions are not correctly nested!"); 
	parseContext.currentCnode = 
		parseContext.currentCnode->get_parent();
}
    break;

  case 101:

/* Line 678 of lalr1.cc  */
#line 750 "Cube3Parser.yy"
    {
	//check for the attributes 
	if (!parseContext.idSeen)
	  error(yylloc,"Missing id attribute in cnode definition!"); 
	else parseContext.idSeen = false;
	if (!parseContext.calleeidSeen)
	  error(yylloc,"Missing callee id attribute in cnode definition!"); 
	else parseContext.calleeidSeen = false;
	if (!parseContext.lineSeen)
	  	parseContext.line = -1;
	else parseContext.lineSeen = false;
	if (!parseContext.modSeen)
	  	parseContext.mod = "";
	else parseContext.modSeen = false;
	//check if the region is defined
	if ((int)parseContext.regionVec.size()<=parseContext.calleeid)
	  error(yylloc,"Undefined region in cnode definition!"); 
	if (parseContext.regionVec[parseContext.calleeid]==NULL)
	  error(yylloc,"Undefined region in cnode definition!"); 
	//extend the cnode vector if necessary
	while ((int)parseContext.cnodeVec.size()<=parseContext.id) 
		parseContext.cnodeVec.push_back(NULL);
	if (parseContext.cnodeVec[parseContext.id]!=NULL)
	  error(yylloc,"Re-declared cnode!");
	//define the cnode
	parseContext.currentCnode = cube.def_cnode(
			parseContext.regionVec[parseContext.calleeid],
			services::escapeFromXML(parseContext.mod),
			parseContext.line,
			parseContext.currentCnode,
			parseContext.id);
	parseContext.cnodeVec[parseContext.id] = 
		parseContext.currentCnode;
}
    break;

  case 111:

/* Line 678 of lalr1.cc  */
#line 810 "Cube3Parser.yy"
    {
	//check for attributes
	parseContext.idSeen = false;
	//check for tags
	parseContext.nameSeen = false;
	if (parseContext.descrSeen) parseContext.descrSeen = false;
	else parseContext.descr = "";

	parseContext.currentMachine = cube.def_mach(
		services::escapeFromXML(parseContext.name),
		services::escapeFromXML(parseContext.descr),
		parseContext.id);
	while ((int)parseContext.machVec.size()<=parseContext.id)
		parseContext.machVec.push_back(NULL);
	if (parseContext.machVec[parseContext.id]!=NULL)
	  error(yylloc,"Re-declared machine!"); 
	parseContext.machVec[parseContext.id] = parseContext.currentMachine;
}
    break;

  case 115:

/* Line 678 of lalr1.cc  */
#line 836 "Cube3Parser.yy"
    {
	//check for attributes
	parseContext.idSeen = false;
	//check for tags
	parseContext.nameSeen = false;
	
	parseContext.currentNode = cube.def_node(
		services::escapeFromXML(parseContext.name),
		parseContext.currentMachine,
		parseContext.id);
	while ((int)parseContext.nodeVec.size()<=parseContext.id)
		parseContext.nodeVec.push_back(NULL);
	if (parseContext.nodeVec[parseContext.id]!=NULL)
	  error(yylloc,"Re-declared node!");
	parseContext.nodeVec[parseContext.id] = parseContext.currentNode;
}
    break;

  case 119:

/* Line 678 of lalr1.cc  */
#line 860 "Cube3Parser.yy"
    {
	//check for attributes
	parseContext.idSeen = false;
	//check for tags
	if (parseContext.nameSeen) parseContext.nameSeen = false;
	else parseContext.name = "";
	if (parseContext.rankSeen) parseContext.rankSeen = false;
	else parseContext.rank = 0;

	std::ostringstream name;
	if (parseContext.name.empty()) {
	  name << "Process " << parseContext.rank;
	} else {
	  name << parseContext.name;
	}
	parseContext.currentProc = cube.def_proc(
		services::escapeFromXML(name.str()),
		parseContext.rank,
		parseContext.currentNode,
		parseContext.id);
	while ((int)parseContext.procVec.size()<=parseContext.id)
		parseContext.procVec.push_back(NULL);
	if (parseContext.procVec[parseContext.id]!=NULL)
	  error(yylloc,"Re-declared process!");
	parseContext.procVec[parseContext.id] = parseContext.currentProc;
}
    break;

  case 127:

/* Line 678 of lalr1.cc  */
#line 903 "Cube3Parser.yy"
    {
	//check for attributes
	parseContext.idSeen = false;
	//check for tags
	if (parseContext.nameSeen) parseContext.nameSeen = false;
	else parseContext.name = "";
	if (parseContext.rankSeen) parseContext.rankSeen = false;
	else parseContext.rank = 0;

	std::ostringstream name;
	if (parseContext.name.empty()) {
	  name << "Thread " << parseContext.rank;
	} else {
	  name << parseContext.name;
	}
	parseContext.currentThread = cube.def_thrd(
		services::escapeFromXML(name.str()),
		parseContext.rank,
		parseContext.currentProc,
		parseContext.id);
	while ((int)parseContext.threadVec.size()<=parseContext.id) 
		parseContext.threadVec.push_back(NULL);
	if (parseContext.threadVec[parseContext.id]!=NULL)
	  error(yylloc,"Re-declared thread!"); 
	parseContext.threadVec[parseContext.id] = parseContext.currentThread;
}
    break;

  case 130:

/* Line 678 of lalr1.cc  */
#line 936 "Cube3Parser.yy"
    {
	if (!parseContext.checkThreadIds())
	  error(yylloc,"Thread ids must cover an interval [0,n] without gap!"); 
}
    break;

  case 138:

/* Line 678 of lalr1.cc  */
#line 957 "Cube3Parser.yy"
    {
	//check for attributes
	parseContext.ndimsSeen = false;

	if (parseContext.ndims!=(int)parseContext.dimVec.size())
	  error(yylloc,"Too few or too many topology dimensions are declared!"); 
	if ((int)parseContext.dimNameVec.size() !=0 && (int)parseContext.dimNameVec.size() != parseContext.ndims) {
   	   error(yyloc, "Either all dimensions in one topology are named or none");
	   }	  
	parseContext.currentCart = cube.def_cart(
		parseContext.ndims,
		parseContext.dimVec,
		parseContext.periodicVec);
		
		if (parseContext.cartNameSeen)
            (parseContext.currentCart)->set_name(services::escapeFromXML(parseContext.cartName));
		if(parseContext.dimNameSeen) {
		  (parseContext.currentCart)->set_namedims(parseContext.dimNameVec);
		}
	    
		
}
    break;

  case 140:

/* Line 678 of lalr1.cc  */
#line 982 "Cube3Parser.yy"
    {parseContext.dimVec.clear();
	   parseContext.dimNameVec.clear();
	   parseContext.periodicVec.clear();
        parseContext.cartNameSeen=false;
}
    break;

  case 143:

/* Line 678 of lalr1.cc  */
#line 995 "Cube3Parser.yy"
    {

  	if(parseContext.dimNameSeen) {
				
				     }

	//check for attributes
	if (!parseContext.sizeSeen)
	  error(yylloc,"Missing size attribute!"); 
	else parseContext.sizeSeen = false;
	if (!parseContext.periodicSeen)
	  error(yylloc,"Missing periodic attribute!");
	else parseContext.periodicSeen = false;
  }
    break;

  case 144:

/* Line 678 of lalr1.cc  */
#line 1012 "Cube3Parser.yy"
    {
	parseContext.dimNameSeen=false;
}
    break;

  case 156:

/* Line 678 of lalr1.cc  */
#line 1041 "Cube3Parser.yy"
    {
	//check for attributes
	parseContext.thrdidSeen = false;

	if (parseContext.dimVec.size()!=parseContext.longValues.size())
	  error(yylloc,"Too few or too many dimension coordinates in coord tag!"); 
	for (unsigned i=0; i<parseContext.dimVec.size(); i++){
	    if (parseContext.longValues[i]>=parseContext.dimVec[i])
	  error(yylloc,"Topology coordinate is out of range!");
	}
	if (parseContext.thrdid>=(int)parseContext.threadVec.size()) 
	  error(yylloc,"Thread of the topology coordinates wasn't declared!"); 
	if (parseContext.threadVec[parseContext.thrdid]==NULL) 
	  error(yylloc,"Thread of the topology coordinates wasn't declared!"); 
	cube.def_coords(parseContext.currentCart,
                         (cube::Sysres*)(parseContext.threadVec[parseContext.thrdid]),
                         parseContext.longValues);
}
    break;

  case 157:

/* Line 678 of lalr1.cc  */
#line 1062 "Cube3Parser.yy"
    {
	//check for attributes
	parseContext.procidSeen = false;

	if (parseContext.dimVec.size()!=parseContext.longValues.size())
	  error(yylloc,"Too few or too many dimension coordinates in coord tag!"); 
	for (unsigned i=0; i<parseContext.dimVec.size(); i++){
	    if (parseContext.longValues[i]>=parseContext.dimVec[i])
	  error(yylloc,"Topology coordinate is out of range!"); 
	}
	if (parseContext.procid>=(int)parseContext.procVec.size()) 
	  error(yylloc,"Process of the topology coordinates wasn't declared!"); 
	if (parseContext.procVec[parseContext.procid]==NULL) 
	  error(yylloc,"Process of the topology coordinates wasn't declared!"); 
	cube.def_coords(parseContext.currentCart,
                         (cube::Sysres*)(parseContext.procVec[parseContext.procid]),
                         parseContext.longValues);
}
    break;

  case 158:

/* Line 678 of lalr1.cc  */
#line 1083 "Cube3Parser.yy"
    {
	//check for attributes
	parseContext.nodeidSeen = false;

	if (parseContext.dimVec.size()!=parseContext.longValues.size())
	  error(yylloc,"Too few or too many dimension coordinates in coord tag!"); 
	for (unsigned i=0; i<parseContext.dimVec.size(); i++){
	    if (parseContext.longValues[i]>=parseContext.dimVec[i])
	  error(yylloc,"Topology coordinate is out of range!"); 
	}
	if (parseContext.nodeid>=(int)parseContext.nodeVec.size()) 
	  error(yylloc,"Node of the topology coordinates wasn't declared!"); 
	if (parseContext.nodeVec[parseContext.nodeid]==NULL) 
	  error(yylloc,"Node of the topology coordinates wasn't declared!"); 
	cube.def_coords(parseContext.currentCart,
                         (cube::Sysres*)(parseContext.nodeVec[parseContext.nodeid]),
                         parseContext.longValues);
}
    break;

  case 159:

/* Line 678 of lalr1.cc  */
#line 1104 "Cube3Parser.yy"
    {
	//check for attributes
	parseContext.machidSeen = false;

	if (parseContext.dimVec.size()!=parseContext.longValues.size())
	  error(yylloc,"Too few or too many dimension coordinates in coord tag!"); 
	for (unsigned i=0; i<parseContext.dimVec.size(); i++){
	    if (parseContext.longValues[i]>=parseContext.dimVec[i])
	  error(yylloc,"Topology coordinate is out of range!"); 
	}
	if (parseContext.machid>=(int)parseContext.machVec.size()) 
	  error(yylloc,"Machine of the topology coordinates wasn't declared!"); 
	if (parseContext.machVec[parseContext.machid]==NULL) 
	  error(yylloc,"Machine of the topology coordinates wasn't declared!"); 
	cube.def_coords(parseContext.currentCart,
                         (cube::Sysres*)(parseContext.machVec[parseContext.machid]),
                         parseContext.longValues);
}
    break;

  case 160:

/* Line 678 of lalr1.cc  */
#line 1127 "Cube3Parser.yy"
    {if (parseContext.dynamicMetricLoading) return 0;}
    break;

  case 164:

/* Line 678 of lalr1.cc  */
#line 1137 "Cube3Parser.yy"
    {
	//check for attributes
	parseContext.metricidSeen = false;

	if ((int)parseContext.metricVec.size()<=parseContext.metricid)
	  error(yylloc,"Metric of the severity matrix wasn't declared!"); 
	if (parseContext.metricVec[parseContext.metricid]==NULL)
	  error(yylloc,"Metric of the severity matrix wasn't declared!");
	parseContext.currentMetric = 
		parseContext.metricVec[parseContext.metricid];

	parseContext.ignoreMetric = false;
        cube::Metric* metric = parseContext.currentMetric;
        while (metric!=NULL) {
    		if (metric->get_val() == "VOID") {
      		  parseContext.ignoreMetric = true;
      		  break;
    		}
    		metric = metric->get_parent();
  	}
}
    break;

  case 170:

/* Line 678 of lalr1.cc  */
#line 1170 "Cube3Parser.yy"
    {
	//check for attributes
	parseContext.cnodeidSeen = false;

	if (!parseContext.ignoreMetric){
		if ((int)parseContext.cnodeVec.size()<=parseContext.cnodeid)
		  error(yylloc,"Cnode of the severity row wasn't declared!");
		if (parseContext.cnodeVec[parseContext.cnodeid]==NULL)
		  error(yylloc,"Cnode of the severity row wasn't declared!"); 
		parseContext.currentCnode = 
			parseContext.cnodeVec[parseContext.cnodeid];
	}
}
    break;

  case 171:

/* Line 678 of lalr1.cc  */
#line 1182 "Cube3Parser.yy"
    {
	if (!parseContext.ignoreMetric){
	  	std::vector<double> & v = parseContext.realValues;
		std::vector<cube::Thread*> & t = parseContext.threadVec;
        	if (v.size()>t.size())	
                  error(yylloc,"Too many values in severity row!"); 
		cube::Metric* metric = parseContext.currentMetric;
		cube::Cnode * cnode = parseContext.currentCnode;
        	for (unsigned i=0; i<v.size(); i++){
            if (isinf(v[i]) || isnan(v[i])) {
              error(yylloc,"Malformed severity value (NAN or INF) in input file.");
            }
            else
            {
                if (v[i]!=0.0){
                        cube.set_sev(metric,
                            cnode,
                            t[i],
                            v[i]);
                       }
            }
        }
	}
}
    break;



/* Line 678 of lalr1.cc  */
#line 1432 "Cube3Parser.c"
	default:
          break;
      }
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	error (yylloc, yysyntax_error_ (yystate, yytoken));
      }

    yyerror_range[0] = yylloc;
    if (yyerrstatus_ == 3)
      {
	/* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

	if (yychar <= yyeof_)
	  {
	  /* Return failure if at end of input.  */
	  if (yychar == yyeof_)
	    YYABORT;
	  }
	else
	  {
	    yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
	    yychar = yyempty_;
	  }
      }

    /* Else will try to reuse lookahead token after shifting the error
       token.  */
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;

    yyerror_range[0] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (yyn != yypact_ninf_)
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	YYABORT;

	yyerror_range[0] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    yyerror_range[1] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the lookahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyempty_)
      yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval, &yylloc);

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (yystate_stack_.height () != 1)
      {
	yydestruct_ ("Cleanup: popping",
		   yystos_[yystate_stack_[0]],
		   &yysemantic_stack_[0],
		   &yylocation_stack_[0]);
	yypop_ ();
      }

    return yyresult;
  }

  // Generate an error message.
  std::string
  Cube3Parser::yysyntax_error_ (int yystate, int tok)
  {
    std::string res;
    YYUSE (yystate);
#if YYERROR_VERBOSE
    int yyn = yypact_[yystate];
    if (yypact_ninf_ < yyn && yyn <= yylast_)
      {
	/* Start YYX at -YYN if negative to avoid negative indexes in
	   YYCHECK.  */
	int yyxbegin = yyn < 0 ? -yyn : 0;

	/* Stay within bounds of both yycheck and yytname.  */
	int yychecklim = yylast_ - yyn + 1;
	int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
	int count = 0;
	for (int x = yyxbegin; x < yyxend; ++x)
	  if (yycheck_[x + yyn] == x && x != yyterror_)
	    ++count;

	// FIXME: This method of building the message is not compatible
	// with internationalization.  It should work like yacc.c does it.
	// That is, first build a string that looks like this:
	// "syntax error, unexpected %s or %s or %s"
	// Then, invoke YY_ on this string.
	// Finally, use the string as a format to output
	// yytname_[tok], etc.
	// Until this gets fixed, this message appears in English only.
	res = "syntax error, unexpected ";
	res += yytnamerr_ (yytname_[tok]);
	if (count < 5)
	  {
	    count = 0;
	    for (int x = yyxbegin; x < yyxend; ++x)
	      if (yycheck_[x + yyn] == x && x != yyterror_)
		{
		  res += (!count++) ? ", expecting " : " or ";
		  res += yytnamerr_ (yytname_[x]);
		}
	  }
      }
    else
#endif
      res = YY_("syntax error");
    return res;
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const short int Cube3Parser::yypact_ninf_ = -251;
  const short int
  Cube3Parser::yypact_[] =
  {
        20,   -12,    37,    55,    39,    54,  -251,  -251,    -8,  -251,
    -251,  -251,  -251,  -251,     1,  -251,  -251,  -251,  -251,    38,
     -27,    61,    72,     1,  -251,    51,    49,    92,   117,  -251,
    -251,     8,  -251,    84,    86,  -251,    52,    13,  -251,  -251,
    -251,    51,    49,    49,   109,    65,  -251,  -251,  -251,  -251,
      87,  -251,    90,    84,  -251,   127,   126,  -251,  -251,  -251,
    -251,    49,    65,    65,    24,    29,  -251,   112,    66,  -251,
    -251,  -251,  -251,    40,    65,    66,    66,   130,   132,   133,
    -251,  -251,  -251,  -251,    -4,  -251,   -14,  -251,   -13,  -251,
      52,   -16,  -251,  -251,  -251,    93,    94,    88,    89,    91,
      97,    82,  -251,  -251,  -251,  -251,  -251,  -251,  -251,  -251,
      40,  -251,    66,  -251,  -251,  -251,  -251,  -251,   -30,  -251,
     139,   140,  -251,  -251,  -251,  -251,  -251,     4,  -251,  -251,
    -251,   141,  -251,  -251,    85,  -251,  -251,  -251,  -251,  -251,
    -251,  -251,  -251,  -251,  -251,  -251,    98,  -251,  -251,  -251,
     128,   -30,  -251,  -251,  -251,  -251,  -251,   100,   115,  -251,
     -18,  -251,  -251,  -251,    99,  -251,   101,   115,  -251,    32,
      62,  -251,  -251,  -251,  -251,  -251,  -251,   146,   148,    73,
    -251,   151,   154,   153,   142,  -251,  -251,  -251,   155,  -251,
    -251,    52,    47,  -251,  -251,     5,  -251,    26,   131,   157,
    -251,  -251,  -251,  -251,   162,   163,   113,  -251,  -251,  -251,
      -6,  -251,    78,   143,   131,  -251,   100,    18,  -251,  -251,
    -251,  -251,  -251,  -251,   166,   165,  -251,  -251,  -251,   168,
     170,   171,   172,   173,   174,   175,   176,    83,  -251,  -251,
    -251,  -251,  -251,  -251,  -251,   152,  -251,  -251,  -251,  -251,
     147,   149,   150,   156,  -251,    18,   158,    52,    95,  -251,
    -251,  -251,  -251,  -251,  -251,  -251,   178,  -251,  -251,    27,
     121,  -251,  -251,    27,  -251,  -251,   161,  -251,    52,    96,
    -251,   183,  -251,  -251,    27,   -20,  -251
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned char
  Cube3Parser::yydefact_[] =
  {
         0,     0,     0,     0,     0,     0,    40,    41,     0,    38,
       1,    43,    44,    36,     0,     2,     3,    37,    39,     0,
      57,     0,     0,     0,    49,     0,     0,     0,     0,    54,
      55,     0,    52,    60,     0,    58,     0,     0,    65,    42,
      50,     0,     0,     0,     0,     0,     4,     5,    51,    53,
       0,    62,     0,    61,    56,     0,     0,    80,    64,    66,
      67,     0,     0,     0,     0,     0,    82,     0,     0,    24,
      59,    63,     9,    69,     0,     0,     0,     0,     0,     0,
      95,    94,    96,    97,     0,    92,     0,    83,     0,    98,
       0,   128,   109,   160,    48,     0,     0,     0,     0,     0,
       0,     0,    73,    74,    75,    76,    77,    78,    79,    68,
      70,    71,     0,    46,    47,     6,    17,    18,    85,    93,
       0,     0,   104,   102,   105,   103,   106,     0,   100,    81,
      99,     0,   130,   110,     0,   129,   162,    25,    26,    27,
      28,    29,    30,    33,    72,    45,     0,    90,    91,    89,
       0,    86,    87,    19,    10,   101,   107,     0,   132,   108,
       0,    34,    84,    88,    31,   140,     0,   133,   134,     0,
       0,   161,   163,   111,    32,   131,   135,     0,     0,     0,
     136,     0,     0,     0,     0,     7,    20,   137,     0,    15,
     164,     0,     0,   113,   144,     0,   141,     0,   166,     0,
     112,   114,   138,   142,     0,     0,     0,   147,   148,   149,
       0,   145,     0,     0,   167,   168,     0,     0,     8,    21,
      22,    23,   143,   146,     0,     0,   165,   169,   115,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   150,   152,
     153,   154,   155,    16,   170,     0,    11,    12,    13,    14,
       0,     0,     0,     0,   139,     0,     0,     0,     0,   117,
     156,   157,   158,   159,   151,   171,     0,   116,   118,     0,
       0,   123,   124,   119,   121,    35,     0,   122,     0,     0,
     125,     0,   120,   126,     0,     0,   127
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  Cube3Parser::yypgoto_[] =
  {
      -251,  -251,  -251,  -251,  -251,   -72,  -251,  -251,   -36,  -251,
    -251,  -251,  -251,  -251,  -251,  -251,  -251,  -251,  -251,     0,
    -251,  -251,   144,  -251,  -251,  -251,  -251,  -251,   -94,  -251,
    -113,  -117,  -251,  -251,  -251,  -251,   184,  -251,  -251,  -251,
    -251,   177,  -251,   164,   179,  -251,  -251,  -251,  -251,     2,
    -251,   159,  -251,  -251,   102,  -251,   -25,  -251,   129,  -251,
    -251,    48,  -251,   114,  -251,   116,    74,  -251,   -41,  -251,
     118,  -251,  -251,    11,  -251,  -251,   -53,  -251,   -78,  -250,
    -251,   -71,  -251,  -251,  -251,  -251,  -251,  -251,    43,  -251,
    -251,  -251,    12,  -251,  -251,     3,  -251,   -44,  -251,  -251,
    -251,  -251,   -56,  -251,  -251,  -251,  -251,  -251,  -251,     6,
    -251
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  Cube3Parser::yydefgoto_[] =
  {
        -1,     6,     7,    29,    30,    80,   179,   207,    81,   124,
     233,   234,   235,   236,   183,   225,    82,    83,   125,   180,
     208,   209,    51,   102,   103,   104,   105,   106,   107,   173,
     108,   271,   272,     2,     3,     8,     9,    13,    14,    22,
      23,    24,    31,    32,    25,    34,    35,    52,    53,    26,
      37,    38,   109,   110,   111,    60,    45,    65,    66,   150,
     151,   152,    84,    85,    88,    89,   126,   127,    68,    91,
      92,   184,   192,   193,   245,   258,   259,   276,   273,   274,
     279,   280,   134,   135,   158,   166,   167,   181,   168,   217,
     169,   195,   196,   197,   210,   211,   237,   238,   239,   240,
     241,   242,    94,   136,   160,   172,   198,   213,   214,   215,
     256
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const signed char Cube3Parser::yytable_ninf_ = -1;
  const unsigned short int
  Cube3Parser::yytable_[] =
  {
        56,   149,   222,   118,    17,   148,    90,    86,   128,   286,
      19,   155,   170,    33,   122,   194,    48,    62,    63,   113,
     114,    75,    76,   277,   147,    42,   100,    43,   101,    36,
      57,     1,   146,   112,   149,   277,    74,    10,   148,    20,
     164,   202,   146,    61,    15,    21,   270,    64,   129,    86,
     123,   174,   132,   171,   131,   122,   145,   147,    58,    16,
       4,     5,    55,    77,     4,     5,   120,   121,    11,    12,
     200,   191,    55,    77,    78,    79,   204,    36,   205,   206,
      55,    77,    27,    28,   120,   121,    95,    39,    96,   146,
      97,   123,    98,   270,    99,    21,   100,    46,   101,   228,
      55,    77,    78,    79,   229,   230,   231,   232,   204,    44,
     205,   206,    27,    28,   177,   178,   220,   221,   254,   255,
     267,   257,    47,   282,   278,    54,    50,    64,    55,    67,
      69,    70,    72,    73,    90,   115,    93,   116,   117,   139,
     137,   143,   140,   138,   153,   154,   141,   162,   157,   165,
     159,   185,   182,   186,   142,   199,   178,   101,   188,   189,
     190,   161,   146,   212,   216,   194,   191,   218,   219,   224,
     175,   243,   244,   246,   226,   247,   248,   249,   257,   187,
     250,   251,   252,   253,   260,   269,   261,   262,   275,   278,
     284,   265,    18,   263,    87,    49,    59,    71,   119,   163,
      40,   156,    41,   201,   130,   268,   285,   203,   283,   133,
     176,   264,   144,   223,     0,     0,     0,     0,     0,     0,
     227,   266,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   281
  };

  /* YYCHECK.  */
  const short int
  Cube3Parser::yycheck_[] =
  {
        36,   118,     8,     7,    12,   118,    22,    20,    21,    29,
       9,     7,    30,    40,    86,    10,     8,    42,    43,    75,
      76,    62,    63,   273,   118,    23,    56,    25,    58,    16,
      17,    11,    62,    74,   151,   285,    61,     0,   151,    38,
     157,    36,    62,    41,     5,    44,    66,    18,    61,    20,
      86,   164,    68,    71,    90,   127,   112,   151,    45,     5,
      72,    73,    76,    77,    72,    73,    80,    81,    13,    14,
      23,    24,    76,    77,    78,    79,    82,    16,    84,    85,
      76,    77,    74,    75,    80,    81,    46,    15,    48,    62,
      50,   127,    52,    66,    54,    44,    56,     5,    58,   216,
      76,    77,    78,    79,    86,    87,    88,    89,    82,    60,
      84,    85,    74,    75,    82,    83,     3,     4,    35,    36,
      25,    26,     5,    27,    28,    39,    42,    18,    76,    64,
      43,    41,     5,     7,    22,     5,    70,     5,     5,    51,
      47,    59,    53,    49,     5,     5,    55,    19,     7,    34,
      65,     5,    90,     5,    57,   191,    83,    58,     7,     5,
       7,    63,    62,    32,     7,    10,    24,     5,     5,    91,
      69,     5,     7,     5,    31,     5,     5,     5,    26,   179,
       7,     7,     7,     7,    37,     7,    37,    37,    67,    28,
       7,    33,     8,    37,    65,    31,    37,    53,    84,   151,
      23,   127,    23,   192,    88,   258,   284,   195,   279,    91,
     167,   255,   110,   210,    -1,    -1,    -1,    -1,    -1,    -1,
     214,   257,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   278
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  Cube3Parser::yystos_[] =
  {
         0,    11,   125,   126,    72,    73,    93,    94,   127,   128,
       0,    13,    14,   129,   130,     5,     5,    12,   128,     9,
      38,    44,   131,   132,   133,   136,   141,    74,    75,    95,
      96,   134,   135,    40,   137,   138,    16,   142,   143,    15,
     133,   136,   141,   141,    60,   148,     5,     5,     8,   135,
      42,   114,   139,   140,    39,    76,   100,    17,    45,   143,
     147,   141,   148,   148,    18,   149,   150,    64,   160,    43,
      41,   114,     5,     7,   148,   160,   160,    77,    78,    79,
      97,   100,   108,   109,   154,   155,    20,   150,   156,   157,
      22,   161,   162,    70,   194,    46,    48,    50,    52,    54,
      56,    58,   115,   116,   117,   118,   119,   120,   122,   144,
     145,   146,   160,   194,   194,     5,     5,     5,     7,   155,
      80,    81,    97,   100,   101,   110,   158,   159,    21,    61,
     157,   100,    68,   162,   174,   175,   195,    47,    49,    51,
      53,    55,    57,    59,   146,   194,    62,   120,   122,   123,
     151,   152,   153,     5,     5,     7,   158,     7,   176,    65,
     196,    63,    19,   153,   123,    34,   177,   178,   180,   182,
      30,    71,   197,   121,   122,    69,   180,    82,    83,    98,
     111,   179,    90,   106,   163,     5,     5,   111,     7,     5,
       7,    24,   164,   165,    10,   183,   184,   185,   198,   100,
      23,   165,    36,   184,    82,    84,    85,    99,   112,   113,
     186,   187,    32,   199,   200,   201,     7,   181,     5,     5,
       3,     4,     8,   187,    91,   107,    31,   201,   123,    86,
      87,    88,    89,   102,   103,   104,   105,   188,   189,   190,
     191,   192,   193,     5,     7,   166,     5,     5,     5,     5,
       7,     7,     7,     7,    35,    36,   202,    26,   167,   168,
      37,    37,    37,    37,   189,    33,   100,    25,   168,     7,
      66,   123,   124,   170,   171,    67,   169,   171,    28,   172,
     173,   100,    27,   173,     7,   170,    29
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  Cube3Parser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  Cube3Parser::yyr1_[] =
  {
         0,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   121,   122,   123,   124,   125,   126,   127,   127,
     128,   128,   129,   130,   130,   131,   131,   131,   131,   132,
     132,   133,   134,   134,   135,   135,   136,   137,   137,   138,
     139,   139,   140,   140,   141,   142,   142,   142,   143,   144,
     144,   145,   145,   146,   146,   146,   146,   146,   146,   146,
     147,   148,   149,   149,   150,   151,   151,   152,   152,   153,
     153,   153,   154,   154,   155,   155,   155,   155,   156,   156,
     156,   157,   158,   158,   158,   158,   159,   159,   160,   161,
     161,   163,   162,   164,   164,   166,   165,   167,   167,   169,
     168,   170,   170,   171,   171,   172,   172,   173,   174,   174,
     176,   175,   177,   177,   178,   178,   179,   179,   181,   180,
     182,   183,   183,   184,   185,   186,   186,   187,   187,   187,
     188,   188,   189,   189,   189,   189,   190,   191,   192,   193,
     195,   194,   196,   196,   198,   197,   199,   199,   200,   200,
     202,   201
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  Cube3Parser::yyr2_[] =
  {
         0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     0,     1,     2,     2,     2,     2,     3,     1,     2,
       1,     1,     3,     1,     1,     6,     5,     5,     4,     1,
       2,     3,     1,     2,     1,     1,     3,     0,     1,     3,
       0,     1,     1,     2,     3,     1,     2,     2,     4,     0,
       1,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     4,     1,     2,     5,     0,     1,     1,     2,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     2,
       2,     3,     1,     1,     1,     1,     1,     2,     4,     1,
       2,     0,     8,     1,     2,     0,     7,     1,     2,     0,
       7,     1,     2,     1,     1,     1,     2,     5,     0,     1,
       0,     4,     0,     1,     1,     2,     1,     2,     0,     8,
       1,     1,     2,     3,     1,     1,     2,     1,     1,     1,
       1,     3,     1,     1,     1,     1,     3,     3,     3,     3,
       0,     4,     0,     2,     0,     6,     0,     1,     1,     2,
       0,     5
  };

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const Cube3Parser::yytname_[] =
  {
    "\"end of file\"", "error", "$undefined", "\"false\"", "\"true\"",
  "\"attribute value\"", "\"error\"", "\">\"", "\"/>\"", "\"<attr\"",
  "\"<dim\"", "\"<?xml\"", "\"?>\"", "\"<cube version=\\\"3.0\\\"\"",
  "\"<cube version=\\\"2.0\\\"\"", "\"</cube>\"", "\"<metric\"",
  "\"</metric>\"", "\"<region\"", "\"</region>\"", "\"<cnode\"",
  "\"</cnode>\"", "\"<machine\"", "\"</machine>\"", "\"<node\"",
  "\"</node>\"", "\"<process\"", "\"</process>\"", "\"<thread\"",
  "\"</thread>\"", "\"<matrix\"", "\"</matrix>\"", "\"<row\"",
  "\"</row>\"", "\"<cart\"", "\"</cart>\"", "\"<coord\"", "\"</coord>\"",
  "\"<doc>\"", "\"</doc>\"", "\"<mirrors>\"", "\"</mirrors>\"",
  "\"<murl>\"", "\"</murl>\"", "\"<metrics>\"", "\"</metrics>\"",
  "\"<disp_name>\"", "\"</disp_name>\"", "\"<uniq_name>\"",
  "\"</uniq_name>\"", "\"<dtype>\"", "\"</dtype>\"", "\"<uom>\"",
  "\"</uom>\"", "\"<val>\"", "\"</val>\"", "\"<url>\"", "\"</url>\"",
  "\"<descr>\"", "\"</descr>\"", "\"<program>\"", "\"</program>\"",
  "\"<name>\"", "\"</name>\"", "\"<system>\"", "\"</system>\"",
  "\"<rank>\"", "\"</rank>\"", "\"<topologies>\"", "\"</topologies>\"",
  "\"<severity>\"", "\"</severity>\"", "\"attribute name version\"",
  "\"attribute name encoding\"", "\"attribute name key\"",
  "\"attribute name value\"", "\"attribute name id\"",
  "\"attribute name mod\"", "\"attribute name begin\"",
  "\"attribute name end\"", "\"attribute name line\"",
  "\"attribute name calleeid\"", "\"attribute name name\"",
  "\"attribute name ndims\"", "\"attribute name size\"",
  "\"attribute name periodic\"", "\"attribute name thrdId\"",
  "\"attribute name procId\"", "\"attribute name nodeId\"",
  "\"attribute name machId\"", "\"attribute name metricId\"",
  "\"attribute name cnodeId\"", "$accept", "version_attr", "encoding_attr",
  "key_attr", "value_attr", "mod_attr", "cart_name_attr", "dim_name_attr",
  "id_attr", "calleeid_attr", "thrdid_attr", "procid_attr", "nodeid_attr",
  "machid_attr", "metricid_attr", "cnodeid_attr", "begin_attr", "end_attr",
  "line_attr", "ndims_attr", "size_attr", "periodic_attr", "murl_tag",
  "disp_name_tag", "uniq_name_tag", "dtype_tag", "uom_tag", "val_tag",
  "url_tag", "descr_tag_opt", "descr_tag", "name_tag", "rank_tag",
  "document", "xml_tag", "xml_attributes", "xml_attribute", "cube_tag",
  "cube_begin", "cube_content", "attr_tags", "attr_tag", "attr_attributes",
  "attr_attribute", "doc_tag", "mirrors_tag_opt", "mirrors_tag",
  "murl_tags_opt", "murl_tags", "metrics_tag", "metric_tag",
  "metric_begin", "tags_of_metric_opt", "tags_of_metric", "tag_of_metric",
  "metric_end", "program_tag", "region_tags", "region_tag",
  "tags_of_region_opt", "tags_of_region", "tag_of_region",
  "region_attributes", "region_attribute", "cnode_tag", "cnode_begin",
  "cnode_attribute", "cnode_attributes", "system_tag", "machine_tags",
  "machine_tag", "$@1", "node_tags", "node_tag", "$@2", "process_tags",
  "process_tag", "$@3", "tags_of_process", "tag_of_process", "thread_tags",
  "thread_tag", "topologies_tag_opt", "topologies_tag", "$@4",
  "cart_tags_opt", "cart_tags", "cart_attrs", "cart_tag", "$@5",
  "cart_open", "dim_tags", "dim_tag", "dim_open", "dim_attributes",
  "dim_attribute", "coord_tags", "coord_tag", "coord_tag_thrd",
  "coord_tag_proc", "coord_tag_node", "coord_tag_mach", "severity_tag",
  "$@6", "matrix_tags", "matrix_tag", "$@7", "row_tags_opt", "row_tags",
  "row_tag", "$@8", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const Cube3Parser::rhs_number_type
  Cube3Parser::yyrhs_[] =
  {
       125,     0,    -1,    72,     5,    -1,    73,     5,    -1,    74,
       5,    -1,    75,     5,    -1,    77,     5,    -1,    82,     5,
      -1,    82,     5,    -1,    76,     5,    -1,    81,     5,    -1,
      86,     5,    -1,    87,     5,    -1,    88,     5,    -1,    89,
       5,    -1,    90,     5,    -1,    91,     5,    -1,    78,     5,
      -1,    79,     5,    -1,    80,     5,    -1,    83,     5,    -1,
      84,     5,    -1,    85,     3,    -1,    85,     4,    -1,    42,
      43,    -1,    46,    47,    -1,    48,    49,    -1,    50,    51,
      -1,    52,    53,    -1,    54,    55,    -1,    56,    57,    -1,
      -1,   122,    -1,    58,    59,    -1,    62,    63,    -1,    66,
      67,    -1,   126,   129,    -1,    11,   127,    12,    -1,   128,
      -1,   127,   128,    -1,    93,    -1,    94,    -1,   130,   131,
      15,    -1,    13,    -1,    14,    -1,   132,   136,   141,   148,
     160,   194,    -1,   132,   141,   148,   160,   194,    -1,   136,
     141,   148,   160,   194,    -1,   141,   148,   160,   194,    -1,
     133,    -1,   132,   133,    -1,     9,   134,     8,    -1,   135,
      -1,   134,   135,    -1,    95,    -1,    96,    -1,    38,   137,
      39,    -1,    -1,   138,    -1,    40,   139,    41,    -1,    -1,
     140,    -1,   114,    -1,   140,   114,    -1,    44,   142,    45,
      -1,   143,    -1,   142,   143,    -1,   142,   147,    -1,    16,
     100,     7,   144,    -1,    -1,   145,    -1,   146,    -1,   145,
     146,    -1,   115,    -1,   116,    -1,   117,    -1,   118,    -1,
     119,    -1,   120,    -1,   122,    -1,    17,    -1,    60,   149,
     156,    61,    -1,   150,    -1,   149,   150,    -1,    18,   154,
       7,   151,    19,    -1,    -1,   152,    -1,   153,    -1,   152,
     153,    -1,   123,    -1,   120,    -1,   122,    -1,   155,    -1,
     154,   155,    -1,   100,    -1,    97,    -1,   108,    -1,   109,
      -1,   157,    -1,   156,   157,    -1,   156,    21,    -1,    20,
     159,     7,    -1,   100,    -1,   110,    -1,    97,    -1,   101,
      -1,   158,    -1,   159,   158,    -1,    64,   161,   174,    65,
      -1,   162,    -1,   161,   162,    -1,    -1,    22,   100,     7,
     123,   121,   163,   164,    23,    -1,   165,    -1,   164,   165,
      -1,    -1,    24,   100,     7,   123,   166,   167,    25,    -1,
     168,    -1,   167,   168,    -1,    -1,    26,   100,     7,   170,
     169,   172,    27,    -1,   171,    -1,   170,   171,    -1,   123,
      -1,   124,    -1,   173,    -1,   172,   173,    -1,    28,   100,
       7,   170,    29,    -1,    -1,   175,    -1,    -1,    68,   176,
     177,    69,    -1,    -1,   178,    -1,   180,    -1,   178,   180,
      -1,   111,    -1,    98,   111,    -1,    -1,   182,   179,     7,
     183,    36,   181,   188,    35,    -1,    34,    -1,   184,    -1,
     183,   184,    -1,   185,   186,     8,    -1,    10,    -1,   187,
      -1,   186,   187,    -1,    99,    -1,   112,    -1,   113,    -1,
     189,    -1,   188,    36,   189,    -1,   190,    -1,   191,    -1,
     192,    -1,   193,    -1,   102,     7,    37,    -1,   103,     7,
      37,    -1,   104,     7,    37,    -1,   105,     7,    37,    -1,
      -1,    70,   195,   196,    71,    -1,    -1,   196,   197,    -1,
      -1,    30,   106,     7,   198,   199,    31,    -1,    -1,   200,
      -1,   201,    -1,   200,   201,    -1,    -1,    32,   107,     7,
     202,    33,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  Cube3Parser::yyprhs_[] =
  {
         0,     0,     3,     6,     9,    12,    15,    18,    21,    24,
      27,    30,    33,    36,    39,    42,    45,    48,    51,    54,
      57,    60,    63,    66,    69,    72,    75,    78,    81,    84,
      87,    90,    91,    93,    96,    99,   102,   105,   109,   111,
     114,   116,   118,   122,   124,   126,   133,   139,   145,   150,
     152,   155,   159,   161,   164,   166,   168,   172,   173,   175,
     179,   180,   182,   184,   187,   191,   193,   196,   199,   204,
     205,   207,   209,   212,   214,   216,   218,   220,   222,   224,
     226,   228,   233,   235,   238,   244,   245,   247,   249,   252,
     254,   256,   258,   260,   263,   265,   267,   269,   271,   273,
     276,   279,   283,   285,   287,   289,   291,   293,   296,   301,
     303,   306,   307,   316,   318,   321,   322,   330,   332,   335,
     336,   344,   346,   349,   351,   353,   355,   358,   364,   365,
     367,   368,   373,   374,   376,   378,   381,   383,   386,   387,
     396,   398,   400,   403,   407,   409,   411,   414,   416,   418,
     420,   422,   426,   428,   430,   432,   434,   438,   442,   446,
     450,   451,   456,   457,   460,   461,   468,   469,   471,   473,
     476,   477
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  Cube3Parser::yyrline_[] =
  {
         0,   196,   196,   203,   210,   217,   224,   232,   242,   253,
     262,   272,   281,   290,   299,   308,   317,   328,   336,   344,
     353,   365,   377,   383,   394,   400,   406,   412,   419,   426,
     433,   439,   440,   443,   450,   457,   474,   480,   496,   497,
     501,   502,   508,   512,   515,   521,   522,   523,   524,   530,
     531,   535,   548,   549,   553,   554,   560,   562,   563,   567,
     570,   571,   575,   576,   582,   586,   587,   588,   593,   631,
     632,   636,   637,   641,   642,   643,   644,   645,   646,   647,
     651,   663,   667,   668,   672,   711,   712,   716,   717,   721,
     722,   723,   727,   728,   732,   733,   734,   735,   739,   740,
     741,   750,   787,   788,   789,   790,   794,   795,   801,   805,
     806,   810,   810,   831,   832,   836,   836,   855,   856,   860,
     860,   889,   890,   893,   894,   898,   899,   903,   931,   932,
     936,   936,   942,   943,   947,   948,   952,   953,   957,   957,
     982,   990,   991,   995,  1012,  1018,  1019,  1023,  1024,  1025,
    1029,  1030,  1034,  1035,  1036,  1037,  1041,  1062,  1083,  1104,
    1127,  1127,  1130,  1132,  1137,  1137,  1160,  1161,  1165,  1166,
    1170,  1170
  };

  // Print the state stack on the debug stream.
  void
  Cube3Parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  Cube3Parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "):" << std::endl;
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
  Cube3Parser::token_number_type
  Cube3Parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
           0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int Cube3Parser::yyeof_ = 0;
  const int Cube3Parser::yylast_ = 242;
  const int Cube3Parser::yynnts_ = 111;
  const int Cube3Parser::yyempty_ = -2;
  const int Cube3Parser::yyfinal_ = 10;
  const int Cube3Parser::yyterror_ = 1;
  const int Cube3Parser::yyerrcode_ = 256;
  const int Cube3Parser::yyntokens_ = 92;

  const unsigned int Cube3Parser::yyuser_token_number_max_ = 346;
  const Cube3Parser::token_number_type Cube3Parser::yyundef_token_ = 2;


/* Line 1054 of lalr1.cc  */
#line 1 "[Bison:b4_percent_define_default]"

} // cubeparser

/* Line 1054 of lalr1.cc  */
#line 2177 "Cube3Parser.c"


/* Line 1056 of lalr1.cc  */
#line 1210 "Cube3Parser.yy"
 /*** Additional Code ***/


void cubeparser::Cube3Parser::error(const Cube3Parser::location_type& l,
			    const std::string& m)
{
   if (strstr(m.c_str(),"expecting <?xml")!=NULL) {
     driver.error_just_message("The cube file is probably empty or filled with wrong content. The file has ended before the header of cube started. \n");
    }
   if (strstr(m.c_str()," expecting </row>")!=NULL) {
     driver.error_just_message("One of the possible reasons is \n    1) that the severity value is malformed. CUBE expects the \"double\" value in C_LOCALE with dot instead of comma;. \n    2) that the CUBE file is not properly ended. Probably the writing of CUBE file was interrupted.");
    }
   if (strstr(m.c_str()," expecting <matrix")!=NULL || 
        (strstr(m.c_str()," expecting <severity>")!=NULL) ) {
     driver.error_just_message("The cube file has probably a proper structure, but doesn't contain any severity values.");
    }
   if (strstr(m.c_str()," expecting <metric")!=NULL) {
     driver.error_just_message("The cube file doesn't contain any information about metric dimension.");
    }
   if (strstr(m.c_str()," expecting <region")!=NULL) {
     driver.error_just_message("The cube file doesn't contain any information about program dimension.");
    }
   if (strstr(m.c_str()," expecting <machine")!=NULL) {
     driver.error_just_message("The cube file doesn't contain any information about system dimension.");
    }
   if (strstr(m.c_str()," expecting <thread")!=NULL) {
     driver.error_just_message("The system dimension of the cube file is malformed. It contains a process without any threads.");
    }
   if (strstr(m.c_str()," expecting <process")!=NULL) {
     driver.error_just_message("The system dimension of the cube file is malformed. It contains a node without any processes.");
    }
   if (strstr(m.c_str()," expecting <node")!=NULL) {
     driver.error_just_message("The system dimension of the cube file is malformed. It contains a machine without any computing nodes.");
    }
      driver.error(l, m);

}





