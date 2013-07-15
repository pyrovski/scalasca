/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_GENERATOR_PATTERN_H
#define SCOUT_GENERATOR_PATTERN_H


#include <cstdio>
#include <map>
#include <string>


/*--- Type definitions ----------------------------------------------------*/

enum missing_t {
  NOTHING,
  DEF_NAME,
  DEF_CLASS,
  DEF_INFO,
  DEF_UNIT
};


/*
 *---------------------------------------------------------------------------
 *
 * class Pattern
 *
 *---------------------------------------------------------------------------
 */

class Pattern
{
  public:
    /* Constructors & destructor */
    Pattern(const std::string& identifier)
      : m_id(identifier), m_parent("NONE"), m_hidden(false), m_nodocs(false),
        m_type("Generic")
    {}

    /* Setting pattern attributes */
    void set_name(const std::string& name)      { m_name    = name;  }
    void set_classname(const std::string& name) { m_class   = name;  }
    void set_docname(const std::string& name)   { m_docname = name;  }
    void set_parent(const std::string& id)      { m_parent  = id;    }
    void set_info(const std::string& info)      { m_info    = info;  }
    void set_descr(const std::string& descr)    { m_descr   = descr; }
    void set_diagnosis(const std::string& diag) { m_diag    = diag;  }
    void set_unit(const std::string& unit)      { m_unit    = unit;  }
    void set_condition(const std::string& cond) { m_cond    = cond;  }
    void set_init(const std::string& code)      { m_init    = code;  }
    void set_cleanup(const std::string& code)   { m_cleanup = code;  }
    void set_data(const std::string& code)      { m_data    = code;  }
    void set_hidden()                           { m_hidden  = true;  }
    void set_nodocs()                           { m_nodocs  = true;  }
    void set_type(const std::string& type)      { m_type    = type;  }

    /* Retrieving pattern attributes */
    std::string get_id() const        { return m_id;      }
    std::string get_name() const      { return m_name;    }
    std::string get_classname() const { return m_class;   }
    std::string get_docname() const {
      if (m_docname.empty())
        return m_name;
      return m_docname;
    }
    std::string get_parent() const    { return m_parent;  }
    std::string get_info() const      { return m_info;    }
    std::string get_descr() const     { return m_descr;   }
    std::string get_diagnosis() const { return m_diag;    }
    std::string get_unit() const      { return m_unit;    }
    std::string get_condition() const { return m_cond;    }
    std::string get_init() const      { return m_init;    }
    std::string get_cleanup() const   { return m_cleanup; }
    std::string get_data() const      { return m_data;    }
    bool        get_hidden() const    { return m_hidden;  }
    bool        get_nodocs() const    { return m_nodocs;  }
    std::string get_type() const      { return m_type;    }

    bool skip_impl() const { return m_hidden && m_callbacks.empty(); }

    /* Adding callbacks */
    bool add_callback(const std::string& event, const std::string& code);

    /* Checking validity */
    missing_t is_valid() const;

    /* Writing output */
    void write_impl(std::FILE* fp) const;
    void write_html(std::FILE* fp, bool isFirst);


  private:
    /* Data */
    std::string m_id;        // symbolic pattern name (id)
    std::string m_name;      // display name
    std::string m_class;     // class name
    std::string m_docname;   // class name for documantation
    std::string m_parent;    // symbolic name (id) of parent
    std::string m_info;      // brief pattern description
    std::string m_descr;     // detailed pattern description (HTML)
    std::string m_diag;      // diagnosis description (HTML)
    std::string m_unit;      // unit of measurement
    std::string m_cond;      // conditional expression controlling pattern creation
    std::string m_init;      // code used for initialization of data
    std::string m_cleanup;   // code used for cleaning up data
    std::string m_data;      // code for local data fields
    bool        m_hidden;    // TRUE, if pattern is not visible in unmapped CUBE file
    bool        m_nodocs;    // TRUE, if no documentation should be generated
    std::string m_type;      // Pattern type (MPI, Generic)

    std::map<std::string,std::string> m_callbacks;   // event |-> callback code

    void process_html(std::string& text);
};


#endif   /* !SCOUT_GENERATOR_PATTERN_H */
