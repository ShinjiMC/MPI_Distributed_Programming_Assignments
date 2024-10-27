#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>

int main(int argc, char *argv[])
{
    int rank, comm_sz;
    const int n = 4; // Dimensión de la matriz cuadrada
    int block_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    // Verifica que n sea divisible por comm_sz
    if (n % comm_sz != 0)
    {
        if (rank == 0)
            std::cerr << "Error: n debe ser divisible entre el número de procesos." << std::endl;
        MPI_Finalize();
        return -1;
    }

    block_size = n / comm_sz; // Tamaño del bloque de filas para cada proceso

    // Proceso 0 inicializa la matriz y el vector completos
    std::vector<int> matrix(n * n), vector(n), local_matrix(block_size * n), local_result(block_size, 0); // Cambié local_result a block_size

    if (rank == 0)
    {
        // Inicializar la matriz y el vector con valores (por simplicidad, usaremos valores arbitrarios)
        for (int i = 0; i < n; i++)
        {
            vector[i] = 1; // Vector de 1s para simplificar la verificación
            for (int j = 0; j < n; j++)
                matrix[i * n + j] = i + j; // Ejemplo de matriz
        }
    }

    // Distribuir bloques de filas a cada proceso
    MPI_Scatter(matrix.data(), block_size * n, MPI_INT, local_matrix.data(), block_size * n, MPI_INT, 0, MPI_COMM_WORLD);

    // Distribuir el vector completo a cada proceso
    MPI_Bcast(vector.data(), n, MPI_INT, 0, MPI_COMM_WORLD);

    // Multiplicación local en cada proceso
    for (int i = 0; i < block_size; i++)
    {
        for (int j = 0; j < n; j++)
        {
            local_result[i] += local_matrix[i * n + j] * vector[j]; // Cada proceso suma su bloque de resultados
        }
    }

    // Reducir y recolectar los resultados parciales en el proceso 0
    std::vector<int> global_result(n, 0);
    MPI_Reduce(local_result.data(), global_result.data(), block_size, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); // Cambié n a block_size

    // Proceso 0 muestra el resultado final
    if (rank == 0)
    {
        std::cout << "Resultado final de la multiplicación matriz-vector:" << std::endl;
        for (int i = 0; i < n; i++)
        {
            std::cout << global_result[i] << " ";
        }
        std::cout << std::endl;
    }

    MPI_Finalize();
    return 0;
}
