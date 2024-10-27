#include <mpi.h>
#include <iostream>
#include <cstdlib> // Para rand() y RAND_MAX
#include <cmath>   // Para sqrt()
#include <ctime>   // Para time()
#include <cstring> // Para manejar los argumentos

// Función para generar un número aleatorio en el rango [-1, 1]
double random_double()
{
    return 2.0 * rand() / RAND_MAX - 1.0;
}

int main(int argc, char *argv[])
{
    int rank, size;
    long long int tosses_per_process, total_tosses, number_in_circle = 0, global_number_in_circle;

    // Inicializar MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) // Comprobamos si se ha pasado el número correcto de argumentos
    {
        if (rank == 0)
        {
            std::cerr << "Uso: mpirun -np <num_procesos> ./montecarlo_pi <num_lanzamientos>" << std::endl;
        }
        MPI_Finalize(); // Finalizar MPI si no se proporcionan los argumentos correctamente
        return -1;
    }

    if (rank == 0)
    {
        // Proceso 0 obtiene el número total de lanzamientos desde los argumentos
        total_tosses = std::atoll(argv[1]); // Convertir argumento de cadena a número
    }

    // Proceso 0 difunde el número total de lanzamientos a los demás procesos
    MPI_Bcast(&total_tosses, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

    // Asegurarse de que todos los procesos participen en la simulación
    tosses_per_process = total_tosses / size;
    long long int remainder_tosses = total_tosses % size; // Para manejar los restos

    srand(time(NULL) + rank); // Inicializa la semilla del generador aleatorio diferente para cada proceso

    // Cada proceso realiza sus lanzamientos
    for (long long int toss = 0; toss < tosses_per_process; toss++)
    {
        double x = random_double();
        double y = random_double();
        double distance_squared = x * x + y * y;
        if (distance_squared <= 1.0)
        {
            number_in_circle++;
        }
    }

    // Si hay restos de lanzamientos, el proceso 0 los maneja
    if (rank == 0)
    {
        for (long long int toss = 0; toss < remainder_tosses; toss++)
        {
            double x = random_double();
            double y = random_double();
            double distance_squared = x * x + y * y;
            if (distance_squared <= 1.0)
            {
                number_in_circle++;
            }
        }
    }

    // Reducir el número total de aciertos en el círculo a un valor global
    MPI_Reduce(&number_in_circle, &global_number_in_circle, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // El proceso 0 estima y muestra el valor de pi
    if (rank == 0)
    {
        double pi_estimate = 4.0 * global_number_in_circle / total_tosses;
        std::cout << "Estimación de pi: " << pi_estimate << std::endl;
    }

    // Finalizar MPI
    MPI_Finalize();

    return 0;
}
