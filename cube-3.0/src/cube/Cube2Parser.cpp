
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
#line 43 "Cube2Parser.c"


#include "Cube2Parser.h"

/* User implementation prologue.  */

/* Line 317 of lalr1.cc  */
#line 163 "Cube2Parser.yy"


#include "Driver.h"
#include "Cube2Scanner.h"
#include <cassert>
#include <cstdlib>
#include <cstring>

using namespace std;

#include "Metric.h"
#include "Cnode.h"

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex cube2Lexer.lex

// Workaround for Sun Studio C++ compilers on Solaris
#if defined(__SVR4) && defined(__SUNPRO_CC)
  #include <ieeefp.h>

  #define isinf(x)  (fpclass(x) == FP_NINF || fpclass(x) == FP_PINF)
  #define isnan(x)  isnand(x)
#endif



/* Line 317 of lalr1.cc  */
#line 82 "Cube2Parser.c"

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
#line 151 "Cube2Parser.c"
#if YYERROR_VERBOSE

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  Cube2Parser::yytnamerr_ (const char *yystr)
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
  Cube2Parser::Cube2Parser (class Driver& driver_yyarg, class ParseContext& parseContext_yyarg, class Cube2Scanner& cube2Lexer_yyarg, class cube::Cube& cube_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      driver (driver_yyarg),
      parseContext (parseContext_yyarg),
      cube2Lexer (cube2Lexer_yyarg),
      cube (cube_yyarg)
  {
  }

  Cube2Parser::~Cube2Parser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  Cube2Parser::yy_symbol_value_print_ (int yytype,
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
  Cube2Parser::yy_symbol_print_ (int yytype,
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
  Cube2Parser::yydestruct_ (const char* yymsg,
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
  Cube2Parser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  Cube2Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  Cube2Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  Cube2Parser::debug_level_type
  Cube2Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  Cube2Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif

  int
  Cube2Parser::parse ()
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
#line 41 "Cube2Parser.yy"
{
    // initialize the initial location object
    yylloc.begin.filename = yylloc.end.filename = &driver.streamname;
}

/* Line 553 of lalr1.cc  */
#line 338 "Cube2Parser.c"

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
	  case 3:

/* Line 678 of lalr1.cc  */
#line 203 "Cube2Parser.yy"
    {
	bool valid = (strcmp(parseContext.version.c_str(),"1.0")==0);
	if (!valid) error(yylloc,"XML version is expected to be 1.0!");
	valid = (strcmp(parseContext.encoding.c_str(),"UTF-8")==0);
	if (!valid) error(yylloc,"XML encoding is expected to be UTF-8!");
}
    break;

  case 5:

/* Line 678 of lalr1.cc  */
#line 218 "Cube2Parser.yy"
    {
	parseContext.cubeVersion = 2;
}
    break;

  case 12:

/* Line 678 of lalr1.cc  */
#line 238 "Cube2Parser.yy"
    {
	cube.def_attr(parseContext.key,parseContext.value);
}
    break;

  case 19:

/* Line 678 of lalr1.cc  */
#line 261 "Cube2Parser.yy"
    {cube.def_mirror(parseContext.murl);}
    break;

  case 20:

/* Line 678 of lalr1.cc  */
#line 262 "Cube2Parser.yy"
    {cube.def_mirror(parseContext.murl);}
    break;

  case 25:

/* Line 678 of lalr1.cc  */
#line 280 "Cube2Parser.yy"
    {
	while ((int)parseContext.metricVec.size()<=parseContext.id) 
		parseContext.metricVec.push_back(NULL);
	if (parseContext.metricVec[parseContext.id]!=NULL)
	  error(yylloc,"Re-declared metric!");
        std::ostringstream uniq;
        uniq << parseContext.id << parseContext.name;
	std::string dtype;
        if (!parseContext.dtype.empty())
	  dtype = parseContext.dtype;
	else 
	  dtype = (parseContext.uom == "sec" ? "FLOAT" : "INTEGER");
	parseContext.currentMetric = 
		cube.def_met(   parseContext.name,
				uniq.str(),
				dtype,
				parseContext.uom,
				parseContext.val,
				parseContext.url,
				parseContext.descr,
				parseContext.currentMetric,
				parseContext.id);
	parseContext.metricVec[parseContext.id] = parseContext.currentMetric;
}
    break;

  case 26:

/* Line 678 of lalr1.cc  */
#line 307 "Cube2Parser.yy"
    {
	if (parseContext.currentMetric==NULL)
	  error(yylloc,"Metric definitions are not correctly nested!"); 
	parseContext.currentMetric = (parseContext.currentMetric)->get_parent();
}
    break;

  case 32:

/* Line 678 of lalr1.cc  */
#line 331 "Cube2Parser.yy"
    {
	while ((int)parseContext.moduleVec.size()<=parseContext.id) 
		parseContext.moduleVec.push_back("");
	if (!parseContext.moduleVec[parseContext.id].empty())
	  error(yylloc,"Re-declared module!"); 
	//in the old parser the name UNKNOWN is skipped,
	//I skip it also for the moment
	//check if it should be so
	std::string name;
	if (strcmp(parseContext.name.c_str(),"UNKNOWN")==0) name = "";
	else name = parseContext.name;
	parseContext.moduleVec[parseContext.id] = name;
}
    break;

  case 35:

/* Line 678 of lalr1.cc  */
#line 352 "Cube2Parser.yy"
    {
	while ((int)parseContext.regionVec.size()<=parseContext.id) 
		parseContext.regionVec.push_back(NULL);
	if (parseContext.regionVec[parseContext.id]!=NULL)
	  error(yylloc,"Re-declared region!"); 
	std::string mod;
	if (parseContext.moduleid<0 || 
	    parseContext.moduleid>=(int)parseContext.moduleVec.size()) 
		mod = "";
        else mod = parseContext.moduleVec[parseContext.moduleid];
	parseContext.regionVec[parseContext.id] = 
		cube.def_region(
			parseContext.name,
			parseContext.beginln,
			parseContext.endln,
			parseContext.url,	
			parseContext.descr,
			mod,
			parseContext.id);
}
    break;

  case 38:

/* Line 678 of lalr1.cc  */
#line 380 "Cube2Parser.yy"
    {
	int startPos = 3 * parseContext.id;
	while ((int)parseContext.csiteVec.size()<startPos+3) 
		parseContext.csiteVec.push_back(0);
	parseContext.csiteVec[startPos] = parseContext.line;
	parseContext.csiteVec[startPos+1] = parseContext.moduleid;
	parseContext.csiteVec[startPos+2] = parseContext.calleenr;
}
    break;

  case 41:

/* Line 678 of lalr1.cc  */
#line 393 "Cube2Parser.yy"
    {
	if (parseContext.currentCnode==NULL)
	  error(yylloc,"Cnode definitions are not correctly nested!"); 
	parseContext.currentCnode = parseContext.currentCnode->get_parent();
}
    break;

  case 42:

/* Line 678 of lalr1.cc  */
#line 401 "Cube2Parser.yy"
    {
	if (3*(1+parseContext.csiteid)>(int)parseContext.csiteVec.size())
	  error(yylloc,"Undefined csite!"); 
	long line = parseContext.csiteVec[(parseContext.csiteid)*3];
	int modId = (int)(parseContext.csiteVec[(parseContext.csiteid)*3+1]);
	if (modId>=0 && (int)parseContext.moduleVec.size()<=modId)
	  error(yylloc,"Undefined module!"); 
	std::string mod;
	if (modId<0) mod = "";
	else mod = parseContext.moduleVec[modId];
	int regionId = (int)(parseContext.csiteVec[(parseContext.csiteid)*3+2]);
	if ((int)parseContext.regionVec.size()<=regionId)
	  error(yylloc,"Undefined region in cnode definition!");
	if (parseContext.regionVec[regionId]==NULL)
	  error(yylloc,"Undefined region in cnode definition!");
	while ((int)parseContext.cnodeVec.size()<=parseContext.id) 
		parseContext.cnodeVec.push_back(NULL);
	if (parseContext.cnodeVec[parseContext.id]!=NULL)
	  error(yylloc,"Re-declared cnode!");
	parseContext.currentCnode = 
		cube.def_cnode(parseContext.regionVec[regionId],
			       mod,
		               line,
			       parseContext.currentCnode,
			       parseContext.id);
	parseContext.cnodeVec[parseContext.id] = parseContext.currentCnode;
}
    break;

  case 47:

/* Line 678 of lalr1.cc  */
#line 448 "Cube2Parser.yy"
    {
	parseContext.currentMachine = cube.def_mach(
		parseContext.name,
		"");
	for (int i=parseContext.systemVec.size(); i<=parseContext.locid; i++)
		parseContext.systemVec.push_back(NULL);
	if (parseContext.systemVec[parseContext.locid]!=NULL)
	  error(yylloc,"Re-declared machine!");
	parseContext.systemVec[parseContext.locid] = 
		(cube::Sysres*)(parseContext.currentMachine);
}
    break;

  case 51:

/* Line 678 of lalr1.cc  */
#line 471 "Cube2Parser.yy"
    {
	parseContext.currentNode = cube.def_node(
				parseContext.name,
				parseContext.currentMachine);
	for (int i=parseContext.systemVec.size(); i<=parseContext.locid; i++)
		parseContext.systemVec.push_back(NULL);
	if (parseContext.systemVec[parseContext.locid]!=NULL)
	  error(yylloc,"Re-declared node!");
	parseContext.systemVec[parseContext.locid] = 
		(cube::Sysres*)(parseContext.currentNode);
}
    break;

  case 55:

/* Line 678 of lalr1.cc  */
#line 494 "Cube2Parser.yy"
    {
	std::ostringstream name;
	if (parseContext.name.empty()) {
	  name << "Process " << parseContext.rank;
	} else {
	  name << parseContext.name;
	}
	parseContext.currentProc = cube.def_proc(
			name.str(),
			parseContext.rank,
			parseContext.currentNode);
	for (int i=parseContext.systemVec.size(); i<=parseContext.locid; i++)
		parseContext.systemVec.push_back(NULL);
	if (parseContext.systemVec[parseContext.locid]!=NULL)
	  error(yylloc,"Re-declared process!");
	parseContext.systemVec[parseContext.locid] = 
		(cube::Sysres*)(parseContext.currentProc);
}
    break;

  case 58:

/* Line 678 of lalr1.cc  */
#line 520 "Cube2Parser.yy"
    {
	std::ostringstream name;
	if (parseContext.name.empty()) {
	  name << "Thread " << parseContext.rank;
	} else {
	  name << parseContext.name;
	}
	parseContext.currentThread = cube.def_thrd(
			name.str(),
			parseContext.rank,
			parseContext.currentProc);
	parseContext.threadVec.push_back(parseContext.currentThread);
	for (int i=parseContext.systemVec.size(); i<=parseContext.locid; i++)
		parseContext.systemVec.push_back(NULL);
	if (parseContext.systemVec[parseContext.locid]!=NULL)
	  error(yylloc,"Re-declared thread!"); 
	parseContext.systemVec[parseContext.locid] = 
		(cube::Sysres*)(parseContext.currentThread);
}
    break;

  case 66:

/* Line 678 of lalr1.cc  */
#line 559 "Cube2Parser.yy"
    {
	if (parseContext.ndims!=(int)parseContext.dimVec.size())
	  error(yylloc,"Too few or too many topology dimensions are declared!");
	parseContext.currentCart = 
		cube.def_cart(parseContext.ndims,parseContext.dimVec,parseContext.periodicVec);
}
    break;

  case 68:

/* Line 678 of lalr1.cc  */
#line 568 "Cube2Parser.yy"
    {
	parseContext.dimVec.clear();
	parseContext.periodicVec.clear();
}
    break;

  case 74:

/* Line 678 of lalr1.cc  */
#line 589 "Cube2Parser.yy"
    {
	if (parseContext.dimVec.size()!=parseContext.longValues.size())
	  error(yylloc,"Too few or too many dimension coordinates in coord tag!"); 
	for (unsigned i=0; i<parseContext.dimVec.size(); i++){
	  if (parseContext.longValues[i]>=parseContext.dimVec[i])
	    error(yylloc,"Topology coordinate is out of range!");
	}
	if (parseContext.locid>=(int)parseContext.systemVec.size()) 
	  error(yylloc,"System entity of the topology coordinates wasn't declared!"); 
	if (parseContext.systemVec[parseContext.locid]==NULL) 
	  error(yylloc,"System entity of the topology coordinates wasn't declared!");
	cube.def_coords(parseContext.currentCart,
                        parseContext.systemVec[parseContext.locid],
                        parseContext.longValues);
}
    break;

  case 78:

/* Line 678 of lalr1.cc  */
#line 619 "Cube2Parser.yy"
    {
	if (parseContext.metricid<0) parseContext.ignoreMetric = true;
	else {
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
}
    break;

  case 84:

/* Line 678 of lalr1.cc  */
#line 651 "Cube2Parser.yy"
    {
	if (!parseContext.ignoreMetric){
	if ((int)parseContext.cnodeVec.size()<=parseContext.cnodeid)
	  error(yylloc,"Cnode of the severity row wasn't declared!"); 
	if (parseContext.cnodeVec[parseContext.cnodeid]==NULL)
	  error(yylloc,"Cnode of the severity row wasn't declared!");
	parseContext.currentCnode = parseContext.cnodeVec[parseContext.cnodeid];
	}
}
    break;

  case 85:

/* Line 678 of lalr1.cc  */
#line 659 "Cube2Parser.yy"
    {
	if (!parseContext.ignoreMetric){
        if (parseContext.realValues.size()>parseContext.threadVec.size())	
          error(yylloc,"Too many values in severity row!"); 
        for (unsigned i=0; i<parseContext.realValues.size(); i++){
            if (isinf(parseContext.realValues[i]) || isnan(parseContext.realValues[i])) {
              error(yylloc,"Malformed severity value (NAN or INF) in input file.");
            }
            else
            {
		if (parseContext.realValues[i]!=0.0){
	           cube.set_sev(parseContext.currentMetric,
	  	        parseContext.currentCnode,
		        parseContext.threadVec[i],
		        parseContext.realValues[i]);
		}
            }
        }
	}
}
    break;

  case 86:

/* Line 678 of lalr1.cc  */
#line 685 "Cube2Parser.yy"
    { parseContext.version = parseContext.str.str(); }
    break;

  case 87:

/* Line 678 of lalr1.cc  */
#line 687 "Cube2Parser.yy"
    { parseContext.encoding = parseContext.str.str(); }
    break;

  case 88:

/* Line 678 of lalr1.cc  */
#line 689 "Cube2Parser.yy"
    { parseContext.key = parseContext.str.str(); }
    break;

  case 89:

/* Line 678 of lalr1.cc  */
#line 691 "Cube2Parser.yy"
    { parseContext.value = parseContext.str.str(); }
    break;

  case 90:

/* Line 678 of lalr1.cc  */
#line 696 "Cube2Parser.yy"
    { 
	if (parseContext.str.str().empty())
	  error(yylloc,"Empty id!"); 
	int id = atoi(parseContext.str.str().c_str());
	if (id<0) error(yylloc,"Ids must be non-negative!");
	parseContext.id = id; 
}
    break;

  case 91:

/* Line 678 of lalr1.cc  */
#line 704 "Cube2Parser.yy"
    { 
	if (parseContext.str.str().empty())
	  error(yylloc,"Empty id!");
	int id = atoi(parseContext.str.str().c_str());
	if (id<0) error(yylloc,"Ids must be non-negative!");
	parseContext.csiteid = id; 
}
    break;

  case 92:

/* Line 678 of lalr1.cc  */
#line 712 "Cube2Parser.yy"
    { 
	if (parseContext.str.str().empty())
	  error(yylloc,"Empty id!");
	int id = atoi(parseContext.str.str().c_str());
	if (id<0) error(yylloc,"Thread ids must be non-negative!");
	parseContext.locid = id; 
}
    break;

  case 93:

/* Line 678 of lalr1.cc  */
#line 720 "Cube2Parser.yy"
    { 
	if (parseContext.str.str().empty())
	  error(yylloc,"Empty id!");
	int id = atoi(parseContext.str.str().c_str());
	parseContext.metricid = id; 
}
    break;

  case 94:

/* Line 678 of lalr1.cc  */
#line 727 "Cube2Parser.yy"
    { 
	if (parseContext.str.str().empty())
	  error(yylloc,"Empty id!");
	int id = atoi(parseContext.str.str().c_str());
	if (id<0) error(yylloc,"Cnode ids must be non-negative!");
 	parseContext.cnodeid= id; 
}
    break;

  case 95:

/* Line 678 of lalr1.cc  */
#line 738 "Cube2Parser.yy"
    { 
	if (parseContext.str.str().empty())
	  error(yylloc,"Empty ndims!");
	long ndims = atol(parseContext.str.str().c_str());
	if (ndims<=0)
	   error(yylloc,"Topology dimensions must be positive numbers!");
	parseContext.ndims = ndims; 
}
    break;

  case 96:

/* Line 678 of lalr1.cc  */
#line 750 "Cube2Parser.yy"
    { 
	if (parseContext.str.str().empty())
	  error(yylloc,"Empty size!");
	long size = atol(parseContext.str.str().c_str());
	if (size<=0)
	  error(yylloc,"Dimension sizes must be positive numbers!");
	parseContext.dimVec.push_back(size);
}
    break;

  case 97:

/* Line 678 of lalr1.cc  */
#line 762 "Cube2Parser.yy"
    { 
	parseContext.periodicVec.push_back(false);
}
    break;

  case 98:

/* Line 678 of lalr1.cc  */
#line 765 "Cube2Parser.yy"
    { 
	parseContext.periodicVec.push_back(true);
}
    break;

  case 99:

/* Line 678 of lalr1.cc  */
#line 778 "Cube2Parser.yy"
    {
	parseContext.beginln = atol(parseContext.str.str().c_str());
}
    break;

  case 100:

/* Line 678 of lalr1.cc  */
#line 781 "Cube2Parser.yy"
    {parseContext.beginln = -1;}
    break;

  case 101:

/* Line 678 of lalr1.cc  */
#line 784 "Cube2Parser.yy"
    { 
	parseContext.endln = atol(parseContext.str.str().c_str());
}
    break;

  case 102:

/* Line 678 of lalr1.cc  */
#line 787 "Cube2Parser.yy"
    {parseContext.endln = -1;}
    break;

  case 103:

/* Line 678 of lalr1.cc  */
#line 790 "Cube2Parser.yy"
    {
	parseContext.line = atol(parseContext.str.str().c_str());
}
    break;

  case 104:

/* Line 678 of lalr1.cc  */
#line 793 "Cube2Parser.yy"
    {parseContext.line = -1;}
    break;

  case 105:

/* Line 678 of lalr1.cc  */
#line 799 "Cube2Parser.yy"
    {
	  int moduleId = atoi(parseContext.str.str().c_str());
	  if (moduleId<0)
	    error(yylloc,"Module ids must be nonnegative integers!");
	  if ((int)parseContext.moduleVec.size()<=moduleId) 
	    error(yylloc,"Reference to an undefined module!");
	parseContext.moduleid = moduleId;
}
    break;

  case 106:

/* Line 678 of lalr1.cc  */
#line 807 "Cube2Parser.yy"
    {parseContext.moduleid = -1;}
    break;

  case 107:

/* Line 678 of lalr1.cc  */
#line 811 "Cube2Parser.yy"
    {
	int calleeNr = atoi(parseContext.str.str().c_str());
	if (calleeNr<0)
	  error(yylloc,"Callee ids must be nonnegative integers!");
	parseContext.calleenr = calleeNr; 
}
    break;

  case 108:

/* Line 678 of lalr1.cc  */
#line 817 "Cube2Parser.yy"
    {parseContext.calleenr = -1;}
    break;

  case 109:

/* Line 678 of lalr1.cc  */
#line 820 "Cube2Parser.yy"
    {
	int rank = atoi(parseContext.str.str().c_str());
	if (rank<0) 
	  error(yylloc,"Ranks must be non-negative!");
	parseContext.rank = rank; 
}
    break;

  case 110:

/* Line 678 of lalr1.cc  */
#line 830 "Cube2Parser.yy"
    {parseContext.dtype = parseContext.str.str();}
    break;

  case 111:

/* Line 678 of lalr1.cc  */
#line 831 "Cube2Parser.yy"
    {parseContext.dtype = "";}
    break;

  case 112:

/* Line 678 of lalr1.cc  */
#line 834 "Cube2Parser.yy"
    {parseContext.murl = parseContext.str.str(); }
    break;

  case 113:

/* Line 678 of lalr1.cc  */
#line 837 "Cube2Parser.yy"
    {parseContext.name = parseContext.str.str(); }
    break;

  case 114:

/* Line 678 of lalr1.cc  */
#line 840 "Cube2Parser.yy"
    {parseContext.uom = parseContext.str.str(); }
    break;

  case 115:

/* Line 678 of lalr1.cc  */
#line 841 "Cube2Parser.yy"
    {parseContext.uom = "";}
    break;

  case 116:

/* Line 678 of lalr1.cc  */
#line 844 "Cube2Parser.yy"
    {parseContext.val = parseContext.str.str(); }
    break;

  case 117:

/* Line 678 of lalr1.cc  */
#line 845 "Cube2Parser.yy"
    {parseContext.val = "";}
    break;

  case 118:

/* Line 678 of lalr1.cc  */
#line 848 "Cube2Parser.yy"
    {parseContext.url = parseContext.str.str(); }
    break;

  case 119:

/* Line 678 of lalr1.cc  */
#line 849 "Cube2Parser.yy"
    {parseContext.url = "";}
    break;

  case 120:

/* Line 678 of lalr1.cc  */
#line 852 "Cube2Parser.yy"
    {parseContext.descr = parseContext.str.str(); }
    break;

  case 121:

/* Line 678 of lalr1.cc  */
#line 853 "Cube2Parser.yy"
    {parseContext.descr = "";}
    break;



/* Line 678 of lalr1.cc  */
#line 1160 "Cube2Parser.c"
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
  Cube2Parser::yysyntax_error_ (int yystate, int tok)
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
  const signed char Cube2Parser::yypact_ninf_ = -88;
  const short int
  Cube2Parser::yypact_[] =
  {
         7,   -62,    28,    11,   -58,   -44,   -88,   -88,   -88,     0,
     -88,   -51,    38,   -24,     5,    45,    50,     0,   -88,     8,
      -4,   -88,   -88,   -22,   -16,    14,    18,   -88,   -13,    -6,
     -88,   -88,   -88,     8,    -4,    -4,    39,     2,   -88,   -20,
      69,    21,    24,    14,   -88,   -88,   -12,    70,   -88,   -88,
     -88,   -88,    -4,     2,     2,   -13,    43,    39,   -88,    63,
       6,   -88,   -88,   -88,   -88,   -88,   -88,    27,     2,     6,
       6,    75,   -13,   -21,   -88,   -88,     1,   -14,   -88,    68,
      59,   -88,    31,    32,     6,   -88,   -88,    27,    83,   -13,
     -88,    -5,   -88,     9,    85,    71,   -88,    26,   -88,     1,
      20,   -88,    72,    10,   -25,   -88,   -88,    34,    36,   -88,
      61,    27,    94,   -13,   -88,   -10,   -88,   -88,    27,   -88,
      29,    71,   -88,    16,   -88,    96,   -88,   -88,     1,    22,
     -88,    84,    15,   101,   -88,   -88,   -88,    47,    48,   -88,
      67,    73,    30,   -88,   -88,   -88,   -88,   -88,   -88,    33,
     105,    27,   106,   -88,   -88,     1,    23,   -88,   -88,   -88,
     -88,    51,    52,    76,    77,    79,    67,    35,   112,   -88,
     111,   -88,    27,   115,   -88,   -88,    92,   -88,    62,    60,
     -88,    89,    90,   -88,    86,   -88,   -88,    49,     3,   -88,
      57,    27,    42,   104,    92,   -88,   -88,    74,   -88,   -88,
      93,    52,    91,    99,    53,    55,   -88,   -88,    64,   -88,
      57,    54,   133,   -88,   -88,   -88,   -88,    60,   -88,   -88,
     -88,    44,   136,     1,   -88,   122,   -88,   -88,   113,   -88,
     -88,   -88,    25,   -88,   140,   -88,   116,   -88,   -88,     1,
     123,   -88,   -88,   -88
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned char
  Cube2Parser::yydefact_[] =
  {
         0,     0,     0,     0,     0,     0,     1,     5,     2,     0,
      86,     0,     0,     0,    14,     0,     0,     0,    10,     0,
       0,    87,     3,     0,     0,    17,     0,    15,     0,     0,
      22,     4,    11,     0,     0,     0,    28,     0,    88,     0,
       0,     0,     0,    18,    19,    13,     0,     0,    26,    21,
      23,    24,     0,     0,     0,     0,     0,    29,    30,     0,
       0,    89,    12,   112,    16,    20,    90,     0,     0,     0,
       0,     0,     0,     0,    33,    31,     0,    59,    44,     0,
       0,     9,     0,   111,     0,     8,     7,     0,     0,     0,
      34,     0,    36,     0,     0,    62,    45,     0,    60,     0,
       0,    48,     0,     0,     0,    76,   113,     0,   115,     6,
       0,     0,     0,     0,    37,     0,    39,    92,     0,    68,
       0,    63,    64,     0,    43,     0,    46,    49,     0,     0,
      52,     0,     0,     0,    75,    77,   110,     0,   117,    32,
     106,   104,     0,    41,    27,    40,    47,    61,    65,     0,
       0,     0,     0,    50,    53,     0,     0,    56,    93,    78,
     114,     0,   119,     0,   100,     0,   106,     0,     0,    95,
       0,    51,     0,     0,    54,    57,    80,   116,     0,   121,
     105,     0,   102,   103,   108,    91,    42,     0,     0,    69,
       0,     0,     0,     0,    81,    82,   118,     0,    25,    99,
       0,   119,     0,     0,     0,     0,    66,    70,     0,    55,
       0,     0,     0,    79,    83,   120,   101,   121,   107,    38,
      96,     0,     0,     0,   109,     0,    94,    84,     0,    97,
      98,    71,     0,    72,     0,    58,     0,    35,    67,     0,
       0,    85,    73,    74
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  Cube2Parser::yypgoto_[] =
  {
       -88,   -88,   -88,   -88,   -88,   -88,   -88,   135,   137,   -88,
     -88,   -88,   -88,     4,   -88,   121,   -88,   -23,   -88,   -88,
      98,   -88,    80,   -88,    65,   -88,    46,   -34,   -88,    81,
     -88,   -88,    66,   -88,   -88,    40,   -88,   -88,    12,   -88,
     -88,   -88,   -88,    41,   -88,   -88,   -88,   -31,   -88,   -80,
     -43,   -88,    56,   -88,   -88,   -88,   -30,   -88,   -88,   -88,
     -88,   -88,   -54,   -88,   -74,   -88,   -88,   -88,   -88,   -88,
     -88,   -88,   -88,    -3,   -88,   -45,   -88,   124,   -87,   -88,
     -88,   -29,   -47
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  Cube2Parser::yydefgoto_[] =
  {
        -1,     2,     3,     8,     9,    16,    17,    18,    19,    26,
      27,    42,    43,    20,    29,    30,    51,    37,    56,    57,
      58,    73,    74,    91,    92,   115,   116,    60,    77,    78,
      79,   100,   101,   102,   129,   130,   131,   156,   157,    97,
      98,   120,   121,   122,   223,   123,   188,   189,   232,   233,
      81,   104,   105,   176,   193,   194,   195,   236,     5,    12,
      24,    40,    47,   168,   234,   133,   212,   150,   205,   222,
     182,   201,   166,   164,   203,   209,   108,    44,    83,   138,
     162,   179,   198
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const signed char Cube2Parser::yytable_ninf_ = -1;
  const unsigned char
  Cube2Parser::yytable_[] =
  {
       110,    71,    94,    76,   103,   113,   143,    28,    48,    13,
     113,    53,    54,   187,    72,     1,    89,     4,    88,    69,
      70,    34,     7,    35,   140,   125,    85,    86,     6,    68,
     206,   146,    89,    10,    84,   112,    11,    52,   126,    99,
      21,   109,   153,   128,    22,   174,   155,   229,   230,    14,
      49,   238,   239,   134,   152,    15,    25,    23,    28,   142,
     144,    95,    31,    15,   171,    36,    39,    41,    45,    38,
      46,    61,    55,    59,    62,    63,    64,    67,    72,    66,
      76,   173,    87,    80,    82,   190,    93,    99,   103,   106,
     111,   107,   118,   128,   136,   139,   119,   137,   124,   132,
     117,   141,   149,   151,   210,   147,   158,   155,   159,   160,
     163,   161,   170,   172,   167,   177,   181,   178,   165,   186,
     180,   187,   191,   192,   169,   183,   185,   197,   196,   199,
     208,   200,   211,   202,   213,   216,   204,   219,   224,   218,
     227,   231,   215,   221,   220,   226,   235,   240,   241,   237,
      50,   243,    32,    90,    33,    75,   114,   207,    96,   242,
     135,   145,   148,   184,   214,   225,   127,    65,   175,   154,
     228,     0,   217
  };

  /* YYCHECK.  */
  const short int
  Cube2Parser::yycheck_[] =
  {
        87,    55,    76,    17,    29,    15,    16,    13,    14,     9,
      15,    34,    35,    10,    35,     8,    37,    79,    72,    53,
      54,    17,    11,    19,   111,    99,    69,    70,     0,    52,
      27,   118,    37,    91,    68,    89,    80,    33,    18,    19,
      91,    84,    20,    21,     6,    22,    23,     3,     4,    49,
      56,    26,    27,    78,   128,    55,    51,    81,    13,   113,
      70,    75,    12,    55,   151,    69,    82,    53,    50,    91,
      83,    91,    33,    71,     5,    54,    52,     7,    35,    91,
      17,   155,     7,    77,    57,   172,    85,    19,    29,    58,
       7,    59,     7,    21,    60,    34,    25,    61,    72,    89,
      91,     7,    86,     7,   191,    76,    91,    23,     7,    62,
      43,    63,     7,     7,    84,    64,    39,    65,    45,     7,
      44,    10,     7,    31,    91,    46,    91,    67,    66,    40,
      73,    41,    90,    47,    30,    42,    87,    38,    74,    48,
       7,     5,    68,    88,    91,    91,    24,     7,    32,    36,
      29,    28,    17,    73,    17,    57,    91,   188,    77,   239,
     104,   115,   121,   166,   194,   210,   100,    43,   156,   129,
     217,    -1,   201
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  Cube2Parser::yystos_[] =
  {
         0,     8,    94,    95,    79,   151,     0,    11,    96,    97,
      91,    80,   152,     9,    49,    55,    98,    99,   100,   101,
     106,    91,     6,    81,   153,    51,   102,   103,    13,   107,
     108,    12,   100,   101,   106,   106,    69,   110,    91,    82,
     154,    53,   104,   105,   170,    50,    83,   155,    14,    56,
     108,   109,   106,   110,   110,    33,   111,   112,   113,    71,
     120,    91,     5,    54,    52,   170,    91,     7,   110,   120,
     120,   155,    35,   114,   115,   113,    17,   121,   122,   123,
      77,   143,    57,   171,   120,   143,   143,     7,   155,    37,
     115,   116,   117,    85,   157,    75,   122,   132,   133,    19,
     124,   125,   126,    29,   144,   145,    58,    59,   169,   143,
     171,     7,   155,    15,   117,   118,   119,    91,     7,    25,
     134,   135,   136,   138,    72,   157,    18,   125,    21,   127,
     128,   129,    89,   158,    78,   145,    60,    61,   172,    34,
     171,     7,   155,    16,    70,   119,   171,    76,   136,    86,
     160,     7,   157,    20,   128,    23,   130,   131,    91,     7,
      62,    63,   173,    43,   166,    45,   165,    84,   156,    91,
       7,   171,     7,   157,    22,   131,   146,    64,    65,   174,
      44,    39,   163,    46,   166,    91,     7,    10,   139,   140,
     171,     7,    31,   147,   148,   149,    66,    67,   175,    40,
      41,   164,    47,   167,    87,   161,    27,   140,    73,   168,
     171,    90,   159,    30,   149,    68,    42,   174,    48,    38,
      91,    88,   162,   137,    74,   168,    91,     7,   175,     3,
       4,     5,   141,   142,   157,    24,   150,    36,    26,    27,
       7,    32,   142,    28
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  Cube2Parser::yytoken_number_[] =
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
     345,   346,   347
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  Cube2Parser::yyr1_[] =
  {
         0,    93,    94,    95,    96,    97,    98,    98,    98,    98,
      99,    99,   100,   101,   102,   102,   103,   104,   104,   105,
     105,   106,   107,   107,   107,   108,   109,   110,   111,   111,
     112,   112,   113,   114,   114,   115,   116,   116,   117,   118,
     118,   118,   119,   120,   121,   121,   122,   123,   124,   124,
     125,   126,   127,   127,   128,   129,   130,   130,   131,   132,
     132,   133,   134,   134,   135,   135,   137,   136,   138,   139,
     139,   140,   141,   141,   142,   143,   144,   144,   146,   145,
     147,   147,   148,   148,   150,   149,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   162,   163,
     163,   164,   164,   165,   165,   166,   166,   167,   167,   168,
     169,   169,   170,   171,   172,   172,   173,   173,   174,   174,
     175,   175
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  Cube2Parser::yyr2_[] =
  {
         0,     2,     2,     4,     3,     1,     6,     5,     5,     4,
       1,     2,     4,     3,     0,     1,     3,     0,     1,     1,
       2,     3,     1,     2,     2,     9,     1,     6,     0,     1,
       1,     2,     5,     1,     2,    10,     1,     2,     7,     1,
       2,     2,     4,     4,     1,     2,     3,     4,     1,     2,
       3,     4,     1,     2,     3,     5,     1,     2,     6,     0,
       1,     3,     0,     1,     1,     2,     0,     8,     1,     1,
       2,     4,     1,     3,     3,     3,     1,     2,     0,     6,
       0,     1,     1,     2,     0,     5,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       0,     2,     0,     2,     0,     2,     0,     2,     0,     2,
       2,     0,     2,     2,     2,     0,     2,     0,     2,     0,
       2,     0
  };

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const Cube2Parser::yytname_[] =
  {
    "\"end of file\"", "error", "$undefined", "\"false\"", "\"true\"",
  "\"/>\"", "\"?>\"", "\">\"", "\"<?xml\"", "\"<attr\"", "\"<dim\"",
  "\"<cube version=\\\"2.0\\\"\"", "\"</cube>\"", "\"<metric\"",
  "\"</metric>\"", "\"<cnode\"", "\"</cnode>\"", "\"<machine\"",
  "\"</machine>\"", "\"<node\"", "\"</node>\"", "\"<process\"",
  "\"</process>\"", "\"<thread\"", "\"</thread>\"", "\"<cart\"",
  "\"</cart>\"", "\"<coord\"", "\"</coord>\"", "\"<matrix\"",
  "\"</matrix>\"", "\"<row\"", "\"</row>\"", "\"<module\"",
  "\"</module>\"", "\"<region\"", "\"</region>\"", "\"<csite\"",
  "\"</csite>\"", "\"<begin>\"", "\"</begin>\"", "\"<end>\"", "\"</end>\"",
  "\"<mod>\"", "\"</mod>\"", "\"<line>\"", "\"</line>\"", "\"<callee>\"",
  "\"</callee>\"", "\"<doc>\"", "\"</doc>\"", "\"<mirrors>\"",
  "\"</mirrors>\"", "\"<murl>\"", "\"</murl>\"", "\"<behavior>\"",
  "\"</behavior>\"", "\"<name>\"", "\"</name>\"", "\"<dtype>\"",
  "\"</dtype>\"", "\"<uom>\"", "\"</uom>\"", "\"<val>\"", "\"</val>\"",
  "\"<url>\"", "\"</url>\"", "\"<descr>\"", "\"</descr>\"",
  "\"<program>\"", "\"</program>\"", "\"<locations>\"", "\"</locations>\"",
  "\"<rank>\"", "\"</rank>\"", "\"<topologies>\"", "\"</topologies>\"",
  "\"<severity>\"", "\"</severity>\"", "\"attribute name version\"",
  "\"attribute name encoding\"", "\"attribute name key\"",
  "\"attribute name value\"", "\"attribute name id\"",
  "\"attribute name csiteId\"", "\"attribute name locid\"",
  "\"attribute name ndims\"", "\"attribute name size\"",
  "\"attribute name periodic\"", "\"attribute name metricId\"",
  "\"attribute name cnodeId\"", "\"attribute value\"",
  "\"unknown element\"", "$accept", "document", "xml_tag", "cube_tag",
  "cube_begin", "cube_content", "attr_tags", "attr_tag", "doc_tag",
  "mirrors_tag_opt", "mirrors_tag", "murl_tags_opt", "murl_tags",
  "behavior_tag", "metric_tag", "metric_begin", "metric_end",
  "program_tag", "module_tags_opt", "module_tags", "module_tag",
  "region_tags", "region_tag", "csite_tags", "csite_tag", "cnode_tag",
  "cnode_begin", "locations_tag", "machine_tags", "machine_tag",
  "machine_begin", "node_tags", "node_tag", "node_begin", "process_tags",
  "process_tag", "process_begin", "thread_tags", "thread_tag",
  "topologies_tag_opt", "topologies_tag", "cart_tags_opt", "cart_tags",
  "cart_tag", "$@1", "cart_begin", "dim_tags", "dim_tag", "coord_tags",
  "coord_tag", "severity_tag", "matrix_tags", "matrix_tag", "$@2",
  "row_tags_opt", "row_tags", "row_tag", "$@3", "version_attr",
  "encoding_attr", "key_attr", "value_attr", "id_attr", "csiteid_attr",
  "locid_attr", "metricid_attr", "cnodeid_attr", "ndims_attr", "size_attr",
  "periodic_attr", "begin_tag", "end_tag", "line_tag", "mod_tag",
  "callee_tag", "rank_tag", "dtype_tag", "murl_tag", "name_tag", "uom_tag",
  "val_tag", "url_tag", "descr_tag", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const Cube2Parser::rhs_number_type
  Cube2Parser::yyrhs_[] =
  {
        94,     0,    -1,    95,    96,    -1,     8,   151,   152,     6,
      -1,    97,    98,    12,    -1,    11,    -1,    99,   101,   106,
     110,   120,   143,    -1,   101,   106,   110,   120,   143,    -1,
      99,   106,   110,   120,   143,    -1,   106,   110,   120,   143,
      -1,   100,    -1,    99,   100,    -1,     9,   153,   154,     5,
      -1,    49,   102,    50,    -1,    -1,   103,    -1,    51,   104,
      52,    -1,    -1,   105,    -1,   170,    -1,   105,   170,    -1,
      55,   107,    56,    -1,   108,    -1,   107,   108,    -1,   107,
     109,    -1,    13,   155,     7,   171,   169,   172,   173,   174,
     175,    -1,    14,    -1,    69,   111,   114,   116,   118,    70,
      -1,    -1,   112,    -1,   113,    -1,   112,   113,    -1,    33,
     155,     7,   171,    34,    -1,   115,    -1,   114,   115,    -1,
      35,   155,     7,   171,   166,   163,   164,   174,   175,    36,
      -1,   117,    -1,   116,   117,    -1,    37,   155,     7,   165,
     166,   167,    38,    -1,   119,    -1,   118,   119,    -1,   118,
      16,    -1,    15,   155,   156,     7,    -1,    71,   121,   132,
      72,    -1,   122,    -1,   121,   122,    -1,   123,   124,    18,
      -1,    17,   157,     7,   171,    -1,   125,    -1,   124,   125,
      -1,   126,   127,    20,    -1,    19,   157,     7,   171,    -1,
     128,    -1,   127,   128,    -1,   129,   130,    22,    -1,    21,
     157,     7,   171,   168,    -1,   131,    -1,   130,   131,    -1,
      23,   157,     7,   171,   168,    24,    -1,    -1,   133,    -1,
      75,   134,    76,    -1,    -1,   135,    -1,   136,    -1,   135,
     136,    -1,    -1,   138,   160,     7,   139,    27,   137,   141,
      26,    -1,    25,    -1,   140,    -1,   139,   140,    -1,    10,
     161,   162,     5,    -1,   142,    -1,   141,    27,   142,    -1,
     157,     7,    28,    -1,    77,   144,    78,    -1,   145,    -1,
     144,   145,    -1,    -1,    29,   158,     7,   146,   147,    30,
      -1,    -1,   148,    -1,   149,    -1,   148,   149,    -1,    -1,
      31,   159,     7,   150,    32,    -1,    79,    91,    -1,    80,
      91,    -1,    81,    91,    -1,    82,    91,    -1,    83,    91,
      -1,    84,    91,    -1,    85,    91,    -1,    89,    91,    -1,
      90,    91,    -1,    86,    91,    -1,    87,    91,    -1,    88,
       3,    -1,    88,     4,    -1,    39,    40,    -1,    -1,    41,
      42,    -1,    -1,    45,    46,    -1,    -1,    43,    44,    -1,
      -1,    47,    48,    -1,    -1,    73,    74,    -1,    59,    60,
      -1,    -1,    53,    54,    -1,    57,    58,    -1,    61,    62,
      -1,    -1,    63,    64,    -1,    -1,    65,    66,    -1,    -1,
      67,    68,    -1,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  Cube2Parser::yyprhs_[] =
  {
         0,     0,     3,     6,    11,    15,    17,    24,    30,    36,
      41,    43,    46,    51,    55,    56,    58,    62,    63,    65,
      67,    70,    74,    76,    79,    82,    92,    94,   101,   102,
     104,   106,   109,   115,   117,   120,   131,   133,   136,   144,
     146,   149,   152,   157,   162,   164,   167,   171,   176,   178,
     181,   185,   190,   192,   195,   199,   205,   207,   210,   217,
     218,   220,   224,   225,   227,   229,   232,   233,   242,   244,
     246,   249,   254,   256,   260,   264,   268,   270,   273,   274,
     281,   282,   284,   286,   289,   290,   296,   299,   302,   305,
     308,   311,   314,   317,   320,   323,   326,   329,   332,   335,
     338,   339,   342,   343,   346,   347,   350,   351,   354,   355,
     358,   361,   362,   365,   368,   371,   372,   375,   376,   379,
     380,   383
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  Cube2Parser::yyrline_[] =
  {
         0,   197,   197,   203,   214,   218,   224,   225,   226,   227,
     233,   234,   238,   246,   248,   249,   253,   256,   257,   261,
     262,   269,   273,   274,   275,   280,   307,   318,   321,   322,
     326,   327,   331,   347,   348,   352,   375,   376,   380,   391,
     392,   393,   401,   435,   439,   440,   444,   448,   462,   463,
     467,   471,   485,   486,   490,   494,   515,   516,   520,   541,
     542,   546,   549,   550,   554,   555,   559,   559,   568,   575,
     576,   580,   584,   585,   589,   609,   613,   614,   619,   619,
     641,   642,   646,   647,   651,   651,   685,   687,   689,   691,
     696,   704,   712,   720,   727,   738,   750,   762,   765,   778,
     781,   784,   787,   790,   793,   799,   807,   811,   817,   820,
     830,   831,   834,   837,   840,   841,   844,   845,   848,   849,
     852,   853
  };

  // Print the state stack on the debug stream.
  void
  Cube2Parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  Cube2Parser::yy_reduce_print_ (int yyrule)
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
  Cube2Parser::token_number_type
  Cube2Parser::yytranslate_ (int t)
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
      85,    86,    87,    88,    89,    90,    91,    92
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int Cube2Parser::yyeof_ = 0;
  const int Cube2Parser::yylast_ = 172;
  const int Cube2Parser::yynnts_ = 83;
  const int Cube2Parser::yyempty_ = -2;
  const int Cube2Parser::yyfinal_ = 6;
  const int Cube2Parser::yyterror_ = 1;
  const int Cube2Parser::yyerrcode_ = 256;
  const int Cube2Parser::yyntokens_ = 93;

  const unsigned int Cube2Parser::yyuser_token_number_max_ = 347;
  const Cube2Parser::token_number_type Cube2Parser::yyundef_token_ = 2;


/* Line 1054 of lalr1.cc  */
#line 1 "[Bison:b4_percent_define_default]"

} // cubeparser

/* Line 1054 of lalr1.cc  */
#line 1834 "Cube2Parser.c"


/* Line 1056 of lalr1.cc  */
#line 856 "Cube2Parser.yy"
 /*** Additional Code ***/

void cubeparser::Cube2Parser::error(const Cube2Parser::location_type& l,
			            const std::string& m)
{
    driver.error(l, m);
}





