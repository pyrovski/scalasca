/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include "Filter.h"

#include <fstream>
#include <set>
#include <string>

#include <fnmatch.h>

#include "Region.h"

using namespace cube;
using namespace std;

void Filter::add_file(string file_url)
{
	ifstream ifs(file_url.c_str(), ios_base::in);
	string line;
	while (1)
	{
		ifs >> line;
		if (ifs.eof())
			break;
		filter_set.insert(line);
	}
}

bool Filter::matches(const Cnode* node) const
{
	return matches(node->get_callee()->get_name());
}

bool Filter::matches(string node_name) const
{
	// TODO: For efficiency, we may detect whether a line is really a pattern or just
	//   a string. Using this detection, we may apply a filter to a single node in
	//     O(log(number of non-wildcard-filters) + (number of wildcard-filters))
  //   instead of
	//     O(number of filters).
	for (set<string>::const_iterator it = filter_set.begin();
			it != filter_set.end(); ++it)
	{
		if (fnmatch(it->c_str(), node_name.c_str(), 0) != FNM_NOMATCH)
			return true;
	}
	return false;
}

