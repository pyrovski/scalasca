#!/bin/sh

# -- Uncomment for stand-alone use
#CFGecho () {
#  echo "$1"
#}

#ErrorAndExit () {
#  echo "ERROR: $1"
#  exit 1
#}

CFGecho "Configuring MPI Adapter"

# File names
DEFS=mpi-support.defs
HEADER=epk_defs_mpi.h
LOG=mpi-support.log
ERRLOG=/dev/null

# generate spec file with all supported calls
if [ -z "${MPILIB}" ]
then
  CFGecho "  ERROR: Cannot determine location of MPI library"
  CFGecho "  INFO: Reverting to Scalasca default wrapper set"
  # choosing default set (only MPI 1.x function calls)
  cp ../utils/build/default_wrappers_mpi_1x.txt ${DEFS}
else
  CFGecho "- Using ${MPILIB}"
  ${MPINM} -g -P ${MPILIB} > /tmp/mpilib-nm-log-$$ 2> /tmp/mpilib-nm-error-$$

  # 2nd try for stripped libraries on Linux
  if [ ! -s /tmp/mpilib-nm-log-$$ -a "${SYSTEM}" = "Linux" ] && \
     echo "${MPILIB}" | grep -q -e '.*\.so' ;
  then
    ${MPINM} -D -g -P ${MPILIB} > /tmp/mpilib-nm-log-$$ 2> /tmp/mpilib-nm-error-$$
  fi

  if [ -s /tmp/mpilib-nm-log-$$ ]
  then
    grep '^PMPI_' /tmp/mpilib-nm-log-$$ | \
    cut -d " " -f 1 | \
    grep -v _\$ | \
    tr "[a-z]" "[A-Z]" | \
    grep -v "_FN" | \
    sed -e "s/^P\([a-zA-Z0-9_]*\).*/\1/" | \
    sort -u > ${DEFS} 
  else
    CFGecho "  ERROR: Cannot determine wrapper support from MPI library"
    CFGecho "  ERROR: `cat /tmp/mpilib-nm-error-$$`"
    CFGecho "  INFO: Reverting to Scalasca default wrapper set"
    # choosing default set (only MPI 1.x function calls)
    cp ../utils/build/default_wrappers_mpi_1x.txt ${DEFS}
  fi
  rm -f /tmp/mpilib-nm-log-$$ /tmp/mpilib-nm-error-$$
fi

# Generating header to activate supported wrappers
echo "#ifndef EPK_DEFS_MPI_H" > ${HEADER}
echo "#define EPK_DEFS_MPI_H" >> ${HEADER}
cat ${DEFS} | \
while read line; do \
  echo "#define HAS_"$line >> ${HEADER}; \
done

if [  "${NOFOR}" != "yes" ]
then
  # check which fortran constants are defined
  # MPI_ARGV_NULL, MPI_ARGVS_NULL and MPI_ERRCODRS_IGNORE are specific 
  # to the spawn interface and currently ignored
  if [ -z "${MDF}" ]; then
    echo "include Makefile.defs" > Makefile.conftest
  else
    echo "include Makefile.defs.be" > Makefile.conftest
  fi
  if [ -n "${MPI}" ]
  then
    if [ -f "../mf/mpi-${MPI}.def" ]
    then
      echo "include ../mf/mpi-${MPI}.def" >> Makefile.conftest
    fi
  fi   
  cat >> Makefile.conftest <<[[EOT]]

conftest: conftest.f90
	@\$(MPIF90) conftest.f90 -o conftest 2> /dev/null > /dev/null
[[EOT]]

  for mpiconst in MPI_BOTTOM MPI_IN_PLACE MPI_STATUS_IGNORE MPI_STATUSES_IGNORE MPI_UNWEIGHTED;
  do
    cat > conftest.f90 <<[[EOT]]
      program conftest
      implicit none
      include 'mpif.h'
      write (*,*) $mpiconst
      end program conftest
[[EOT]]
    ${MK} -f Makefile.conftest conftest 2> /dev/null > /dev/null
    if [ -f conftest ]; then
      echo "#define HAS_"$mpiconst >> $HEADER;
    fi
    # clean up before next test
    rm -f conftest.f90 conftest 
done
rm -f Makefile.conftest
fi

echo "#endif" >> ${HEADER}

# generate warning and error log
cp ../utils/build/${LOG}.tmpl ${LOG}.c
cc -E ${EXCL_MPI_DEFS} ${MPIEXTRA} ${LOG}.c 2> ${ERRLOG} | egrep "(Warning|Error)" > ${LOG}

# create of target counts for each group
grep -v epk_defs_mpi.h ${LOG}.c > ${LOG}_target.c
cc -E ${EXCL_MPI_DEFS} ${MPIEXTRA} ${LOG}_target.c 2> ${ERRLOG} | egrep "(Warning|Error)" > ${LOG}_target
grep group ${LOG}_target | \
  sed -e "s/^.*'\([^']*\)'[^']*$/\1/" | \
  sort | uniq -c | \
  sed -e "s/[^0-9]*\([0-9][0-9]*\)[^a-zA-Z0-9_][^a-zA-Z0-9_]*\([a-zA-Z_0-9]*\).*$/TARGET_COUNT_\2=\1/" | \
  tr [a-z] [A-Z] > target_counts.tmp

# create of missed counts for each group
grep group ${LOG} | \
  sed -e "s/^.*'\([^']*\)'[^']*$/\1/" | \
  sort | uniq -c | \
  sed -e "s/[^0-9]*\([0-9][0-9]*\)[^a-zA-Z0-9_][^a-zA-Z0-9_]*\([a-zA-Z_0-9]*\).*$/MISSED_COUNT_\2=\1/" | \
 tr [a-z] [A-Z] > missed_counts.tmp

# source generate files
. ./target_counts.tmp
. ./missed_counts.tmp

# if error log is non-zero size then output a warning
if [ "`wc -l ${LOG} | sed -e 's/[ ]*\(.*\)$/\1/' | cut -d ' ' -f 1`" != "0" ]
then
  CFGecho "  NOTE: MPI library does not support interposition on all calls."

  INCOMPLETE_GROUPS=`sed -e "s/^MISSED_COUNT_\([a-zA-Z0-9_]*\)=.*$/\1/" -e "s/\n/ /g" missed_counts.tmp`
  for grp in $INCOMPLETE_GROUPS;
  do
    # TODO: Currently Scout relies on full RMA support because it uses
    #       -DHAS_MPI2_1SIDED to guard around RMA analysis
    if [ "$grp" = "RMA" ]; then
      DISABLE_MPI_RMA_SUPPORT=yes
    fi
    if [ "`eval echo \\${TARGET_COUNT_${grp}}`" -eq "`eval echo \\${MISSED_COUNT_${grp}}`" ]; then
      CFGecho "        Support for group $grp is not available."
    else
      CFGecho "        Support for group $grp is incomplete."
      # write list of missing functions in incomplete groups.
      if [ ! -f ${LOG_ANNEX} ]; then
        echo "" > ${LOG_ANNEX}
        echo "- List of MPI calls without interposition support:" >> ${LOG_ANNEX}
      fi
      #echo "" >> ${LOG_ANNEX}
      echo "  Group: $grp" >> ${LOG_ANNEX}
      grep -i "group '$grp'" ${LOG} | \
          sed -e "s/^.*\([0-9][0-9]*\.[0-9\]*\)[^']*'\([^']*\).*$/      \2 (MPI \1)/" >> ${LOG_ANNEX}
    fi
  done

fi

# output each manually disabled group
DISABLED_GROUPS=`echo ${EXCL_MPI_DEFS} | sed -e 's,-DNO_MPI_,,g' -e 's,EXTRA,,g'`
for grp in ${DISABLED_GROUPS};
do
  if [ "${grp}" = "IO" -a "${NONSTDMPIO}" = "yes" ]
  then
    CFGecho "        Support for group IO disabled (non-standard implementation)"
  elif [ "${grp}" = "EXT" -a "${NONSTDMPIEXT}" = "yes" ]
  then
    CFGecho "        Support for group EXT disabled (non-standard implementation)"
  else
    CFGecho "        Support for group $grp manually disabled"
  fi
done

# check whether for core or full wrapper support
# note: if EXTRA if found then -DNO_MPI_EXTRA guard is enabled
if [ ! -z "`echo ${EXCL_MPI_DEFS} | grep EXTRA`" ]; then
  CFGecho "  NOTE: Analysis restricted to core MPI functions (communication, synchronisation)"
  CFGecho "        Use --enable-all-mpi-wrappers to enable full MPI support"
fi

# clean up
rm -f target_counts.tmp missed_counts.tmp ${LOG}.c ${LOG}_target \
  ${LOG}_target.c 

