%{ /*** C/C++ Declarations ***/

#include <string>

#include "Cube2Parser.h"
#include "Cube2Scanner.h"

/* import the parser's token type into a local typedef */
typedef cubeparser::Cube2Parser::token token;
typedef cubeparser::Cube2Parser::token_type token_type;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. */
#define yyterminate() return token::END

/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

using namespace std;
%}

/*** Flex Declarations and Options ***/

/* enable c++ scanner class generation */
%option c++

/* change the name of the scanner class. results in "Cube2FlexLexer" */
%option prefix="Cube2"

/* the manual says "somewhat more optimized" */
%option batch

/* enable scanner to generate debug output. */
%option debug

/* no support for include files is planned */
%option noyywrap 

/* unput is not used */
/*%option nounput */

/* The following paragraph suffices to track locations accurately. Each time
 * yylex is invoked, the begin position is moved onto the end position. */
%{
#define YY_USER_ACTION  yylloc->columns(yyleng);
%}


%x attribute_name 
%x eq 
%x right_quota 
%x quoted_string 
%x quoted_bool
%x quoted_int 
%x tag 
%x string_value
%x long_value
%x long_values
%x real_values
%x cdata

%x scan_error

%% /*** Regular Expressions Part ***/

 /* code to place at the beginning of yylex() */
%{
    // reset location
    yylloc->step();
%}

<INITIAL,attribute_name,eq,right_quota,tag,real_values,long_values,long_value>(\n)+ {
                             	yylloc->lines(yyleng); yylloc->step(); }
<INITIAL,attribute_name,eq,right_quota,tag,real_values,long_values,long_value>[ \t]+ {
                             	yylloc->step(); }
<attribute_name>"/>"        	{BEGIN(tag); 
                             	return cubeparser::Cube2Parser::token::CLOSETAG_RIGHT;}
<attribute_name>"?>"        	{BEGIN(tag); 
                             	return cubeparser::Cube2Parser::token::XML_CLOSE;}
<attribute_name>">"         	{if (parseContext->realContent) BEGIN(real_values);
                             	else if (parseContext->longContent) BEGIN(long_values);
                             	else BEGIN(tag); 
                             	return cubeparser::Cube2Parser::token::OPENTAG_RIGHT;}
<INITIAL>"<?xml"  		{BEGIN(attribute_name); 
                             	return cubeparser::Cube2Parser::token::XML_OPEN;}
<tag>"<attr" 			{BEGIN(attribute_name); 
                             	return cubeparser::Cube2Parser::token::ATTR_OPEN;}
<tag>"<dim" 			{BEGIN(attribute_name); 
                             	return cubeparser::Cube2Parser::token::DIM_OPEN;}
<tag>"<cube version=\"2.0\">"   {return cubeparser::Cube2Parser::token::CUBE_OPEN2;}
<tag>"</cube>" 			{return cubeparser::Cube2Parser::token::CUBE_CLOSE;}
<tag>"<metric" 			{BEGIN(attribute_name); 
                                return cubeparser::Cube2Parser::token::METRIC_OPEN;}
<tag>"</metric>" 		{return cubeparser::Cube2Parser::token::METRIC_CLOSE;}
<tag>"<cnode" 			{BEGIN(attribute_name); 
                                return cubeparser::Cube2Parser::token::CNODE_OPEN;}
<tag>"</cnode>" 		{return cubeparser::Cube2Parser::token::CNODE_CLOSE;}
<tag>"<machine" 		{BEGIN(attribute_name); 
                                return cubeparser::Cube2Parser::token::MACHINE_OPEN;}
<tag>"</machine>" 		{return cubeparser::Cube2Parser::token::MACHINE_CLOSE;}
<tag>"<node" 			{BEGIN(attribute_name); 
                                return cubeparser::Cube2Parser::token::NODE_OPEN;}
<tag>"</node>" 			{return cubeparser::Cube2Parser::token::NODE_CLOSE;}
<tag>"<process" 		{BEGIN(attribute_name); 
                                return cubeparser::Cube2Parser::token::PROCESS_OPEN;}
<tag>"</process>" 		{return cubeparser::Cube2Parser::token::PROCESS_CLOSE;}
<tag>"<thread" 			{BEGIN(attribute_name); 
                                return cubeparser::Cube2Parser::token::THREAD_OPEN;}
<tag>"</thread>" 		{return cubeparser::Cube2Parser::token::THREAD_CLOSE;}
<tag>"<cart" 			{BEGIN(attribute_name); 
                               	return cubeparser::Cube2Parser::token::CART_OPEN;}
<tag>"</cart>" 			{return cubeparser::Cube2Parser::token::CART_CLOSE;}
<tag>"<coord" 			{BEGIN(attribute_name); 
   			        parseContext->longContent = true;
                                parseContext->longValues.clear(); 
                                return cubeparser::Cube2Parser::token::COORD_OPEN;}
<long_values>"</coord>" 	{parseContext->longContent = false;
				BEGIN(tag);
                                return cubeparser::Cube2Parser::token::COORD_CLOSE;}
<tag>"<matrix" 			{BEGIN(attribute_name); 
                                return cubeparser::Cube2Parser::token::MATRIX_OPEN;}
<tag>"</matrix>" 		{return cubeparser::Cube2Parser::token::MATRIX_CLOSE;}
<tag>"<row" 			{BEGIN(attribute_name); 
   			        parseContext->realContent = true;
				parseContext->realValues.clear();
                                return cubeparser::Cube2Parser::token::ROW_OPEN;}
<real_values>"</row>" 		{BEGIN(tag);
				parseContext->realContent = false;
                                return cubeparser::Cube2Parser::token::ROW_CLOSE;}
<tag>"<module" 			{BEGIN(attribute_name); 
                                return cubeparser::Cube2Parser::token::MODULE_OPEN;}
<tag>"</module>" 		{return cubeparser::Cube2Parser::token::MODULE_CLOSE;}
<tag>"<region" 			{BEGIN(attribute_name); 
                                return cubeparser::Cube2Parser::token::REGION_OPEN;}
<tag>"</region>"               	{return cubeparser::Cube2Parser::token::REGION_CLOSE;}
<tag>"<csite" 		       	{BEGIN(attribute_name); 
                                return cubeparser::Cube2Parser::token::CSITE_OPEN;}
<tag>"</csite>" 	       	{return cubeparser::Cube2Parser::token::CSITE_CLOSE;}
<tag>"<begin>"                 	{BEGIN(long_value); 
                                return cubeparser::Cube2Parser::token::BEGIN_OPEN;}
<tag>"</begin>"     		{return cubeparser::Cube2Parser::token::BEGIN_CLOSE;}
<tag>"<end>" 		       	{BEGIN(long_value); 
                                return cubeparser::Cube2Parser::token::END_OPEN;}
<tag>"</end>"       		{return cubeparser::Cube2Parser::token::END_CLOSE;}
<tag>"<mod>" 		       	{BEGIN(long_value); 
                                return cubeparser::Cube2Parser::token::MOD_OPEN;}
<tag>"</mod>"       		{return cubeparser::Cube2Parser::token::MOD_CLOSE;}
<tag>"<line>" 		       	{BEGIN(long_value); 
                                return cubeparser::Cube2Parser::token::LINE_OPEN;}
<tag>"</line>"      		{return cubeparser::Cube2Parser::token::LINE_CLOSE;}
<tag>"<callee>" 	       	{BEGIN(long_value); 
                                return cubeparser::Cube2Parser::token::CALLEE_OPEN;}
<tag>"</callee>"    		{return cubeparser::Cube2Parser::token::CALLEE_CLOSE;}
<tag>"<doc>" 		       	{return cubeparser::Cube2Parser::token::DOC_OPEN;}
<tag>"</doc>" 		       	{return cubeparser::Cube2Parser::token::DOC_CLOSE;}
<tag>"<mirrors>" 	       	{return cubeparser::Cube2Parser::token::MIRRORS_OPEN;}
<tag>"</mirrors>" 	       	{return cubeparser::Cube2Parser::token::MIRRORS_CLOSE;}
<tag>"<murl>" 	               	{BEGIN(string_value);
			     	parseContext->str.str("");
                             	return cubeparser::Cube2Parser::token::MURL_OPEN;}
<string_value>"</murl>"      	{BEGIN(tag);
                             	return cubeparser::Cube2Parser::token::MURL_CLOSE;}
<tag>"<behavior>" 	       	{return cubeparser::Cube2Parser::token::BEHAVIOR_OPEN;}
<tag>"</behavior>" 	       	{return cubeparser::Cube2Parser::token::BEHAVIOR_CLOSE;}
<tag>"<name>" 			{BEGIN(string_value);
			     	parseContext->str.str("");
                             	return cubeparser::Cube2Parser::token::NAME_OPEN;}
<string_value>"</name>" 	{BEGIN(tag);
                             	return cubeparser::Cube2Parser::token::NAME_CLOSE;}
<tag>"<dtype>" 			{BEGIN(string_value);
			     	parseContext->str.str("");
                             	return cubeparser::Cube2Parser::token::DTYPE_OPEN;}
<string_value>"</dtype>" 	{BEGIN(tag);
                             	return cubeparser::Cube2Parser::token::DTYPE_CLOSE;}
<tag>"<uom>" 			{BEGIN(string_value);
			     	parseContext->str.str("");
                             	return cubeparser::Cube2Parser::token::UOM_OPEN;}
<string_value>"</uom>" 		{BEGIN(tag);
                             	return cubeparser::Cube2Parser::token::UOM_CLOSE;}
<tag>"<val>" 			{BEGIN(string_value);
			     	parseContext->str.str("");
                             	return cubeparser::Cube2Parser::token::VAL_OPEN;}
<string_value>"</val>" 		{BEGIN(tag);
                             	return cubeparser::Cube2Parser::token::VAL_CLOSE;}
<tag>"<url>" 			{BEGIN(string_value);
			     	parseContext->str.str("");
                             	return cubeparser::Cube2Parser::token::URL_OPEN;}
<string_value>"</url>" 		{BEGIN(tag);
                             	return cubeparser::Cube2Parser::token::URL_CLOSE;}
<tag>"<descr>" 			{BEGIN(string_value);
			     	parseContext->str.str("");
                             	return cubeparser::Cube2Parser::token::DESCR_OPEN;}
<string_value>"</descr>" 	{BEGIN(tag);
                             	return cubeparser::Cube2Parser::token::DESCR_CLOSE;}
<tag>"<program>"         	{return cubeparser::Cube2Parser::token::PROGRAM_OPEN;}
<tag>"</program>"        	{return cubeparser::Cube2Parser::token::PROGRAM_CLOSE;}
<tag>"<locations>"          	{return cubeparser::Cube2Parser::token::LOCATIONS_OPEN;}
<tag>"</locations>"         	{return cubeparser::Cube2Parser::token::LOCATIONS_CLOSE;}
<tag>"<rank>" 			{BEGIN(long_value);
                             	return cubeparser::Cube2Parser::token::RANK_OPEN;}
<tag>"</rank>" 			{return cubeparser::Cube2Parser::token::RANK_CLOSE;}
<tag>"<topologies>"      	{return cubeparser::Cube2Parser::token::TOPOLOGIES_OPEN;}
<tag>"</topologies>"     	{return cubeparser::Cube2Parser::token::TOPOLOGIES_CLOSE;}
<tag>"<severity>"        	{return cubeparser::Cube2Parser::token::SEVERITY_OPEN;}
<tag>"</severity>"          	{return cubeparser::Cube2Parser::token::SEVERITY_CLOSE;}
<attribute_name>"version"   	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::STRINGCONTENT;
			     	return cubeparser::Cube2Parser::token::ANAME_VERSION;}
<attribute_name>"encoding"  	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::STRINGCONTENT;
			     	return cubeparser::Cube2Parser::token::ANAME_ENCODING;}
<attribute_name>"key"       	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::STRINGCONTENT;
			     	return cubeparser::Cube2Parser::token::ANAME_KEY;}
<attribute_name>"value"     	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::STRINGCONTENT;
			     	return cubeparser::Cube2Parser::token::ANAME_VALUE;}
<attribute_name>"id"|"Id"   	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::INTCONTENT;
			     	return cubeparser::Cube2Parser::token::ANAME_ID;}
<attribute_name>"csiteId"   	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::INTCONTENT;
			     	return cubeparser::Cube2Parser::token::ANAME_CSITEID;}
<attribute_name>"locid"|"locId"	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::INTCONTENT;
			     	return cubeparser::Cube2Parser::token::ANAME_LOCID;}
<attribute_name>"ndims"     	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::INTCONTENT;
			     	return cubeparser::Cube2Parser::token::ANAME_NDIMS;}
<attribute_name>"size"      	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::INTCONTENT;
			     	return cubeparser::Cube2Parser::token::ANAME_SIZE;}
<attribute_name>"periodic"  	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::BOOLCONTENT;
			     	return cubeparser::Cube2Parser::token::ANAME_PERIODIC;}
<attribute_name>"metricId"  	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::INTCONTENT;
			     	return cubeparser::Cube2Parser::token::ANAME_METRICID;}
<attribute_name>"cnodeId"   	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::INTCONTENT;
			     	return cubeparser::Cube2Parser::token::ANAME_CNODEID;}
<eq>"=\""                     	{if (parseContext->contentType==ParseContext::STRINGCONTENT){ 
					BEGIN(quoted_string);
				     	parseContext->str.str("");
				} else if (parseContext->contentType==ParseContext::INTCONTENT) {
					BEGIN(quoted_int);
			     		parseContext->str.str("");
			     	} else BEGIN(quoted_bool); }
<right_quota>"\""               {BEGIN(attribute_name); }
<quoted_string>[^\"\n]+       	{parseContext->str << yytext;}
<quoted_string>[\n]+       	{parseContext->str <<  " ";}
<quoted_string>"\""       	{BEGIN(attribute_name); 
                             	return cubeparser::Cube2Parser::token::ATTRIBUTE_VALUE;}
<quoted_int>([+-]?[0-9]+([Ee][+-]?[0-9]+)?) {
				BEGIN(right_quota);
                             	parseContext->str << yytext;
                             	return cubeparser::Cube2Parser::token::ATTRIBUTE_VALUE;}
<quoted_bool>[fF][aA][lL][sS][eE]\" {
				BEGIN(attribute_name); 
                             	return cubeparser::Cube2Parser::token::FFALSE;}
<quoted_bool>[tT][rR][uU][eE]\" {
				BEGIN(attribute_name); 
                             	return cubeparser::Cube2Parser::token::TTRUE;}
<real_values>([+-]?[0-9]+"."[0-9]*([Ee][+-]?[0-9]+)?) |
([+-]?[0-9]*"."[0-9]+([Ee][+-]?[0-9]+)?) { 
                             	parseContext->realValues.push_back(atof(yytext)); }
<real_values>([+-]?[0-9]+([Ee][+-]?[0-9]+)?) {
                            	parseContext->realValues.push_back(atof(yytext)); }
<long_values>([+-]?[0-9]+([Ee][+-]?[0-9]+)?) {
                             	parseContext->longValues.push_back(atol(yytext)); }
<long_value>([+-]?[0-9]+([Ee][+-]?[0-9]+)?) {
				BEGIN(tag);
			     	parseContext->str.str("");
                             	parseContext->str << yytext; }
<string_value>[^<gla\n]+     	{parseContext->str << yytext; }
<string_value>"gt;"  	        {parseContext->str << ">"; }
<string_value>"lt;"  	        {parseContext->str << "<"; }
<string_value>"amp;" 	        {parseContext->str << "&"; }
<string_value>"g"    	        {parseContext->str << yytext; }
<string_value>"l"    	        {parseContext->str << yytext; }
<string_value>"a"    	        {parseContext->str << yytext; }
<string_value>(\n)+       	{yylloc->lines(yyleng); yylloc->step(); parseContext->str << " "; }
<string_value>"<![CDATA[" 	{BEGIN(cdata); }
<cdata>[^\]]+               	{parseContext->str << yytext; }
<cdata>"\]\]>"              	{BEGIN(string_value); }
<cdata>"\]"                 	{parseContext->str << yytext; }
<INITIAL,attribute_name,eq,right_quota,quoted_string,quoted_bool,quoted_int,tag,string_value,cdata,real_values,long_values,long_value>. {
                             	*yyout << "unexpected character " << yytext << std::endl;
                             	BEGIN(scan_error);}
<scan_error>[^\n]* 		{*yyout << "Before: " << yytext<< std::endl;
                   		return cubeparser::Cube2Parser::token::ERROR;}

%% /*** Additional Code ***/

namespace cubeparser {

Cube2Scanner::Cube2Scanner(std::istream* in,
		 std::ostream* out, ParseContext* parseContext)
  : Cube2FlexLexer(in, out),
    parseContext(parseContext)
{
}

Cube2Scanner::~Cube2Scanner()
{
}

}

/* This implementation of Cube2FlexLexer::yylex() is required to fill the
 * vtable of the class Cube2FlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the Cube2Scanner class instead. */

#ifdef yylex
#undef yylex
#endif

int Cube2FlexLexer::yylex()
{
    std::cerr << "in Cube2FlexLexer::yylex() !" << std::endl;
    return 0;
}
