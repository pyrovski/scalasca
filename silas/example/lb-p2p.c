#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <epik_user.h>
#include <elg_pform.h>

#define MAX_ITER 100
#define BUF_SIZE 1*1024*1024

int load_balanced(int argc, char **argv);
void foo(double time);
void bar(double time);

int load_balanced(int argc, char **argv)
{
    if ((argc > 1) && (strncmp(argv[1],"--unbalanced", 10) == 0))
        return 0;
    else
        return 1;
}

inline double get_wtime()
{
    /* Use the high resolution timer of the measurement system. */
    return elg_pform_wtime();
}

void foo(double time)
{
#pragma pomp inst begin(foo)
    double end_time = get_wtime() + time;

    while (get_wtime() < end_time)
        ;
#pragma pomp inst end(foo)
}

void bar(double time)
{
#pragma pomp inst begin(bar)
    double end_time = get_wtime() + time;

    while (get_wtime() < end_time)
        ;
#pragma pomp inst end(bar)
}

int main(int argc, char** argv)
{
#pragma pomp inst init
#pragma pomp inst begin(main)
    int i,rank,size;
    double load;
    char buffer[BUF_SIZE];
    
    elg_pform_init();

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (load_balanced(argc, argv))
        load = 1.5;
    else
        load = 2 - (rank % 2);

    MPI_Barrier(MPI_COMM_WORLD);

    for (i = 0; i < MAX_ITER; i++)
    {
        printf("[%3i] Iteration %i\n", rank, i);

        if ((rank % 4) == 0)
        {   
            foo(load);
            bar(1);
            MPI_Send(buffer, BUF_SIZE, MPI_INT, ((rank + 2) % size), 0, MPI_COMM_WORLD);
            bar(1);
            MPI_Send(buffer, BUF_SIZE, MPI_INT, ((rank + 1) % size), 0, MPI_COMM_WORLD);
            bar(1);
        }

        if ((rank % 4) == 1)
        {   
            foo(load);
            bar(1);
            MPI_Send(buffer, 1*1024*1024, MPI_INT, ((rank + 2) % size), 0, MPI_COMM_WORLD);
            bar(1);
            MPI_Recv(buffer, 1*1024*1024, MPI_INT, ((rank - 1 + size) % size), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            bar(1);
        }

        if ((rank % 4) == 2)
        {   
            foo(load);
            bar(1);
            MPI_Recv(buffer, 1*1024*1024, MPI_INT, ((rank - 2 + size) % size), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            bar(1);
            MPI_Send(buffer, 1*1024*1024, MPI_INT, ((rank + 1) % size), 0, MPI_COMM_WORLD);
            bar(1);
        }
        
        if ((rank % 4) == 3)
        {   
            foo(load);
            bar(1);
            MPI_Recv(buffer, 1*1024*1024, MPI_INT, ((rank - 2 + size) % size), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            bar(1);
            MPI_Recv(buffer, 1*1024*1024, MPI_INT, ((rank - 1 + size) % size), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            bar(1);
        }
    }

    MPI_Finalize();
#pragma pomp inst end(main)

    return EXIT_SUCCESS;
}
