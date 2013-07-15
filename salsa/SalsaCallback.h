/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef SALSACALLBACK_H_
#define SALSACALLBACK_H_

#include "salsa.h"
#include "MessageChecker.h"
#include "CallbackData.h"
#include "CallbackManager.h"
#include "MpiMessage.h"


class SalsaCallback {
public:
	SalsaCallback(int r, int s, funcs f, modes m, MessageChecker mc);
	virtual ~SalsaCallback();

	void send(const pearl::CallbackManager& cbmanager, int user_event,
			const pearl::Event& event, pearl::CallbackData* data);

	void recv(const pearl::CallbackManager& cbmanager, int user_event,
			const pearl::Event& event, pearl::CallbackData* data);

	void enter(const pearl::CallbackManager& cbmanager, int user_event,
			const pearl::Event& event, pearl::CallbackData* data);

	double* get_results();

private:
	int rank, size;
	int stat_func, stat_mode;
	std::vector<double> result;
	std::vector<int> counter;
	MessageChecker mc;
	funcs func;
	modes mode;
	// Arrays for MPI message handling - see MPI.pattern in Scout
	std::vector<pearl::MpiMessage*> m_pending;
	std::vector<MPI_Request> m_requests;
	std::vector<int>         m_indices;
	std::vector<MPI_Status>  m_statuses;

	void completion_check();
};

#endif /* SALSACALLBACK_H_ */
