# --------------------------------------------------------------------------
@ Region
# --------------------------------------------------------------------------

ENTER
4
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
rid
region identifier of the region being entered
elg_ui8
metv[]
metric values

ENTER_CS
4
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
csid
call-site identifier of the call-site being entered
elg_ui8
metv[]
metric values

EXIT
3
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui8
metv[]
metric values

# --------------------------------------------------------------------------
@ MPI-1
# --------------------------------------------------------------------------

MPI_SEND
6
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
dlid
destination location identifier
elg_ui4
cid
communicator identifier
elg_ui4
tag
message tag
elg_ui4
sent
bytes sent

MPI_RECV
6
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
slid
source location identifier
elg_ui4
cid
communicator identifier
elg_ui4
tag
message tag
elg_ui4
recvd
bytes received

MPI_COLLEXIT
8
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui8
metv[]
metric values
elg_ui4
rid
region identifier of the region being exited
elg_ui4
rlid
root location identifier
elg_ui4
cid
communicator identifier
elg_ui4
sent
bytes sent
elg_ui4
recvd
bytes received

# --------------------------------------------------------------------------
@ MPI-1 Non-blocking
# --------------------------------------------------------------------------

MPI_SEND_COMPLETE
3
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
reqid
request identifier of completed send

MPI_RECV_REQUEST
3
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
reqid
receive request identifier

MPI_REQUEST_TESTED
3
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
reqid
identifier of tested request

MPI_CANCELLED
3
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
reqid
identifier of cancelled request

# --------------------------------------------------------------------------
@ MPI-2 One-sided Operations
# --------------------------------------------------------------------------

MPI_PUT_1TS
6
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
dlid
destination location identifier
elg_ui4
wid
window identifier
elg_ui4
rmaid
RMA operation identifier
elg_ui4
nbytes
message length in bytes

MPI_PUT_1TE
5
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
slid
source location identifier
elg_ui4
wid
window identifier
elg_ui4
rmaid
RMA operation identifier

MPI_PUT_1TE_REMOTE
5
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
dlid
destination location identifier
elg_ui4
wid
window identifier
elg_ui4
rmaid
RMA operation identifier

MPI_GET_1TO
3
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
rmaid
RMA operation identifier

MPI_GET_1TS
6
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
dlid
destination location identifier
elg_ui4
wid
window identifier
elg_ui4
rmaid
RMA operation identifier
elg_ui4
nbytes
message length in bytes

MPI_GET_1TS_REMOTE
6
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
dlid
destination location identifier
elg_ui4
wid
window identifier
elg_ui4
rmaid
RMA operation identifier
elg_ui4
nbytes
message length in bytes

MPI_GET_1TE
5
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
slid
source location identifier
elg_ui4
wid
window identifier
elg_ui4
rmaid
RMA operation identifier

MPI_WINEXIT
6
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui8
metv[]
metric values
elg_ui4
wid
window identifier
elg_ui4
cid
communicator identifier
elg_ui1
synex
synchronization exit flag

MPI_WINCOLLEXIT
4
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui8
metv[]
metric values
elg_ui4
wid
window identifier

MPI_WIN_LOCK
5
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
llid
lock location identifier
elg_ui4
wid
window identifier
elg_ui1
ltype
lock type

MPI_WIN_UNLOCK
4
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
llid
lock location identifier
elg_ui4
wid
window identifier

# --------------------------------------------------------------------------
@ Generic One-sided Operations
# --------------------------------------------------------------------------

PUT_1TS
5
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
dlid
destination location identifier
elg_ui4
rmaid
RMA operation identifier
elg_ui4
nbytes
message length in bytes

PUT_1TE
4
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
slid
source location identifier
elg_ui4
rmaid
RMA operation identifier

PUT_1TE_REMOTE
4
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
dlid
destination location identifier
elg_ui4
rmaid
RMA operation identifier

GET_1TS
5
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
dlid
destination location identifier
elg_ui4
rmaid
RMA operation identifier
elg_ui4
nbytes
message length in bytes

GET_1TS_REMOTE
5
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
dlid
destination location identifier
elg_ui4
rmaid
RMA operation identifier
elg_ui4
nbytes
message length in bytes

GET_1TE
4
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
slid
source location identifier
elg_ui4
rmaid
RMA operation identifier

COLLEXIT
7
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui8
metv[]
metric values
elg_ui4
rlid
root location identifier
elg_ui4
cid
communicator identifier
elg_ui4
sent
bytes sent
elg_ui4
recvd
bytes received

ALOCK
3
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
lkid
identifier of the lock being acquired

RLOCK
3
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
lkid
identifier of the lock being released

# --------------------------------------------------------------------------
@ OpenMP
# --------------------------------------------------------------------------

OMP_FORK
2
elg_ui4
lid
location identifier
elg_d8
time
time stamp

OMP_JOIN
2
elg_ui4
lid
location identifier
elg_d8
time
time stamp

OMP_ALOCK
3
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
lkid
identifier of the lock being acquired

OMP_RLOCK
3
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui4
lkid
identifier of the lock being released

OMP_COLLEXIT
3
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui8
metv[]
metric values

# --------------------------------------------------------------------------
@ EPILOG Internal
# --------------------------------------------------------------------------

LOG_OFF
3
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui8
metv[]
metric values

LOG_ON
3
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui8
metv[]
metric values

ENTER_TRACING
3
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui8
metv[]
metric values

EXIT_TRACING
3
elg_ui4
lid
location identifier
elg_d8
time
time stamp
elg_ui8
metv[]
metric values

