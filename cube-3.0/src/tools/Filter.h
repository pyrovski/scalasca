/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef _FILTER_H_GUARD_
#define _FILTER_H_GUARD_

#include <string>
#include <set>

#include "Cnode.h"

namespace cube
{
	class Filter
	{
		public:
			void add_file(std::string file_url);
			bool matches(const Cnode* node) const;
			bool matches(std::string uniq_node_name) const;
      inline bool empty() { return filter_set.empty(); }
	
		private:
			std::set<std::string> filter_set;
	};
}

#endif

