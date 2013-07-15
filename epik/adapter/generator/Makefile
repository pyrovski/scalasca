all: gen_event_files gen_shmem_files

gen_event_files : gen_event_files.cpp
	g++ -g $< -o $@

gen_shmem_files : gen_shmem_files.cpp
	g++ -Wall -ansi -pedantic $< -o $@

clean :
	rm -rf a.out core SHMEM_COLLS gen_event_files gen_shmem_files *.gen epilog.gen.tex
	rm -f *.dvi *.log *.aux
