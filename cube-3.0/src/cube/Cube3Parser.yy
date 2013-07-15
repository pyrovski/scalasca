%code requires{ /*** C/C++ Declarations ***/

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>
#include "services.h"
#include "ParseContext.h"
#include "Cartesian.h"

}

/*** yacc/bison Declarations ***/

/* Require bison 2.3 or later */
%require "2.3"

/* add debug output code to generated parser. */
%debug

/* start symbol is named "document" */
%start document

/* write out a header file containing the token defines */
%defines

/* use newer C++ skeleton file */
%skeleton "lalr1.cc"

/* namespace to enclose parser in */
%name-prefix="cubeparser"

/* set the parser's class identifier */
%define "parser_class_name" "Cube3Parser"

/* keep track of the current position within the input */
%locations

%initial-action
{
    // initialize the initial location object
    @$.begin.filename = @$.end.filename = &driver.streamname;
};

/* The driver is passed by reference to the parser and to the scanner. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class Driver& driver }
%parse-param { class ParseContext& parseContext }
%parse-param { class Cube3Scanner& cube3Lexer }
%parse-param { class cube::Cube& cube }

/* verbose error messages */
%error-verbose

 /*** the grammar's tokens ***/
%union
{ };

%token FFALSE              "false"
%token TTRUE               "true"
%token ATTRIBUTE_VALUE     "attribute value"
%token END 0               "end of file"
%token ERROR               "error"

%token OPENTAG_RIGHT       ">" 
%token CLOSETAG_RIGHT      "/>" 

%token ATTR_OPEN           "<attr"
%token DIM_OPEN            "<dim"

%token XML_OPEN            "<?xml"
%token XML_CLOSE           "?>"

%token CUBE_OPEN3          "<cube version=\"3.0\""
%token CUBE_OPEN2          "<cube version=\"2.0\""
%token CUBE_CLOSE          "</cube>"
%token METRIC_OPEN         "<metric"
%token METRIC_CLOSE        "</metric>"
%token REGION_OPEN         "<region"
%token REGION_CLOSE        "</region>"
%token CNODE_OPEN          "<cnode"
%token CNODE_CLOSE         "</cnode>"
%token MACHINE_OPEN        "<machine"
%token MACHINE_CLOSE       "</machine>"
%token NODE_OPEN           "<node"
%token NODE_CLOSE          "</node>"
%token PROCESS_OPEN        "<process"
%token PROCESS_CLOSE       "</process>"
%token THREAD_OPEN         "<thread"
%token THREAD_CLOSE        "</thread>"
%token MATRIX_OPEN         "<matrix"
%token MATRIX_CLOSE        "</matrix>"
%token ROW_OPEN            "<row"
%token ROW_CLOSE           "</row>"
%token CART_OPEN           "<cart"
%token CART_CLOSE          "</cart>"
%token COORD_OPEN          "<coord"
%token COORD_CLOSE         "</coord>"

%token DOC_OPEN            "<doc>"
%token DOC_CLOSE           "</doc>"
%token MIRRORS_OPEN        "<mirrors>"
%token MIRRORS_CLOSE       "</mirrors>"
%token MURL_OPEN           "<murl>"
%token MURL_CLOSE          "</murl>"
%token METRICS_OPEN	   "<metrics>"
%token METRICS_CLOSE	   "</metrics>"
%token DISP_NAME_OPEN      "<disp_name>"
%token DISP_NAME_CLOSE     "</disp_name>"
%token UNIQ_NAME_OPEN      "<uniq_name>"
%token UNIQ_NAME_CLOSE     "</uniq_name>"
%token DTYPE_OPEN          "<dtype>"
%token DTYPE_CLOSE         "</dtype>"
%token UOM_OPEN            "<uom>"
%token UOM_CLOSE           "</uom>"
%token VAL_OPEN            "<val>"
%token VAL_CLOSE           "</val>"
%token URL_OPEN            "<url>"
%token URL_CLOSE           "</url>"
%token DESCR_OPEN          "<descr>"
%token DESCR_CLOSE         "</descr>"
%token PROGRAM_OPEN        "<program>"
%token PROGRAM_CLOSE       "</program>"
%token NAME_OPEN           "<name>"
%token NAME_CLOSE          "</name>"
%token SYSTEM_OPEN         "<system>"
%token SYSTEM_CLOSE        "</system>"
%token RANK_OPEN           "<rank>"
%token RANK_CLOSE          "</rank>"
%token TOPOLOGIES_OPEN     "<topologies>"
%token TOPOLOGIES_CLOSE    "</topologies>"
%token SEVERITY_OPEN       "<severity>"
%token SEVERITY_CLOSE      "</severity>"

%token ANAME_VERSION       "attribute name version"
%token ANAME_ENCODING      "attribute name encoding"
%token ANAME_KEY           "attribute name key"
%token ANAME_VALUE         "attribute name value"
%token ANAME_ID            "attribute name id"
%token ANAME_MOD           "attribute name mod"
%token ANAME_BEGIN         "attribute name begin"
%token ANAME_END           "attribute name end"
%token ANAME_LINE          "attribute name line"
%token ANAME_CALLEEID      "attribute name calleeid"
%token ANAME_NAME          "attribute name name"
%token ANAME_NDIMS         "attribute name ndims"
%token ANAME_SIZE          "attribute name size"
%token ANAME_PERIODIC      "attribute name periodic"
%token ANAME_THRDID        "attribute name thrdId"
%token ANAME_PROCID        "attribute name procId"
%token ANAME_NODEID        "attribute name nodeId"
%token ANAME_MACHID        "attribute name machId"
%token ANAME_METRICID      "attribute name metricId"
%token ANAME_CNODEID       "attribute name cnodeId"

%{

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

%}

%% /*** Grammar Rules ***/

/***basic value fields ***/

/*** define the attribute values ***/

/* string attributes */

version_attr :
ANAME_VERSION ATTRIBUTE_VALUE { 
	if (parseContext.versionSeen)
	  error(yylloc,"Multiple version attributes defines!"); 
	else parseContext.versionSeen = true;
	parseContext.version = parseContext.str.str(); 
};
encoding_attr :
ANAME_ENCODING ATTRIBUTE_VALUE { 
	if (parseContext.encodingSeen)
	  error(yylloc,"Multiple encoding attributes defines!"); 
	else parseContext.encodingSeen = true;
	parseContext.encoding = parseContext.str.str(); 
};
key_attr :
ANAME_KEY ATTRIBUTE_VALUE { 
	if (parseContext.keySeen)
	  error(yylloc,"Multiple key attributes defines!"); 
	else parseContext.keySeen = true;
	parseContext.key = parseContext.str.str(); 
};
value_attr :
ANAME_VALUE ATTRIBUTE_VALUE { 
	if (parseContext.valueSeen)
	  error(yylloc,"Multiple value attributes defines!"); 
	else parseContext.valueSeen = true;	
	parseContext.value = parseContext.str.str(); 
};
mod_attr :          
ANAME_MOD ATTRIBUTE_VALUE {  
	if (parseContext.modSeen)
	  error(yylloc,"Multiple module attributes defines!"); 
	else parseContext.modSeen = true;
	parseContext.mod = parseContext.str.str(); }
;

cart_name_attr :          
ANAME_NAME ATTRIBUTE_VALUE {  
    if (parseContext.cartNameSeen)
      error(yylloc,"Multiple topology names  defines!"); 
    else parseContext.cartNameSeen = true;
    parseContext.cartName = parseContext.str.str(); 
}
;


dim_name_attr :    
ANAME_NAME ATTRIBUTE_VALUE {
	if (parseContext.dimNameSeen) 
	  error(yylloc,"Multiple ndim attributes defines!"); 
	else parseContext.dimNameSeen = true;
	  parseContext.dimNameVec.push_back(parseContext.str.str()); 
};


/* int attributes */

id_attr :    
ANAME_ID ATTRIBUTE_VALUE { 
	if (parseContext.idSeen) 
	  error(yylloc,"Multiple id attributes defines!"); 
	else parseContext.idSeen = true;
	if (parseContext.longAttribute<0) error(yylloc,"Ids must be non-negative!"); 
	parseContext.id = (int)parseContext.longAttribute; 
};

calleeid_attr :         
ANAME_CALLEEID ATTRIBUTE_VALUE { 
	if (parseContext.calleeidSeen) 
	  error(yylloc,"Multiple callee id attributes defines!"); 
	else parseContext.calleeidSeen = true;
	if (parseContext.longAttribute<0) 
	  error(yylloc,"Callee ids of regions must be non-negative!"); 
	parseContext.calleeid = (int)parseContext.longAttribute; 
};

thrdid_attr :    
ANAME_THRDID ATTRIBUTE_VALUE { 
	if (parseContext.thrdidSeen) 
	  error(yylloc,"Multiple thread id attributes defines!"); 
	else parseContext.thrdidSeen = true;
	if (parseContext.longAttribute<0) error(yylloc,"Thread ids must be non-negative!"); 
	parseContext.thrdid = (int)parseContext.longAttribute; 
};

procid_attr :    
ANAME_PROCID ATTRIBUTE_VALUE { 
	if (parseContext.procidSeen) 
	  error(yylloc,"Multiple process id attributes defines!"); 
	else parseContext.procidSeen = true;
	if (parseContext.longAttribute<0) error(yylloc,"Process ids must be non-negative!"); 
	parseContext.procid = (int)parseContext.longAttribute; 
};

nodeid_attr :    
ANAME_NODEID ATTRIBUTE_VALUE { 
	if (parseContext.nodeidSeen) 
	  error(yylloc,"Multiple node id attributes defines!"); 
	else parseContext.nodeidSeen = true;
	if (parseContext.longAttribute<0) error(yylloc,"Node ids must be non-negative!"); 
	parseContext.nodeid = (int)parseContext.longAttribute; 
};

machid_attr :    
ANAME_MACHID ATTRIBUTE_VALUE { 
	if (parseContext.machidSeen) 
	  error(yylloc,"Multiple machine id attributes defines!"); 
	else parseContext.machidSeen = true;
	if (parseContext.longAttribute<0) error(yylloc,"Machine ids must be non-negative!"); 
	parseContext.machid = (int)parseContext.longAttribute; 
};

metricid_attr :       
ANAME_METRICID ATTRIBUTE_VALUE { 
	if (parseContext.metricidSeen) 
	  error(yylloc,"Multiple metric id attributes defines!"); 
	else parseContext.metricidSeen = true;
	if (parseContext.longAttribute<0) error(yylloc,"Metric ids must be non-negative!"); 
	parseContext.metricid = parseContext.longAttribute; 
};

cnodeid_attr :    
ANAME_CNODEID ATTRIBUTE_VALUE { 
	if (parseContext.cnodeidSeen) 
	  error(yylloc,"Multiple cnode id attributes defines!"); 
	else parseContext.cnodeidSeen = true;
	if (parseContext.longAttribute<0) error(yylloc,"Cnode ids must be non-negative!"); 
	parseContext.cnodeid = (int)parseContext.longAttribute; 
};

/* long attributes */

begin_attr :         
ANAME_BEGIN ATTRIBUTE_VALUE { 
	if (parseContext.beginSeen) 
	  error(yylloc,"Multiple begin attributes defines!"); 
	else parseContext.beginSeen = true;
	parseContext.beginln = parseContext.longAttribute; }
;

end_attr :       
ANAME_END ATTRIBUTE_VALUE { 
	if (parseContext.endSeen) 
	  error(yylloc,"Multiple end attributes defines!"); 
	else parseContext.endSeen = true;
	parseContext.endln = parseContext.longAttribute; }
;

line_attr :       
ANAME_LINE ATTRIBUTE_VALUE { 
	if (parseContext.lineSeen) 
	  error(yylloc,"Multiple line attributes defines!"); 
	else parseContext.lineSeen = true;
	parseContext.line = parseContext.longAttribute; }
;

 
ndims_attr :    
ANAME_NDIMS ATTRIBUTE_VALUE {
	if (parseContext.ndimsSeen) 
	  error(yylloc,"Multiple ndims attributes defines!"); 
	else parseContext.ndimsSeen = true;
	if (parseContext.longAttribute<=0)
	   error(yylloc,"Topology dimensions must be positive numbers!");
	parseContext.ndims = parseContext.longAttribute;
};
 


size_attr :       
ANAME_SIZE ATTRIBUTE_VALUE { 
	if (parseContext.sizeSeen) 
	  error(yylloc,"Multiple size attributes defines!"); 
	else parseContext.sizeSeen = true;
	if (parseContext.longAttribute<=0)
	  error(yylloc,"Dimension sizes must be positive numbers!");
	parseContext.dimVec.push_back(parseContext.longAttribute);
};

/* boolean attributes */

periodic_attr :        
ANAME_PERIODIC FFALSE { 
	if (parseContext.periodicSeen) 
	  error(yylloc,"Multiple periodic attributes defines!"); 
	else parseContext.periodicSeen = true;
	parseContext.periodicVec.push_back(false);
}
| ANAME_PERIODIC TTRUE { 
	if (parseContext.periodicSeen) 
	  error(yylloc,"Multiple periodic attributes defined!");
	else parseContext.periodicSeen = true;
	parseContext.periodicVec.push_back(true);
}
;

/*** basic tags  ***/

murl_tag :
MURL_OPEN MURL_CLOSE {
	if (parseContext.murlSeen) 
	  error(yylloc,"Multiple murl tags defined!"); 
	else parseContext.murlSeen = true;
	parseContext.murl = parseContext.str.str(); };
disp_name_tag :
DISP_NAME_OPEN DISP_NAME_CLOSE { 
	if (parseContext.dispnameSeen) 
	  error(yylloc,"Multiple disp_name tags defined!"); 
	else parseContext.dispnameSeen = true;
	parseContext.disp_name = parseContext.str.str(); };
uniq_name_tag :
UNIQ_NAME_OPEN UNIQ_NAME_CLOSE { 
	if (parseContext.uniqnameSeen) 
	  error(yylloc,"Multiple uniq_name tags defined!"); 
	else parseContext.uniqnameSeen = true;
	parseContext.uniq_name = parseContext.str.str(); };
dtype_tag :
DTYPE_OPEN DTYPE_CLOSE { 
	if (parseContext.dtypeSeen) 
	  error(yylloc,"Multiple dtype tags defined!"); 
	else parseContext.dtypeSeen = true;
	parseContext.dtype = parseContext.str.str(); }
;
uom_tag :
UOM_OPEN UOM_CLOSE {
	if (parseContext.uomSeen) 
	  error(yylloc,"Multiple uom tags defined!"); 
	else parseContext.uomSeen = true;
	parseContext.uom = parseContext.str.str(); }
;
val_tag :
VAL_OPEN VAL_CLOSE {
	if (parseContext.valSeen) 
	  error(yylloc,"Multiple val tags defined!"); 
	else parseContext.valSeen = true;
	parseContext.val = parseContext.str.str(); }
;
url_tag :
URL_OPEN URL_CLOSE {
	if (parseContext.urlSeen) 
	  error(yylloc,"Multiple url tags defined!");
	else parseContext.urlSeen = true;
	parseContext.url = parseContext.str.str(); }
;
descr_tag_opt :
| descr_tag
;
descr_tag :
DESCR_OPEN DESCR_CLOSE {
	if (parseContext.descrSeen) 
	  error(yylloc,"Multiple descr tags defined!");
	else parseContext.descrSeen = true;
	parseContext.descr = parseContext.str.str(); }
;
name_tag :
NAME_OPEN NAME_CLOSE {
	if (parseContext.nameSeen) 
	  error(yylloc,"Multiple name tags defined!"); 
	else parseContext.nameSeen = true;
	parseContext.name = parseContext.str.str(); };

rank_tag :
RANK_OPEN RANK_CLOSE {
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
};

/*** document structure ***/

document :
xml_tag cube_tag 
;

/*** xml tag ***/

xml_tag :
XML_OPEN xml_attributes XML_CLOSE {
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
;

xml_attributes :
xml_attribute
| xml_attributes xml_attribute
;

xml_attribute :
version_attr 
| encoding_attr
;

/*** cube tag ***/

cube_tag :
cube_begin cube_content CUBE_CLOSE 
;

cube_begin :
CUBE_OPEN3 {
	parseContext.cubeVersion = 3;
} 
| CUBE_OPEN2 {
	return 1;
} 
;

cube_content :
attr_tags doc_tag metrics_tag program_tag system_tag severity_tag
| attr_tags metrics_tag program_tag system_tag severity_tag
| doc_tag  metrics_tag program_tag system_tag severity_tag
| metrics_tag program_tag system_tag severity_tag
;

/*** attribute tags ***/

attr_tags :
attr_tag
| attr_tags attr_tag
;

attr_tag :
ATTR_OPEN attr_attributes CLOSETAG_RIGHT {
	//check for attributes
	if (!parseContext.keySeen)
	  error(yylloc,"Missing key attribute!"); 
	else parseContext.keySeen = false;
	if (!parseContext.valueSeen)
	  error(yylloc,"Missing value attribute!"); 
	else parseContext.valueSeen = false;
	cube.def_attr(services::escapeFromXML(parseContext.key),services::escapeFromXML(parseContext.value));
}
;

attr_attributes :
attr_attribute
| attr_attributes attr_attribute
;

attr_attribute :
key_attr 
| value_attr
;

/*** documentation tag ***/

doc_tag :
DOC_OPEN mirrors_tag_opt DOC_CLOSE;

mirrors_tag_opt :
| mirrors_tag
;

mirrors_tag :
MIRRORS_OPEN murl_tags_opt MIRRORS_CLOSE
;

murl_tags_opt:
| murl_tags
;

murl_tags :
murl_tag {parseContext.murlSeen = false; cube.def_mirror(services::escapeFromXML(parseContext.murl));}
| murl_tags murl_tag {parseContext.murlSeen = false; cube.def_mirror(services::escapeFromXML(parseContext.murl));}
;

/*** metrics tag ***/

metrics_tag :
METRICS_OPEN metric_tag METRICS_CLOSE 
;

metric_tag :
metric_begin
| metric_tag metric_begin
| metric_tag metric_end
;


metric_begin :
METRIC_OPEN id_attr OPENTAG_RIGHT tags_of_metric_opt {
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
;

tags_of_metric_opt :
| tags_of_metric
;

tags_of_metric:
tag_of_metric
| tags_of_metric tag_of_metric
;

tag_of_metric :
  disp_name_tag 
| uniq_name_tag 
| dtype_tag 
| uom_tag 
| val_tag 
| url_tag 
| descr_tag
;

metric_end :
METRIC_CLOSE {
	if (parseContext.currentMetric==NULL)
	  error(yylloc,"Metric definitions are not correctly nested!");
	parseContext.currentMetric = 
		(parseContext.currentMetric)->get_parent();
}
;


/*** program tag ***/

program_tag :
PROGRAM_OPEN region_tags cnode_tag PROGRAM_CLOSE
;

region_tags :
region_tag
| region_tags region_tag
;

region_tag :
REGION_OPEN region_attributes OPENTAG_RIGHT tags_of_region_opt REGION_CLOSE {
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
;

tags_of_region_opt :
| tags_of_region
;

tags_of_region :
tag_of_region
| tags_of_region tag_of_region
;

tag_of_region :
  name_tag 
| url_tag 
| descr_tag
;

region_attributes :
  region_attribute
| region_attributes region_attribute
;

region_attribute :
  id_attr 
| mod_attr 
| begin_attr 
| end_attr 
;

cnode_tag :
  cnode_begin
| cnode_tag cnode_begin
| cnode_tag CNODE_CLOSE {
	if (parseContext.currentCnode==NULL)
	  error(yylloc,"Cnode definitions are not correctly nested!"); 
	parseContext.currentCnode = 
		parseContext.currentCnode->get_parent();
}
;

cnode_begin :
CNODE_OPEN cnode_attributes OPENTAG_RIGHT {
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
;

cnode_attribute :
  id_attr 
| line_attr 
| mod_attr 
| calleeid_attr 
;

cnode_attributes :
cnode_attribute
| cnode_attributes cnode_attribute
;

/*** system tag ***/

system_tag :
SYSTEM_OPEN machine_tags topologies_tag_opt SYSTEM_CLOSE
;

machine_tags :
machine_tag
| machine_tags machine_tag
;

machine_tag :
MACHINE_OPEN id_attr OPENTAG_RIGHT name_tag descr_tag_opt {
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
} node_tags MACHINE_CLOSE
;

node_tags :
node_tag
| node_tags node_tag
;

node_tag :
NODE_OPEN id_attr OPENTAG_RIGHT name_tag {
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
} process_tags NODE_CLOSE
;

process_tags :
process_tag
| process_tags process_tag
;

process_tag :
PROCESS_OPEN id_attr OPENTAG_RIGHT tags_of_process  {
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
} thread_tags PROCESS_CLOSE
;

tags_of_process :
tag_of_process
| tags_of_process tag_of_process;

tag_of_process :
  name_tag 
| rank_tag
;

thread_tags :
thread_tag
| thread_tags thread_tag
;

thread_tag :
THREAD_OPEN id_attr OPENTAG_RIGHT tags_of_process THREAD_CLOSE {
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
;

topologies_tag_opt :
| topologies_tag
;

topologies_tag :
TOPOLOGIES_OPEN {
	if (!parseContext.checkThreadIds())
	  error(yylloc,"Thread ids must cover an interval [0,n] without gap!"); 
} cart_tags_opt TOPOLOGIES_CLOSE
;

cart_tags_opt :
| cart_tags
;

cart_tags :
cart_tag
| cart_tags cart_tag
;

cart_attrs :
ndims_attr 
| cart_name_attr ndims_attr 
;

cart_tag :
cart_open cart_attrs OPENTAG_RIGHT dim_tags COORD_OPEN {
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
	    
		
} coord_tags CART_CLOSE 
;

cart_open :
CART_OPEN {parseContext.dimVec.clear();
	   parseContext.dimNameVec.clear();
	   parseContext.periodicVec.clear();
        parseContext.cartNameSeen=false;
}
;

dim_tags :
  dim_tag 
| dim_tags dim_tag
;

dim_tag :
  dim_open dim_attributes CLOSETAG_RIGHT {

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
;

dim_open: 
DIM_OPEN {
	parseContext.dimNameSeen=false;
}
;

dim_attributes :
  dim_attribute
| dim_attributes dim_attribute
;

dim_attribute :
   dim_name_attr
|  size_attr 
| periodic_attr
;

coord_tags :
coord_tag
| coord_tags COORD_OPEN coord_tag
;

coord_tag :
coord_tag_thrd
| coord_tag_proc
| coord_tag_node
| coord_tag_mach
;

coord_tag_thrd :
thrdid_attr  OPENTAG_RIGHT COORD_CLOSE {
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
;

coord_tag_proc :
procid_attr  OPENTAG_RIGHT COORD_CLOSE {
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
;

coord_tag_node :
nodeid_attr  OPENTAG_RIGHT COORD_CLOSE {
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
;

coord_tag_mach :
machid_attr  OPENTAG_RIGHT COORD_CLOSE {
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
;

/*** severity tag ***/

severity_tag :
SEVERITY_OPEN {if (parseContext.dynamicMetricLoading) return 0;} matrix_tags SEVERITY_CLOSE
;

matrix_tags :
/* empty */
| matrix_tags matrix_tag
;


matrix_tag :
MATRIX_OPEN metricid_attr OPENTAG_RIGHT {
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
} row_tags_opt MATRIX_CLOSE
;

row_tags_opt:
|row_tags
;

row_tags :
row_tag
| row_tags row_tag
;

row_tag :
ROW_OPEN cnodeid_attr OPENTAG_RIGHT {
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
} ROW_CLOSE {
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
;



%% /*** Additional Code ***/


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




