#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <mpi.h>
#include <set>

void merge(const std::vector<int> &left, const std::vector<int> &right, std::vector<int> &result)
{
    size_t i = 0, j = 0, k = 0;

    while (i < left.size() && j < right.size())
    {
        if (left[i] < right[j])
        {
            result[k++] = left[i++];
        }
        else
        {
            result[k++] = right[j++];
        }
    }

    while (i < left.size())
    {
        result[k++] = left[i++];
    }

    while (j < right.size())
    {
        result[k++] = right[j++];
    }
}

int main(int argc, char *argv[])
{
    int rank, size;
    int n = 16; // Tamaño total de la lista (debe ser divisible por el número de procesos)

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Generar 16 valores únicos aleatorios en el proceso 0
    std::vector<int> global_data;
    if (rank == 0)
    {
        std::set<int> unique_numbers;
        std::default_random_engine generator;
        std::uniform_int_distribution<int> distribution(1, 100);

        // Generar números únicos
        while (unique_numbers.size() < n)
        {
            unique_numbers.insert(distribution(generator));
        }

        // Copiar los números únicos en el vector global
        global_data.assign(unique_numbers.begin(), unique_numbers.end());

        global_data = {100, 80, 102, 34, 2, 7, 11, 8, 10, 23, 17, 89, 36, 10, 22, 2};
        // Imprimir los valores generados
        std::cout << "Valores aleatorios generados (únicos):\n";
        for (const auto &value : global_data)
        {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }

    // Broadcast the global data to all processes
    global_data.resize(n); // Asegurarse de que todos tengan el mismo tamaño
    MPI_Bcast(global_data.data(), n, MPI_INT, 0, MPI_COMM_WORLD);

    // Repartir los datos locales
    int local_n = n / size; // Número de elementos por proceso
    std::vector<int> local_data(local_n);

    // Asignar los datos locales
    for (int i = 0; i < local_n; i++)
    {
        local_data[i] = global_data[rank * local_n + i];
    }

    // Ordenar localmente
    std::sort(local_data.begin(), local_data.end());

    // Imprimir las listas locales en cada proceso
    std::cout << "Proceso " << rank << " datos locales:\n";
    for (const auto &value : local_data)
    {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    // Fusión utilizando comunicación estructurada en árbol
    std::vector<int> merged_data;
    if (rank == 0)
    {
        merged_data = local_data; // Inicia con la lista local del proceso 0
        for (int i = 1; i < size; i++)
        {
            std::vector<int> temp_data(local_n);
            MPI_Recv(temp_data.data(), local_n, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // Fusionar el vector global con el recibido
            std::vector<int> temp_merged_data(merged_data.size() + temp_data.size());
            merge(merged_data, temp_data, temp_merged_data);
            merged_data = std::move(temp_merged_data); // Actualizar el merged_data
        }
    }
    else
    {
        // Enviar datos ordenados a proceso 0
        MPI_Send(local_data.data(), local_n, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    // El proceso 0 imprime el resultado final
    if (rank == 0)
    {
        std::cout << "Resultado final ordenado:\n";
        for (const auto &value : merged_data)
        {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }

    MPI_Finalize();
    return 0;
}
