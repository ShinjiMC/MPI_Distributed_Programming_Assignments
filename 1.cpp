#include <mpi.h>
#include <iostream>
#include <vector>
#include <iomanip>

// Función para calcular el histograma de un conjunto de datos segun las restricciones del bin
void calculateHistogram(const std::vector<double> &data, int num_bins, std::vector<int> &histogram, double min_value, double bin_width)
{
    for (auto value : data)
        if (value >= min_value && value < (num_bins * bin_width) + min_value) // Valida que el valor esta en el rango de los bins
        {
            int bin_index = static_cast<int>((value - min_value) / bin_width); // Calculo del bin actual al que pertenece
            histogram[bin_index]++;                                            // Incrementa el contador del bin actual
        }
}

int main(int argc, char *argv[])
{
    int rank, size;                       // Identificador de proceso actual y cantidad de procesos
    MPI_Init(&argc, &argv);               // Inicializa el entorno MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtiene el Identificador unico del proceso actual
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtiene el numero total de procesos en el comunicador

    // Decalaraciones de restricciones de bins
    const int num_bins = 5;
    double min_value = 0.0;
    double max_value = 5.0;
    double bin_width = (max_value - min_value) / num_bins;

    // Lista de datos a procesar
    std::vector<double> data;
    if (rank == 0)
    {
        data = {1.3, 2.9, 0.4, 0.3, 1.3, 4.4, 1.7, 0.4, 3.2, 0.3,
                4.9, 2.4, 3.1, 4.4, 3.9, 0.4, 4.2, 4.5, 4.9, 0.9};
        std::cout << "Lista de Datos: " << std::endl;
        for (auto d : data)
            std::cout << d << " ";
        std::cout << std::endl;
    }

    int data_size; // tamaño de los datos
    if (rank == 0)
        data_size = data.size();                          // Proceso 0 obtiene los datos del tamaño ya que es el inicial
    MPI_Bcast(&data_size, 1, MPI_INT, 0, MPI_COMM_WORLD); // Envia el tamaño del vector o de datos a todos los procesos

    int local_size = data_size / size;          // Tamaño de datos que recibira cada proceso
    std::vector<double> local_data(local_size); // Inicializa el vector que se enviara a cada proceso

    // Se usa para enviar los datos a cada proceso por medio de las anteriores declaraciones
    MPI_Scatter(data.data(), local_size, MPI_DOUBLE, local_data.data(), local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    std::vector<int> local_histogram(num_bins, 0);                                   // Inicaliza el histograma local de cada proceso
    calculateHistogram(local_data, num_bins, local_histogram, min_value, bin_width); // Calcula el histograma local

    std::vector<int> global_histogram(num_bins, 0); // Inicializa el histograma global
                                                    // Reduce los histogramas locales a un histograma global
    MPI_Reduce(local_histogram.data(), global_histogram.data(), num_bins, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) // Si el proceso actual es el 0 muestra los datos del histograma global
    {
        std::cout << "Histograma:" << std::endl;
        for (int i = 0; i < num_bins; i++)
            std::cout << std::fixed << std::setprecision(2) << (min_value + i * bin_width) << " - "
                      << std::fixed << std::setprecision(2) << (min_value + (i + 1) * bin_width) << ": "
                      << global_histogram[i] << std::endl;
    }

    MPI_Finalize(); // Finaliza el entorno MPI
    return 0;
}
