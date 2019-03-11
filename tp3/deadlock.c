#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"

int main (int argc, char **argv) {
    MPI_Init (&argc, &argv);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int n = 0;
    int i;
    if (world_rank == 0) {
        int *tab0;
        tab0 = malloc(sizeof(int) * n);
        for (i = 0; i < n; i ++) {
            tab0[i] = i;
        }
        MPI_Send(tab0, n, MPI_INT, 1, 0, MPI_COMM_WORLD);
        int *tab3;
        tab3 = malloc(sizeof(int) * n);
        MPI_Recv(tab3, n, MPI_INT, 1, 0, MPI_COMM_WORLD, NULL);
    } else {
        if (world_rank == 1) {
            int *tab1;
            tab1 = malloc(sizeof(int) * n);
            for (i = 0; i < n; i ++) {
                tab1[i] = n + i;
            }
            int *tab2;
            tab2 = malloc(sizeof(int) * n);
            MPI_Recv(tab2, n, MPI_INT, 0, 0, MPI_COMM_WORLD, NULL);
            //for (i = 0; i < n; i++) {
              //  printf("tab[%i] = %i\n", i, tab2[i]);
                //}
            MPI_Send(tab1, n, MPI_INT, 0, 0, MPI_COMM_WORLD);    
        }
    }
    
    


    MPI_Finalize();
    exit (0);
}