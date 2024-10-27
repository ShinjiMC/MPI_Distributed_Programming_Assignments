#include <iostream>
#include <vector>
#include <mpi.h>

#define n 4
#define m 4

void matrix_vector_product(const std::vector<double> &matrix, const std::vector<double> &vector, std::vector<double> &result, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        result[i] = 0.0;
        for (int j = 0; j < cols; j++)
            result[i] += matrix[i * cols + j] * vector[j];
    }
}

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int local_n = n / size;                        // Número de filas de la matriz asignadas a cada proceso
    std::vector<double> b(m);                      // Vector de entrada
    std::vector<double> c;                         // Resultado final en el proceso 0
    std::vector<double> local_matrix(local_n * m); // Submatriz para cada proceso
    std::vector<double> local_c(local_n);          // Resultado parcial en cada proceso

    // Inicializar el vector y la matriz local en cada proceso
    for (int j = 0; j < m; j++)
        b[j] = j + 1; // Ejemplo de inicialización para el vector

    int start_row = rank * local_n;
    for (int i = 0; i < local_n; i++)
        for (int j = 0; j < m; j++)
            local_matrix[i * m + j] = (start_row + i) + j;

    // Imprimir la matriz y el vector en el proceso 0
    if (rank == 0)
    {
        std::cout << "Matrix:\n";
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < m; j++)
                std::cout << i + j << " ";
            std::cout << "\n";
        }

        std::cout << "\nVector:\n";
        for (auto const &j : b)
            std::cout << j << " ";
        std::cout << "\n\n";

        c.resize(n); // Reservar espacio para el resultado final en el proceso 0
    }

    // Enviar el vector `b` a todos los procesos
    MPI_Bcast(b.data(), m, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Calcular el producto local (submatriz * vector)
    matrix_vector_product(local_matrix, b, local_c, local_n, m);

    // Recoger los resultados parciales de cada proceso en el vector final `c`
    MPI_Gather(local_c.data(), local_n, MPI_DOUBLE, c.data(), local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Imprimir el resultado en el proceso 0
    if (rank == 0)
    {
        std::cout << "Result = ";
        for (auto const &i : c)
            std::cout << i << " ";
        std::cout << "\n";
    }

    MPI_Finalize();

    return 0;
}
