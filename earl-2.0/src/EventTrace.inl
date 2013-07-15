/****************************************************************************
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

// inline definitions

inline std::string EventTrace::get_name() const { return name; }
inline std::string EventTrace::get_path() const { return path; }

inline long EventTrace::get_nevents() const { return btrace.get_nevents(); }
inline long EventTrace::get_nfiles() const { return btrace.get_nfiles(); } 
inline long EventTrace::get_nregs() const { return btrace.get_nregs(); }
inline long EventTrace::get_ncsites() const { return btrace.get_ncsites(); }
inline long EventTrace::get_nmets() const { return btrace.get_nmets(); }

inline long EventTrace::get_nmachs() const { return btrace.get_grid()->get_nmachs(); }
inline long EventTrace::get_nnodes() const { return btrace.get_grid()->get_nnodes(); }
inline long EventTrace::get_nprocs() const { return btrace.get_grid()->get_nprocs(); }
inline long EventTrace::get_nthrds() const { return btrace.get_grid()->get_nthrds(); }
inline long EventTrace::get_nlocs()  const { return btrace.get_grid()->get_nlocs(); }
inline long EventTrace::get_ncoms()  const { return btrace.get_grid()->get_ncoms(); }
inline long EventTrace::get_nwins()  const { return btrace.get_grid()->get_nwins(); }
inline long EventTrace::get_ncarts() const { return btrace.get_grid()->get_ncarts(); }

inline bool EventTrace::is_type(long pos, etype type) { return event(pos).is_type(type); }

inline std::string   EventTrace::get_file(long file_id) const { return btrace.get_file(file_id); }   
inline Region*       EventTrace::get_reg(long reg_id) const { return btrace.get_reg(reg_id); }
inline Callsite*     EventTrace::get_csite(long csite_id) const { return btrace.get_csite(csite_id); }
inline Metric*       EventTrace::get_met(long met_id) const { return btrace.get_met(met_id); }

inline Machine*      EventTrace::get_mach(long mach_id) const { return btrace.get_grid()->get_mach(mach_id); }
inline Node*         EventTrace::get_node(long mach_id, long node_id) const 
				{ return btrace.get_grid()->get_node(mach_id, node_id); }
inline Process*      EventTrace::get_proc(long proc_id) const { return btrace.get_grid()->get_proc(proc_id); }
inline Thread*       EventTrace::get_thrd(long proc_id, long thrd_id) const 
				{ return btrace.get_grid()->get_thrd(proc_id, thrd_id); }

inline Location*     EventTrace::get_loc(long loc_id) const { return btrace.get_grid()->get_loc(loc_id); }
inline Communicator* EventTrace::get_com(long com_id) const { return btrace.get_grid()->get_com(com_id); }
inline MPIWindow*    EventTrace::get_win(long win_id) const { return btrace.get_grid()->get_win(win_id); }
inline Cartesian*    EventTrace::get_cart(long cart_id) const { return btrace.get_grid()->get_cart(cart_id); }


