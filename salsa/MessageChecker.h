/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef MESSAGE_CHECKER_H
#define MESSAGE_CHECKER_H

#include <list>
#include <string>
#include "RemoteEvent.h"

class Constraint {
public:
	Constraint(unsigned int op, unsigned int value, unsigned int value2 = 0) : op(op), value(value), value2(value2) {}
	unsigned int get_op() { return op; }
	unsigned int get_value() { return value; }
	unsigned int get_second_value() { return value2; }
private:
	unsigned int op;
	unsigned int value;
	unsigned int value2;
};

class MessageChecker {
public:
	MessageChecker(const std::list<Constraint>& size, const std::list<Constraint>& tag,
			const std::list<Constraint>& comm) : size(size), tag(tag), comm(comm) {}
	MessageChecker() {}
	bool is_applicable(const pearl::RemoteEvent& event);
	void add_size_constraint(std::string op, unsigned int value) {size.push_back(Constraint(op2int(op), value));}
	void add_size_constraint(std::string op, unsigned int value, unsigned int value2) {size.push_back(Constraint(op2int(op), value, value2));}
	void add_tag_constraint(std::string op, unsigned int value) {tag.push_back(Constraint(op2int(op), value));}
	void add_tag_constraint(std::string op, unsigned int value, unsigned int value2) {tag.push_back(Constraint(op2int(op), value, value2));}
	void add_comm_constraint(std::string op, unsigned int value) {comm.push_back(Constraint(op2int(op), value));}
	void add_comm_constraint(std::string op, unsigned int value, unsigned int value2) {comm.push_back(Constraint(op2int(op), value, value2));}
	void clear();
private:
	unsigned int op2int(std::string op);

	std::list<Constraint> size;
	std::list<Constraint> tag;
	std::list<Constraint> comm;
};

#endif /* MESSAGE_CHECKER_H */
