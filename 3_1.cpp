#include <mpi.h>
#include <iostream>

int main(int argc, char *argv[])
{
    int rank, comm_sz, local_sum, global_sum;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    // Each process initializes its local sum with its rank (or any other data).
    local_sum = rank;

    // Check that comm_sz is a power of two
    if ((comm_sz & (comm_sz - 1)) != 0)
    {
        if (rank == 0)
            std::cerr << "Number of processes must be a power of two!" << std::endl;
        MPI_Finalize();
        return -1;
    }

    // Perform the tree-structured global sum
    int step = 1;
    while (step < comm_sz)
    {
        if (rank % (2 * step) == 0)
        {
            int received_value;
            MPI_Recv(&received_value, 1, MPI_INT, rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            local_sum += received_value;
        }
        else if (rank % step == 0)
        {
            MPI_Send(&local_sum, 1, MPI_INT, rank - step, 0, MPI_COMM_WORLD);
            break;
        }
        step *= 2;
    }

    // Process 0 holds the global sum at the end
    if (rank == 0)
    {
        global_sum = local_sum;
        std::cout << "Global sum is " << global_sum << std::endl;
    }

    MPI_Finalize();
    return 0;
}
