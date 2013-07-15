%code requires{ /*** C/C++ Declarations ***/

#include <cmath>
#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "ParseContext.h"

}

/*** yacc/bison Declarations ***/

/* Require bison 2.3 or later */
%require "2.3"

/* add debug output code to generated parser. */
%debug

/* start symbol is named "start" */
%start document

/* write out a header file containing the token defines */
%defines

/* use newer C++ skeleton file */
%skeleton "lalr1.cc"

/* namespace to enclose parser in */
%name-prefix="cubeparser"

/* set the parser's class identifier */
%define "parser_class_name" "Cube2Parser"

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
%parse-param { class Cube2Scanner& cube2Lexer }
%parse-param { class cube::Cube& cube }

/* verbose error messages */
%error-verbose

 /*** the grammar's tokens ***/

 
%union  
{ 
} 

%token FFALSE               "false"
%token TTRUE                "true"

%token CLOSETAG_RIGHT      "/>" 
%token XML_CLOSE           "?>"
%token OPENTAG_RIGHT       ">" 

%token XML_OPEN            "<?xml"

%token ATTR_OPEN           "<attr"
%token DIM_OPEN            "<dim"

%token CUBE_OPEN2          "<cube version=\"2.0\""
%token CUBE_CLOSE          "</cube>"
%token METRIC_OPEN         "<metric"
%token METRIC_CLOSE        "</metric>"
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
%token CART_OPEN           "<cart"
%token CART_CLOSE          "</cart>"
%token COORD_OPEN          "<coord"
%token COORD_CLOSE         "</coord>"
%token MATRIX_OPEN         "<matrix"
%token MATRIX_CLOSE        "</matrix>"
%token ROW_OPEN            "<row"
%token ROW_CLOSE           "</row>"
%token MODULE_OPEN         "<module"
%token MODULE_CLOSE        "</module>"
%token REGION_OPEN         "<region"
%token REGION_CLOSE        "</region>"
%token CSITE_OPEN          "<csite"
%token CSITE_CLOSE         "</csite>"

%token BEGIN_OPEN          "<begin>"
%token BEGIN_CLOSE         "</begin>"
%token END_OPEN            "<end>"
%token END_CLOSE           "</end>"
%token MOD_OPEN            "<mod>"
%token MOD_CLOSE           "</mod>"
%token LINE_OPEN           "<line>"
%token LINE_CLOSE          "</line>"
%token CALLEE_OPEN         "<callee>"
%token CALLEE_CLOSE        "</callee>"
%token DOC_OPEN            "<doc>"
%token DOC_CLOSE           "</doc>"
%token MIRRORS_OPEN        "<mirrors>"
%token MIRRORS_CLOSE       "</mirrors>"
%token MURL_OPEN           "<murl>"
%token MURL_CLOSE          "</murl>"
%token BEHAVIOR_OPEN       "<behavior>"
%token BEHAVIOR_CLOSE      "</behavior>"
%token NAME_OPEN           "<name>"
%token NAME_CLOSE          "</name>"
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
%token LOCATIONS_OPEN      "<locations>"
%token LOCATIONS_CLOSE     "</locations>"
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
%token ANAME_CSITEID       "attribute name csiteId"
%token ANAME_LOCID         "attribute name locid"
%token ANAME_NDIMS         "attribute name ndims"
%token ANAME_SIZE          "attribute name size"
%token ANAME_PERIODIC      "attribute name periodic"
%token ANAME_METRICID      "attribute name metricId"
%token ANAME_CNODEID       "attribute name cnodeId"

%token ATTRIBUTE_VALUE     "attribute value"
%token END 0               "end of file"
%token ERROR               "unknown element"

%{

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

%}

%% /*** Grammar Rules ***/

/*** document structure ***/

document :
xml_tag cube_tag 
;

/*** xml tag ***/

xml_tag :
XML_OPEN version_attr encoding_attr XML_CLOSE {
	bool valid = (strcmp(parseContext.version.c_str(),"1.0")==0);
	if (!valid) error(yylloc,"XML version is expected to be 1.0!");
	valid = (strcmp(parseContext.encoding.c_str(),"UTF-8")==0);
	if (!valid) error(yylloc,"XML encoding is expected to be UTF-8!");
}
;

/*** cube tag ***/

cube_tag :
cube_begin cube_content CUBE_CLOSE 
;

cube_begin :
CUBE_OPEN2 {
	parseContext.cubeVersion = 2;
} 
;

cube_content :
attr_tags doc_tag behavior_tag program_tag locations_tag severity_tag
| doc_tag behavior_tag program_tag locations_tag severity_tag
| attr_tags behavior_tag program_tag locations_tag severity_tag
| behavior_tag program_tag locations_tag severity_tag
;

/*** attribute tags ***/

attr_tags :
attr_tag
| attr_tags attr_tag
;

attr_tag :
ATTR_OPEN key_attr value_attr CLOSETAG_RIGHT {
	cube.def_attr(parseContext.key,parseContext.value);
}
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
murl_tag {cube.def_mirror(parseContext.murl);}
| murl_tags murl_tag {cube.def_mirror(parseContext.murl);}
;


/*** behavior tag ***/

behavior_tag :
BEHAVIOR_OPEN metric_tag BEHAVIOR_CLOSE 
;

metric_tag :
metric_begin
| metric_tag metric_begin
| metric_tag metric_end
;


metric_begin :
METRIC_OPEN id_attr OPENTAG_RIGHT name_tag dtype_tag uom_tag val_tag url_tag descr_tag {
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
;

metric_end :
METRIC_CLOSE {
	if (parseContext.currentMetric==NULL)
	  error(yylloc,"Metric definitions are not correctly nested!"); 
	parseContext.currentMetric = (parseContext.currentMetric)->get_parent();
}
;


/*** program tag ***/

program_tag :
PROGRAM_OPEN module_tags_opt region_tags csite_tags cnode_tag PROGRAM_CLOSE
;

module_tags_opt :
| module_tags 
;

module_tags :
module_tag 
|module_tags module_tag
;

module_tag :
MODULE_OPEN id_attr OPENTAG_RIGHT name_tag MODULE_CLOSE {
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
;

region_tags :
region_tag
| region_tags region_tag
;

region_tag :
REGION_OPEN id_attr OPENTAG_RIGHT name_tag mod_tag begin_tag end_tag url_tag descr_tag REGION_CLOSE {
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
;

csite_tags :
csite_tag
|csite_tags csite_tag
;

csite_tag :
CSITE_OPEN id_attr OPENTAG_RIGHT line_tag mod_tag callee_tag CSITE_CLOSE {
	int startPos = 3 * parseContext.id;
	while ((int)parseContext.csiteVec.size()<startPos+3) 
		parseContext.csiteVec.push_back(0);
	parseContext.csiteVec[startPos] = parseContext.line;
	parseContext.csiteVec[startPos+1] = parseContext.moduleid;
	parseContext.csiteVec[startPos+2] = parseContext.calleenr;
}
;

cnode_tag :
cnode_begin
| cnode_tag cnode_begin
| cnode_tag CNODE_CLOSE {
	if (parseContext.currentCnode==NULL)
	  error(yylloc,"Cnode definitions are not correctly nested!"); 
	parseContext.currentCnode = parseContext.currentCnode->get_parent();
}
;

cnode_begin :
CNODE_OPEN id_attr csiteid_attr OPENTAG_RIGHT {
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
;



/*** locations tag ***/

locations_tag :
LOCATIONS_OPEN machine_tags topologies_tag_opt LOCATIONS_CLOSE
;

machine_tags :
machine_tag
| machine_tags machine_tag
;

machine_tag :
machine_begin node_tags MACHINE_CLOSE
;

machine_begin :
MACHINE_OPEN locid_attr OPENTAG_RIGHT name_tag {
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
;

node_tags :
node_tag 
| node_tags node_tag
;

node_tag :
node_begin process_tags NODE_CLOSE
;

node_begin :
NODE_OPEN locid_attr OPENTAG_RIGHT name_tag {
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
;

process_tags :
process_tag
| process_tags process_tag
;

process_tag :
process_begin thread_tags PROCESS_CLOSE
;

process_begin :
PROCESS_OPEN locid_attr OPENTAG_RIGHT name_tag rank_tag {
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
;

thread_tags :
thread_tag 
| thread_tags thread_tag
;

thread_tag :
THREAD_OPEN locid_attr OPENTAG_RIGHT name_tag rank_tag THREAD_CLOSE {
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
;

topologies_tag_opt :
| topologies_tag
;

topologies_tag :
TOPOLOGIES_OPEN cart_tags_opt TOPOLOGIES_CLOSE
;

cart_tags_opt :
| cart_tags
;

cart_tags :
cart_tag
| cart_tags cart_tag
;

cart_tag :
cart_begin ndims_attr OPENTAG_RIGHT dim_tags COORD_OPEN {
	if (parseContext.ndims!=(int)parseContext.dimVec.size())
	  error(yylloc,"Too few or too many topology dimensions are declared!");
	parseContext.currentCart = 
		cube.def_cart(parseContext.ndims,parseContext.dimVec,parseContext.periodicVec);
} coord_tags CART_CLOSE 
;

cart_begin :
CART_OPEN {
	parseContext.dimVec.clear();
	parseContext.periodicVec.clear();
} 
;

dim_tags :
dim_tag
| dim_tags dim_tag 
;

dim_tag :
DIM_OPEN size_attr periodic_attr CLOSETAG_RIGHT 
;

coord_tags :
coord_tag
| coord_tags COORD_OPEN coord_tag
;

coord_tag :
locid_attr  OPENTAG_RIGHT COORD_CLOSE {
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
;

/*** severity tag ***/

severity_tag :
SEVERITY_OPEN matrix_tags SEVERITY_CLOSE
;

matrix_tags :
matrix_tag
| matrix_tags matrix_tag
;


matrix_tag :
MATRIX_OPEN metricid_attr OPENTAG_RIGHT {
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
	if (!parseContext.ignoreMetric){
	if ((int)parseContext.cnodeVec.size()<=parseContext.cnodeid)
	  error(yylloc,"Cnode of the severity row wasn't declared!"); 
	if (parseContext.cnodeVec[parseContext.cnodeid]==NULL)
	  error(yylloc,"Cnode of the severity row wasn't declared!");
	parseContext.currentCnode = parseContext.cnodeVec[parseContext.cnodeid];
	}
} ROW_CLOSE {
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
;


/* string attributes */

version_attr :
ANAME_VERSION ATTRIBUTE_VALUE { parseContext.version = parseContext.str.str(); };
encoding_attr :
ANAME_ENCODING ATTRIBUTE_VALUE { parseContext.encoding = parseContext.str.str(); };
key_attr :
ANAME_KEY ATTRIBUTE_VALUE { parseContext.key = parseContext.str.str(); };
value_attr :
ANAME_VALUE ATTRIBUTE_VALUE { parseContext.value = parseContext.str.str(); };

/* int attributes */

id_attr :    
ANAME_ID ATTRIBUTE_VALUE { 
	if (parseContext.str.str().empty())
	  error(yylloc,"Empty id!"); 
	int id = atoi(parseContext.str.str().c_str());
	if (id<0) error(yylloc,"Ids must be non-negative!");
	parseContext.id = id; 
};
csiteid_attr :    
ANAME_CSITEID ATTRIBUTE_VALUE { 
	if (parseContext.str.str().empty())
	  error(yylloc,"Empty id!");
	int id = atoi(parseContext.str.str().c_str());
	if (id<0) error(yylloc,"Ids must be non-negative!");
	parseContext.csiteid = id; 
};
locid_attr :    
ANAME_LOCID ATTRIBUTE_VALUE { 
	if (parseContext.str.str().empty())
	  error(yylloc,"Empty id!");
	int id = atoi(parseContext.str.str().c_str());
	if (id<0) error(yylloc,"Thread ids must be non-negative!");
	parseContext.locid = id; 
};
metricid_attr :       
ANAME_METRICID ATTRIBUTE_VALUE { 
	if (parseContext.str.str().empty())
	  error(yylloc,"Empty id!");
	int id = atoi(parseContext.str.str().c_str());
	parseContext.metricid = id; 
};
cnodeid_attr :    
ANAME_CNODEID ATTRIBUTE_VALUE { 
	if (parseContext.str.str().empty())
	  error(yylloc,"Empty id!");
	int id = atoi(parseContext.str.str().c_str());
	if (id<0) error(yylloc,"Cnode ids must be non-negative!");
 	parseContext.cnodeid= id; 
};

/* long attributes */

ndims_attr :    
ANAME_NDIMS ATTRIBUTE_VALUE { 
	if (parseContext.str.str().empty())
	  error(yylloc,"Empty ndims!");
	long ndims = atol(parseContext.str.str().c_str());
	if (ndims<=0)
	   error(yylloc,"Topology dimensions must be positive numbers!");
	parseContext.ndims = ndims; 
};

/* void attributes */

size_attr :       
ANAME_SIZE ATTRIBUTE_VALUE { 
	if (parseContext.str.str().empty())
	  error(yylloc,"Empty size!");
	long size = atol(parseContext.str.str().c_str());
	if (size<=0)
	  error(yylloc,"Dimension sizes must be positive numbers!");
	parseContext.dimVec.push_back(size);
};

/* boolean attributes */

periodic_attr :        
ANAME_PERIODIC FFALSE { 
	parseContext.periodicVec.push_back(false);
}
| ANAME_PERIODIC TTRUE { 
	parseContext.periodicVec.push_back(true);
}
 
;





/* basic tags long */

begin_tag :         
BEGIN_OPEN BEGIN_CLOSE {
	parseContext.beginln = atol(parseContext.str.str().c_str());
}
| {parseContext.beginln = -1;}
;
end_tag :       
END_OPEN END_CLOSE { 
	parseContext.endln = atol(parseContext.str.str().c_str());
}
| {parseContext.endln = -1;}
;
line_tag :       
LINE_OPEN LINE_CLOSE {
	parseContext.line = atol(parseContext.str.str().c_str());
}
| {parseContext.line = -1;}
;

/* basic tags int */

mod_tag :
MOD_OPEN MOD_CLOSE {
	  int moduleId = atoi(parseContext.str.str().c_str());
	  if (moduleId<0)
	    error(yylloc,"Module ids must be nonnegative integers!");
	  if ((int)parseContext.moduleVec.size()<=moduleId) 
	    error(yylloc,"Reference to an undefined module!");
	parseContext.moduleid = moduleId;
}
| {parseContext.moduleid = -1;}
;

callee_tag :
CALLEE_OPEN CALLEE_CLOSE {
	int calleeNr = atoi(parseContext.str.str().c_str());
	if (calleeNr<0)
	  error(yylloc,"Callee ids must be nonnegative integers!");
	parseContext.calleenr = calleeNr; 
}
| {parseContext.calleenr = -1;}
;
rank_tag :
RANK_OPEN RANK_CLOSE {
	int rank = atoi(parseContext.str.str().c_str());
	if (rank<0) 
	  error(yylloc,"Ranks must be non-negative!");
	parseContext.rank = rank; 
};

/* basic tags string */

dtype_tag :
DTYPE_OPEN DTYPE_CLOSE {parseContext.dtype = parseContext.str.str();}
| {parseContext.dtype = "";}
;
murl_tag :
MURL_OPEN MURL_CLOSE {parseContext.murl = parseContext.str.str(); }
;
name_tag :
NAME_OPEN NAME_CLOSE {parseContext.name = parseContext.str.str(); }
;
uom_tag :
UOM_OPEN UOM_CLOSE {parseContext.uom = parseContext.str.str(); }
| {parseContext.uom = "";}
;
val_tag :
VAL_OPEN VAL_CLOSE {parseContext.val = parseContext.str.str(); }
| {parseContext.val = "";}
;
url_tag :
URL_OPEN URL_CLOSE {parseContext.url = parseContext.str.str(); }
| {parseContext.url = "";}
;
descr_tag :
DESCR_OPEN DESCR_CLOSE {parseContext.descr = parseContext.str.str(); }
| {parseContext.descr = "";}
;

%% /*** Additional Code ***/

void cubeparser::Cube2Parser::error(const Cube2Parser::location_type& l,
			            const std::string& m)
{
    driver.error(l, m);
}




