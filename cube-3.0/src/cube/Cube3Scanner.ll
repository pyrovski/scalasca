%{ /*** C/C++ Declarations ***/

#include <string>

#include "Cube3Parser.h"
#include "Cube3Scanner.h"

typedef cubeparser::Cube3Parser::token token;
typedef cubeparser::Cube3Parser::token_type token_type;

#define yyterminate() return token::END

/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

using namespace std;
%}

/*** Flex Declarations and Options ***/

/* enable c++ scanner class generation */
%option c++

/* change the name of the scanner class. results in "Cube3FlexLexer" */
%option prefix="Cube3"

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
%x string_content
%x cdata
%x real_content
%x long_content
%x scan_error

%% /*** Regular Expressions Part ***/

 /* code at the beginning of yylex() */
%{
    // reset location
    yylloc->step();
%}

<INITIAL,attribute_name,eq,right_quota,tag,real_content,long_content>(\n)+ {
                             	yylloc->lines(yyleng); yylloc->step(); }
<INITIAL,attribute_name,eq,right_quota,tag,real_content,long_content>[ \t]+ {
                             	yylloc->step(); }
<attribute_name>"/>"        	{BEGIN(tag); 
                             	return cubeparser::Cube3Parser::token::CLOSETAG_RIGHT;}
<attribute_name>"?>"        	{BEGIN(tag); 
                             	return cubeparser::Cube3Parser::token::XML_CLOSE;}
<attribute_name>">"         	{if (parseContext->realContent) BEGIN(real_content);
                             	else if (parseContext->longContent) BEGIN(long_content);
                             	else BEGIN(tag); 
                             	return cubeparser::Cube3Parser::token::OPENTAG_RIGHT;}
<INITIAL>"<?xml"  		{BEGIN(attribute_name); 
                             	return cubeparser::Cube3Parser::token::XML_OPEN;}
<tag>"<attr" 			{BEGIN(attribute_name); 
                             	return cubeparser::Cube3Parser::token::ATTR_OPEN;}
<tag>"<dim" 			{BEGIN(attribute_name); 
                             	return cubeparser::Cube3Parser::token::DIM_OPEN;}
<tag>"<cube version=\"3.0\">"   {return cubeparser::Cube3Parser::token::CUBE_OPEN3;}
<tag>"<cube version=\"2.0\">"   {return cubeparser::Cube3Parser::token::CUBE_OPEN2;}
<tag>"</cube>" 			{return cubeparser::Cube3Parser::token::CUBE_CLOSE;}
<tag>"<metric" 			{BEGIN(attribute_name); 
                                return cubeparser::Cube3Parser::token::METRIC_OPEN;}
<tag>"</metric>" 		{return cubeparser::Cube3Parser::token::METRIC_CLOSE;}
<tag>"<cnode" 			{BEGIN(attribute_name); 
                                return cubeparser::Cube3Parser::token::CNODE_OPEN;}
<tag>"</cnode>" 		{return cubeparser::Cube3Parser::token::CNODE_CLOSE;}
<tag>"<machine" 		{BEGIN(attribute_name); 
                                return cubeparser::Cube3Parser::token::MACHINE_OPEN;}
<tag>"</machine>" 		{return cubeparser::Cube3Parser::token::MACHINE_CLOSE;}
<tag>"<node" 			{BEGIN(attribute_name); 
                                return cubeparser::Cube3Parser::token::NODE_OPEN;}
<tag>"</node>" 			{return cubeparser::Cube3Parser::token::NODE_CLOSE;}
<tag>"<process" 		{BEGIN(attribute_name); 
                                return cubeparser::Cube3Parser::token::PROCESS_OPEN;}
<tag>"</process>" 		{return cubeparser::Cube3Parser::token::PROCESS_CLOSE;}
<tag>"<thread" 			{BEGIN(attribute_name); 
                                return cubeparser::Cube3Parser::token::THREAD_OPEN;}
<tag>"</thread>" 		{return cubeparser::Cube3Parser::token::THREAD_CLOSE;}
<tag>"<cart" 			{BEGIN(attribute_name); 
                               	return cubeparser::Cube3Parser::token::CART_OPEN;}
<tag>"</cart>" 			{BEGIN(tag); return cubeparser::Cube3Parser::token::CART_CLOSE;}                              	
<tag>"<coord" 			{BEGIN(attribute_name); 
   			        parseContext->longContent = true;
                                parseContext->longValues.clear();
                                return cubeparser::Cube3Parser::token::COORD_OPEN;}
<long_content>"</coord>" 	{parseContext->longContent = false;
				BEGIN(tag);
                                return cubeparser::Cube3Parser::token::COORD_CLOSE;}
<tag>"<matrix" 			{BEGIN(attribute_name); 
                                return cubeparser::Cube3Parser::token::MATRIX_OPEN;}
<tag>"</matrix>" 		{return cubeparser::Cube3Parser::token::MATRIX_CLOSE;}
<tag>"<row" 			{BEGIN(attribute_name); 
   			        parseContext->realContent = true;
				parseContext->realValues.clear();
                                return cubeparser::Cube3Parser::token::ROW_OPEN;}
<real_content>"</row>" 		{BEGIN(tag);
				parseContext->realContent = false;
                                return cubeparser::Cube3Parser::token::ROW_CLOSE;}
<tag>"<region" 			{BEGIN(attribute_name); 
                                return cubeparser::Cube3Parser::token::REGION_OPEN;}
<tag>"</region>"               	{return cubeparser::Cube3Parser::token::REGION_CLOSE;}
<tag>"<doc>" 		       	{return cubeparser::Cube3Parser::token::DOC_OPEN;}
<tag>"</doc>" 		       	{return cubeparser::Cube3Parser::token::DOC_CLOSE;}
<tag>"<mirrors>" 	       	{return cubeparser::Cube3Parser::token::MIRRORS_OPEN;}
<tag>"</mirrors>" 	       	{return cubeparser::Cube3Parser::token::MIRRORS_CLOSE;}
<tag>"<murl>" 	               	{BEGIN(string_content);
			     	parseContext->str.str("");
                             	return cubeparser::Cube3Parser::token::MURL_OPEN;}
<string_content>"</murl>"      	{BEGIN(tag);
                             	return cubeparser::Cube3Parser::token::MURL_CLOSE;}
<tag>"<metrics>" 	       	{return cubeparser::Cube3Parser::token::METRICS_OPEN;}
<tag>"</metrics>" 		{return cubeparser::Cube3Parser::token::METRICS_CLOSE;}
<tag>"<disp_name>" 		{BEGIN(string_content);
			     	parseContext->str.str("");
                             	return cubeparser::Cube3Parser::token::DISP_NAME_OPEN;}
<string_content>"</disp_name>" 	{BEGIN(tag);
                             	return cubeparser::Cube3Parser::token::DISP_NAME_CLOSE;}
<tag>"<uniq_name>" 		{BEGIN(string_content);
			     	parseContext->str.str("");
                             	return cubeparser::Cube3Parser::token::UNIQ_NAME_OPEN;}
<string_content>"</uniq_name>" 	{BEGIN(tag);
                             	return cubeparser::Cube3Parser::token::UNIQ_NAME_CLOSE;}
<tag>"<dtype>" 			{BEGIN(string_content);
			     	parseContext->str.str("");
                             	return cubeparser::Cube3Parser::token::DTYPE_OPEN;}
<string_content>"</dtype>" 	{BEGIN(tag);
                             	return cubeparser::Cube3Parser::token::DTYPE_CLOSE;}
<tag>"<uom>" 			{BEGIN(string_content);
			     	parseContext->str.str("");
                             	return cubeparser::Cube3Parser::token::UOM_OPEN;}
<string_content>"</uom>" 	{BEGIN(tag);
                             	return cubeparser::Cube3Parser::token::UOM_CLOSE;}
<tag>"<val>" 			{BEGIN(string_content);
			     	parseContext->str.str("");
                             	return cubeparser::Cube3Parser::token::VAL_OPEN;}
<string_content>"</val>" 	{BEGIN(tag);
                             	return cubeparser::Cube3Parser::token::VAL_CLOSE;}
<tag>"<url>" 			{BEGIN(string_content);
			     	parseContext->str.str("");
                             	return cubeparser::Cube3Parser::token::URL_OPEN;}
<string_content>"</url>" 	{BEGIN(tag);
                             	return cubeparser::Cube3Parser::token::URL_CLOSE;}
<tag>"<descr>" 			{BEGIN(string_content);
			     	parseContext->str.str("");
                             	return cubeparser::Cube3Parser::token::DESCR_OPEN;}
<string_content>"</descr>" 	{BEGIN(tag);
                             	return cubeparser::Cube3Parser::token::DESCR_CLOSE;}
<tag>"<program>"         	{return cubeparser::Cube3Parser::token::PROGRAM_OPEN;}
<tag>"</program>"        	{return cubeparser::Cube3Parser::token::PROGRAM_CLOSE;}
<tag>"<name>" 			{BEGIN(string_content);
			     	parseContext->str.str("");
                             	return cubeparser::Cube3Parser::token::NAME_OPEN;}
<string_content>"</name>" 	{BEGIN(tag);
                             	return cubeparser::Cube3Parser::token::NAME_CLOSE;}
<tag>"<system>"          	{return cubeparser::Cube3Parser::token::SYSTEM_OPEN;}
<tag>"</system>"         	{return cubeparser::Cube3Parser::token::SYSTEM_CLOSE;}
<tag>"<rank>" 			{BEGIN(long_content);
				 parseContext->longValues.clear();
				 return cubeparser::Cube3Parser::token::RANK_OPEN;}
<long_content>"</rank>" 	{BEGIN(tag);
                                 return cubeparser::Cube3Parser::token::RANK_CLOSE;}
<tag>"<topologies>"      	{return cubeparser::Cube3Parser::token::TOPOLOGIES_OPEN;}
<tag>"</topologies>"     	{return cubeparser::Cube3Parser::token::TOPOLOGIES_CLOSE;}
<tag>"<severity>"        	{return cubeparser::Cube3Parser::token::SEVERITY_OPEN;}
<tag>"</severity>"          	{return cubeparser::Cube3Parser::token::SEVERITY_CLOSE;}
<attribute_name>"version"   	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::STRINGCONTENT;
			     	return cubeparser::Cube3Parser::token::ANAME_VERSION;}
<attribute_name>"encoding"  	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::STRINGCONTENT;
			     	return cubeparser::Cube3Parser::token::ANAME_ENCODING;}
<attribute_name>"key"       	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::STRINGCONTENT;
			     	return cubeparser::Cube3Parser::token::ANAME_KEY;}
<attribute_name>"value"     	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::STRINGCONTENT;
			     	return cubeparser::Cube3Parser::token::ANAME_VALUE;}
<attribute_name>"id"|"Id"   	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::INTCONTENT;
			     	return cubeparser::Cube3Parser::token::ANAME_ID;}
<attribute_name>"mod"       	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::STRINGCONTENT;
			     	return cubeparser::Cube3Parser::token::ANAME_MOD;}                             	     	
<attribute_name>"begin"     	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::INTCONTENT;
			     	return cubeparser::Cube3Parser::token::ANAME_BEGIN;}
<attribute_name>"end"       	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::INTCONTENT;
			     	return cubeparser::Cube3Parser::token::ANAME_END;}
<attribute_name>"line"       	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::INTCONTENT;
			     	return cubeparser::Cube3Parser::token::ANAME_LINE;}
<attribute_name>"calleeId"  	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::INTCONTENT;
			     	return cubeparser::Cube3Parser::token::ANAME_CALLEEID;}
<attribute_name>"name"         {BEGIN(eq); 
                                parseContext->contentType = ParseContext::STRINGCONTENT;
                                return cubeparser::Cube3Parser::token::ANAME_NAME;
                                }				     	
<attribute_name>"ndims"     	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::INTCONTENT;
			     	return cubeparser::Cube3Parser::token::ANAME_NDIMS;}
<attribute_name>"size"      	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::INTCONTENT;
			     	return cubeparser::Cube3Parser::token::ANAME_SIZE;}
<attribute_name>"periodic"  	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::BOOLCONTENT;
			     	return cubeparser::Cube3Parser::token::ANAME_PERIODIC;}
<attribute_name>"thrdId"    	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::INTCONTENT;
			     	return cubeparser::Cube3Parser::token::ANAME_THRDID;}
<attribute_name>"procId"    	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::INTCONTENT;
			     	return cubeparser::Cube3Parser::token::ANAME_PROCID;}
<attribute_name>"nodeId"    	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::INTCONTENT;
			     	return cubeparser::Cube3Parser::token::ANAME_NODEID;}
<attribute_name>"machId"    	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::INTCONTENT;
			     	return cubeparser::Cube3Parser::token::ANAME_MACHID;}
<attribute_name>"metricId"  	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::INTCONTENT;
			     	return cubeparser::Cube3Parser::token::ANAME_METRICID;}
<attribute_name>"cnodeId"   	{BEGIN(eq); 
                             	parseContext->contentType = ParseContext::INTCONTENT;
			     	return cubeparser::Cube3Parser::token::ANAME_CNODEID;}
<eq>"=\""                     	{if (parseContext->contentType==ParseContext::STRINGCONTENT){ 
					BEGIN(quoted_string);
                             		parseContext->str.str("");
				} else if (parseContext->contentType==ParseContext::INTCONTENT){
					BEGIN(quoted_int);
					parseContext->str.str("");
			     	} else BEGIN(quoted_bool); }
<right_quota>"\""               {BEGIN(attribute_name); }
<quoted_string>[^\"\n]+       	{parseContext->str << yytext;}
<quoted_string>[\n]+       	{parseContext->str << " ";}
<quoted_string>"\""       	{BEGIN(attribute_name); 
                             	return cubeparser::Cube3Parser::token::ATTRIBUTE_VALUE;}
<quoted_int>[^\" \t\n]+  	{
			        char* remain;
				parseContext->longAttribute = strtol(yytext,&remain,10);
				bool ok = (*remain=='\0');
				if (!ok) {
				  BEGIN(scan_error);
				  *yyout << "unexpected string: \"" << yytext << "\"";  
				} else {
				  BEGIN(right_quota);
                             	  return cubeparser::Cube3Parser::token::ATTRIBUTE_VALUE;}
				}
<quoted_bool>[fF][aA][lL][sS][eE]\" {
				BEGIN(attribute_name); 
                             	return cubeparser::Cube3Parser::token::FFALSE;}
<quoted_bool>[tT][rR][uU][eE]\" {
				BEGIN(attribute_name); 
                             	return cubeparser::Cube3Parser::token::TTRUE;}
<real_content>[^< \t\n]+	{
				  if (!parseContext->ignoreMetric){
				    char* remain;
				    double value = strtod(yytext,&remain);
				    if (*remain!='\0'){
	                     		*yyout << "unexpected string \"" << yytext << "\" for double" << std::endl;
                             		BEGIN(scan_error);
				    } else {
                             	    	parseContext->realValues.push_back(value); 
				    }
				  }
				}
<long_content>[^< \t\n]+ 	{
				  char* remain;
				  long value = strtol(yytext,&remain,10);
				  if (*remain!='\0'){
	                     		*yyout << "unexpected string \"" << yytext << "\" for long" << std::endl;
                             		BEGIN(scan_error);
				  } else {
                             		parseContext->longValues.push_back(value); 
				  }
				}
<string_content>[^<gla\n]+     	{parseContext->str << yytext; }
<string_content>"g"    	        {parseContext->str << yytext; }
<string_content>"l"    	        {parseContext->str << yytext; }
<string_content>"a"    	        {parseContext->str << yytext; }
<string_content>(\n)+       	{parseContext->str << " "; }
<string_content>"<![CDATA[" 	{BEGIN(cdata); }
<cdata>[^\]]+               	{parseContext->str << yytext; }
<cdata>"\]\]>"              	{BEGIN(string_content); }
<cdata>"\]"                 	{parseContext->str << yytext; }
<INITIAL,attribute_name,eq,right_quota,quoted_string,quoted_bool,quoted_int,tag,string_content,cdata,real_content,long_content>. {
                             	*yyout << "unexpected character " << yytext << std::endl;
                             	BEGIN(scan_error);}
<scan_error>[^\n]* 		{*yyout << "Before: " << yytext << std::endl;
                   		return cubeparser::Cube3Parser::token::ERROR;}
 /*  */
 /* \special symbols are handeled by function "services::escapeFromXML()" */
 /* < string_content>"&gt;" 	        {parseContext->str << ">"; } */
 /* <string_content>"&lt;" 	        {parseContext->str << "<"; } */
 /* <string_content>"&amp;"	        {parseContext->str << "&"; }  */
 

%% /*** Additional Code ***/

namespace cubeparser {

Cube3Scanner::Cube3Scanner(std::istream* in,
		           std::ostream* out,
			   ParseContext* parseContext)
  : Cube3FlexLexer(in, out),
    parseContext(parseContext)
{
}

Cube3Scanner::~Cube3Scanner()
{
}

}

/* This implementation of Cube3FlexLexer::yylex() is required to fill the
 * vtable of the class Cube3FlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the Cube3Scanner class instead. */

#ifdef yylex
#undef yylex
#endif

int Cube3FlexLexer::yylex()
{
    std::cerr << "in Cube3FlexLexer::yylex() !" << std::endl;
    return 0;
}
