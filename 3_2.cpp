#include <mpi.h>
#include <iostream>

int main(int argc, char *argv[])
{
    int rank, comm_sz, local_sum, global_sum;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    local_sum = rank; // Cada proceso inicializa su suma local con su rango

    int step = 1;
    while (step < comm_sz)
    {
        if (rank % (2 * step) == 0)
        {
            int sender = rank + step;
            if (sender < comm_sz) // Verifica que el proceso receptor esté dentro de los límites
            {
                int received_value;
                MPI_Recv(&received_value, 1, MPI_INT, sender, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                local_sum += received_value;
            }
        }
        else if (rank % (2 * step) == step)
        {
            int receiver = rank - step;
            MPI_Send(&local_sum, 1, MPI_INT, receiver, 0, MPI_COMM_WORLD);
            break; // Proceso que envía termina su participación aquí
        }
        step *= 2; // Aumenta el tamaño del paso para la siguiente iteración
    }

    // El proceso 0 contiene la suma global
    if (rank == 0)
    {
        global_sum = local_sum;
        std::cout << "La suma global es " << global_sum << std::endl;
    }

    MPI_Finalize();
    return 0;
}
