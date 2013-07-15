/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <algorithm>
#include <cctype>
#include <functional>
#include <vector>

#include "Pattern.h"

using namespace std;


//--- Global variables ------------------------------------------------------

extern map<string,Pattern*> id2pattern;
extern vector<Pattern*>     pattern;


//--- Function prototypes ---------------------------------------------------

extern void yyerror(const string& message);


//--- Utility functions -----------------------------------------------------

namespace
{

struct fo_tolower : public std::unary_function<int,int> {
  int operator()(int x) const {
    return std::tolower(x);
  }
};

string lowercase(const string& str)
{
  string result(str);

  transform(str.begin(), str.end(), result.begin(), fo_tolower());

  return result;
}

}   /* unnamed namespace */


/*
 *---------------------------------------------------------------------------
 *
 * class Pattern
 *
 *---------------------------------------------------------------------------
 */

/*--- Adding callbacks ----------------------------------------------------*/

bool Pattern::add_callback(const string& event, const string& code)
{
  map<string,string>::iterator it = m_callbacks.lower_bound(event);
  if (it != m_callbacks.end() && !(m_callbacks.key_comp()(event, it->first)))
    return false;

  m_callbacks.insert(it, make_pair(event, code));
  return true;
}


/*--- Checking validity ---------------------------------------------------*/

missing_t Pattern::is_valid() const
{
  if (m_name.empty())
    return DEF_NAME;
  if (m_class.empty())
    return DEF_CLASS;
  if (m_info.empty())
    return DEF_INFO;
  if (m_unit.empty())
    return DEF_UNIT;

  return NOTHING;
}


/*--- Writing output ------------------------------------------------------*/

void Pattern::write_impl(FILE* fp) const
{
  if (skip_impl())
    return;

  /***** Header *****/

  if (m_type == "MPI")
    fprintf(fp, "#if defined(_MPI)\n\n");
  else if (m_type == "MPI_RMA")
    fprintf(fp, "#if defined(_MPI) && defined(HAS_MPI2_1SIDED)\n\n");
  else if (m_type == "OMP")
    fprintf(fp, "#if defined(_OPENMP)\n\n");

  fprintf(fp, "/*\n"
              " *---------------------------------------------------------------------------\n"
              " *\n"
              " * class %s\n"
              " *\n"
              " *---------------------------------------------------------------------------\n"
              " */\n"
              "\n\n", m_class.c_str());
  if (m_type == "Generic")
    fprintf(fp, "class %s : public Pattern\n", m_class.c_str());
  else if (m_type == "MPI" || m_type == "MPI_RMA")
    fprintf(fp, "class %s : public MpiPattern\n", m_class.c_str());
  else if (m_type == "OMP")
    fprintf(fp, "class %s : public OmpPattern\n", m_class.c_str());
  fprintf(fp, "{\n"
              "  public:\n");

  /***** Destructor *****/

  if (!m_cleanup.empty())
    fprintf(fp, "\n    /* Constructors & destructor */\n"
                "    virtual ~%s()\n"
                "    {\n"
                "%s\n"
                "    }\n",
                m_class.c_str(),
                m_cleanup.c_str());

  /***** reg_cb() *****/

  if (!m_callbacks.empty()) {
    fprintf(fp, "    /* Registering callbacks */\n"
                "    virtual void reg_cb(CallbackManager* cbmanager)\n"
                "    {\n");
    fprintf(fp, "      init();\n\n");
    map<string,string>::const_iterator it = m_callbacks.begin();
    while (it != m_callbacks.end()) {
      fprintf(fp, "      cbmanager->register_callback(%s, PEARL_create_callback(this, &%s::%s_cb));\n",
                  it->first.c_str(),
                  m_class.c_str(),
                  lowercase(it->first).c_str());

      ++it;
    }
    fprintf(fp, "    }\n\n");
  }

  /***** get_id() *****/

  fprintf(fp, "    /* Get pattern information */\n"
              "    virtual long get_id() const\n"
              "    {\n"
              "      return PAT_%s;\n"
              "    }\n\n", m_id.c_str());

  /***** get_parent() *****/

  fprintf(fp, "    virtual long get_parent() const\n"
              "    {\n"
              "      return PAT_%s;\n"
              "    }\n\n", m_parent.c_str());

  /***** get_name() *****/

  fprintf(fp, "    virtual string get_name() const\n"
              "    {\n"
              "      return \"%s\";\n"
              "    }\n\n", m_name.c_str());

  /***** get_unique_name() *****/

  fprintf(fp, "    virtual string get_unique_name() const\n"
              "    {\n"
              "      return \"%s\";\n"
              "    }\n\n", lowercase(m_id).c_str());

  /***** get_descr() *****/

  fprintf(fp, "    virtual string get_descr() const\n"
              "    {\n"
              "      return \"%s\";\n"
              "    }\n\n", m_info.c_str());

  /***** get_unit() *****/

  fprintf(fp, "    virtual string get_unit() const\n"
              "    {\n"
              "      return \"%s\";\n"
              "    }\n\n", m_unit.c_str());

  /***** is_hidden() *****/

  if (m_hidden)
    fprintf(fp, "    virtual bool is_hidden() const\n"
                "    {\n"
                "      return true;\n"
                "    }\n\n");

  /***** Callback methods *****/

  if (!m_callbacks.empty()) {
    fprintf(fp, "    /* Callback methods */\n");
    map<string,string>::const_iterator it = m_callbacks.begin();
    while (it != m_callbacks.end()) {
      fprintf(fp, "    void %s_cb(const pearl::CallbackManager& cbmanager, int user_event, "
                  "const pearl::Event& event, CallbackData* cdata);\n",
                  lowercase(it->first).c_str());
      ++it;
    }
  }

  /***** protected: *****/

  if (!m_init.empty() || ! m_cleanup.empty())
    fprintf(fp, "  protected:\n"
                "    /* Protected methods */\n");

  /***** init() *****/

  if (!m_init.empty())
    fprintf(fp, "\n    virtual void init()\n"
                "    {\n"
                "%s\n"
                "    }\n", m_init.c_str());

  /***** Data *****/

  if (!m_data.empty())
    fprintf(fp, "\n\n  private:\n"
                "%s\n", m_data.c_str());

  /***** Footer *****/

  fprintf(fp, "};\n\n\n");

  /***** Callback method implementations *****/

  if (!m_callbacks.empty()) {
    fprintf(fp, "/*----- Callback methods -----*/\n\n");
    map<string,string>::const_iterator it = m_callbacks.begin();
    while (it != m_callbacks.end()) {
      fprintf(fp, "void %s::%s_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)\n"
                  "{\n",
		  m_class.c_str(),
                  lowercase(it->first).c_str());
      // Heuristic to check whether the data object is used
      // Not bullet proof, but should work reasonably well...
      if (it->second.find("data") != string::npos)
        fprintf(fp, "  CbData* data = static_cast<CbData*>(cdata);\n");
      fprintf(fp, "%s\n"
                  "}\n\n",
                  it->second.c_str());

      ++it;
    }
  }
 
  if (m_type == "MPI")
    fprintf(fp, "#endif   /* _MPI */\n\n");
  else if (m_type == "MPI_RMA")
    fprintf(fp, "#endif   /* _MPI && HAS_MPI2_1SIDED */\n\n");
  else if (m_type == "OMP")
    fprintf(fp, "#endif   /* _OPENMP */\n\n");
  fprintf(fp, "\n");
}


void Pattern::write_html(FILE* fp, bool isFirst)
{
  if (m_nodocs)
    return;

  process_html(m_descr);
  process_html(m_diag);

  if (!isFirst)
    fprintf(fp, "<hr width=\"75%%\" align=\"center\">\n\n");
  fprintf(fp, "<a name=\"%s\"><h3>%s</h3></a>\n",
              lowercase(m_id).c_str(), get_docname().c_str());
  fprintf(fp, "<dl>\n");

  /* Description */
  fprintf(fp, "<dt><b>Description:</b></dt>\n");
  if (m_descr.empty()) {
    if (m_info.empty())
      fprintf(fp, "<dd>No description available.</dd>\n");
    else
      fprintf(fp, "<dd>%s</dd>\n", m_info.c_str());
  } else {
    fprintf(fp, "<dd>%s</dd>\n", m_descr.c_str());
  }

  /* Unit */
  fprintf(fp, "<dt><b>Unit:</b></dt>\n");
  if (m_unit == "sec")
    fprintf(fp, "<dd>Seconds</dd>\n");
  else if (m_unit == "occ")
    fprintf(fp, "<dd>Counts</dd>\n");
  else if (m_unit == "bytes")
    fprintf(fp, "<dd>Bytes</dd>\n");
  else {
    fprintf(stderr, "Unknown unit of measurement!");
    exit(1);
  }

  /* Diagnosis */
  if (!m_diag.empty()) {
    fprintf(fp, "<dt><b>Diagnosis:</b></dt>\n");
    fprintf(fp, "<dd>%s</dd>\n", m_diag.c_str());
  }

  /* Parent */
  string parent_name;
  map<string,Pattern*>::const_iterator it = id2pattern.find(m_parent);
  if (it == id2pattern.end())
    parent_name = "None";
  else
    parent_name = "<a href=\"#" + lowercase(it->second->get_id()) + "\">" +
                  it->second->get_docname() + "</a>";
  fprintf(fp, "<dt><b>Parent:</b></dt>\n"
              "<dd>%s</dd>\n", parent_name.c_str());

  /* Children */
  fprintf(fp, "<dt><b>Children:</b></dt>\n<dd>");
  bool hasChildren = false;
  vector<Pattern*>::iterator pat = pattern.begin();
  while (pat != pattern.end()) {
    if ((*pat)->get_parent() == m_id) {
      if (hasChildren == true)
        fprintf(fp, ", ");
      fprintf(fp, "<a href=\"#%s\">%s</a>",
                  lowercase((*pat)->get_id()).c_str(),
                  (*pat)->get_docname().c_str());
      hasChildren = true;
    }
    ++pat;
  }
  if (!hasChildren)
    fprintf(fp, "None");
  fprintf(fp, "</dd>\n");

  fprintf(fp, "</dl>\n\n");
}


/*--- Private methods -----------------------------------------------------*/

void Pattern::process_html(string& text)
{
  /* Remove trailing spaces */
  string::reverse_iterator it = text.rbegin();
  while (it != text.rend() && *it == ' ')
    ++it;
  text.erase(it.base(), text.end());

  /* Keyword substitution: Cross-references */
  string::size_type spos = text.find("@ref(");
  while (spos != string::npos) {
    /* Search for closing brace */
    string::size_type epos = text.find(")", spos);
    if (epos == string::npos)
      yyerror("Description error: \")\" missing.");

    /* Extract & validate ID */
    string   id(text, spos + 5, epos - (spos + 5));
    map<string,Pattern*>::iterator pat = id2pattern.find(id);
    if (pat == id2pattern.end())
      yyerror("Description error: Unknown pattern name.");

    /* Insert anchor */
    text.replace(spos, epos - spos + 1,
                 "<a href=\"#" + lowercase(pat->second->get_id()) + "\">" +
                 pat->second->get_docname() + "</a>");

    /* Search for new reference */
    spos = text.find("@ref(");
  }

  /* Keyword substitution: Images */
  spos = text.find("@img(");
  while (spos != string::npos) {
    /* Search for closing brace */
    string::size_type epos = text.find(")", spos);
    if (epos == string::npos)
      yyerror("Description error: \")\" missing.");

    /* Extract image name */
    string id(text, spos + 5, epos - (spos + 5));

    /* Insert image reference */
    text.replace(spos, epos - spos + 1,
                 "<br>\n"
                 "<div align=\"center\">\n"
                 "<img src=\"" + id + "\" alt=\"" + m_name + " Example\">\n"
                 "</div>\n"
                 "<br>\n");

    /* Search for new reference */
    spos = text.find("@img(");
  }
}
