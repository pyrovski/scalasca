SHELL=/bin/sh

all:
	@case `ls -d build-* 2> /dev/null | wc -l | sed -e 's/[^0-9]//g'` in \
	  0) echo "ERROR: No build configuration found. Run configure first." \
	     ;; \
	  1) cd build-*; ${MAKE} all \
	     ;; \
	  *) if [ -n "${CFG}" -a -d build-${CFG} ]; then \
	        cd build-${CFG}; ${MAKE} all; \
	     else \
	        echo "ERROR: Select build configuration via CFG= out of"; \
	        ls -d1 build-* | sed -e 's/build-/* /' ; \
	        exit 1 ; \
	     fi ;; \
	esac

install:
	@case `ls -d build-* 2> /dev/null | wc -l | sed -e 's/[^0-9]//g'` in \
	  0) echo "ERROR: No build configuration found. Run configure first." \
	     ;; \
	  1) cd build-*; ${MAKE} install \
	     ;; \
	  *) if [ -n "${CFG}" -a -d build-${CFG} ]; then \
	        cd build-${CFG}; ${MAKE} install; \
	     else \
	        echo "ERROR: Select build configuration via CFG= out of"; \
	        ls -d1 build-* | sed -e 's/build-/* /' ; \
	        exit 1 ; \
	     fi ;; \
	esac

clean:
	@case `ls -d build-* 2> /dev/null | wc -l | sed -e 's/[^0-9]//g'` in \
	  0) echo "ERROR: No build configuration found. Run configure first." \
	     ;; \
	  1) cd build-*; ${MAKE} clean \
	     ;; \
	  *) if [ -n "${CFG}" -a -d build-${CFG} ]; then \
	        cd build-${CFG}; ${MAKE} clean; \
	     else \
	        echo "ERROR: Select build configuration via CFG= out of"; \
	        ls -d1 build-* | sed -e 's/build-/* /' ; \
	        exit 1 ; \
	     fi ;; \
	esac

