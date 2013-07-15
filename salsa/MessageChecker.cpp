/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include "MessageChecker.h"

#include "MpiComm.h"

bool MessageChecker::is_applicable(const pearl::RemoteEvent& event) {
	bool valid_size = false;
	bool valid_tag = false;
	bool valid_comm = false;

	if(!size.empty()) {
		for(std::list<Constraint>::iterator it = size.begin(); it != size.end(); ++it) {
			switch(it->get_op()) {
				case 1:
					if(event->get_sent() == it->get_value()) valid_size = true;
					break;
				case 2:
					if(event->get_sent() != it->get_value()) 	valid_size = true;
					break;
				case 3:
					if(event->get_sent() >= it->get_value()) valid_size = true;
					break;
				case 4:
					if(event->get_sent() > it->get_value()) valid_size = true;
					break;
				case 5:
					if(event->get_sent() <= it->get_value()) valid_size = true;
					break;
				case 6:
					if(event->get_sent() < it->get_value()) valid_size = true;
					break;
				case 7:
					unsigned int s = event->get_sent();
					if(s >= it->get_value() && s <= it->get_second_value()) valid_size = true;
					break;
			}
			if(valid_size) break;
		}
	} else {
		valid_size = true;
	}
	if(!tag.empty()) {
		for(std::list<Constraint>::iterator it = tag.begin(); it != tag.end(); ++it) {
			switch(it->get_op()) {
				case 1:
					if(event->get_tag() == it->get_value()) valid_tag = true;
					break;
				case 2:
					if(event->get_tag() != it->get_value()) 	valid_tag = true;
					break;
				case 3:
					if(event->get_tag() >= it->get_value()) valid_tag = true;
					break;
				case 4:
					if(event->get_tag() > it->get_value()) valid_tag = true;
					break;
				case 5:
					if(event->get_tag() <= it->get_value()) valid_tag = true;
					break;
				case 6:
					if(event->get_tag() < it->get_value()) valid_tag = true;
					break;
				case 7:
					unsigned int t = event->get_tag();
					if(t >= it->get_value() && t <= it->get_second_value()) valid_tag = true;
					break;
			}
			if(valid_tag) break;
		}
	} else {
		valid_tag = true;
	}
	if(!comm.empty()) {
		for(std::list<Constraint>::iterator it = comm.begin(); it != comm.end(); ++it) {
			switch(it->get_op()) {
				case 1:
					if((*event->get_comm()).get_id() == it->get_value()) valid_comm = true;
					break;
				case 2:
					if((*event->get_comm()).get_id() != it->get_value()) 	valid_comm = true;
					break;
				case 3:
					if((*event->get_comm()).get_id() >= it->get_value()) valid_comm = true;
					break;
				case 4:
					if((*event->get_comm()).get_id() > it->get_value()) valid_comm = true;
					break;
				case 5:
					if((*event->get_comm()).get_id() <= it->get_value()) valid_comm = true;
					break;
				case 6:
					if((*event->get_comm()).get_id() < it->get_value()) valid_comm = true;
					break;
				case 7:
					unsigned int c = (*event->get_comm()).get_id();
					if(c >= it->get_value() && c <= it->get_second_value()) valid_comm = true;
					break;
			}
			if(valid_comm) break;
		}
	} else {
		valid_comm = true;
	}
	return (valid_size && valid_tag && valid_comm);
}

unsigned int MessageChecker::op2int(std::string op) {
	if(op == "==") return 1;
	else if(op == "!=") return 2;
	else if(op == ">=") return 3;
	else if(op == ">") return 4;
	else if(op == "<=") return 5;
	else if(op == "<") return 6;
	else if(op == "-") return 7;
	return 0;
}

void MessageChecker::clear() {
	size.clear();
	tag.clear();
	comm.clear();
}

