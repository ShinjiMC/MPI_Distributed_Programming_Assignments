## Problema 3.1

El objetivo es implementar un programa que genere un histograma utilizando la interfaz de paso de mensajes (MPI). Esto implica que el proceso 0 se encargue de leer los datos de entrada, distribuir estos datos entre los diferentes procesos y, finalmente, imprimir el histograma resultante.

### Contexto

Según la sección 2.7.1 mencionada:

1. **Generación de Datos**: Se tiene un conjunto de datos que son números de punto flotante. En el ejemplo dado, los datos son:

   ```
   1.3, 2.9, 0.4, 0.3, 1.3, 4.4, 1.7, 0.4, 3.2, 0.3,
   4.9, 2.4, 3.1, 4.4, 3.9, 0.4, 4.2, 4.5, 4.9, 0.9
   ```

2. **Rango de Datos**: Los datos se encuentran en un rango de 0 a 5. Esto significa que se desea visualizar la distribución de estos datos en este rango específico.

3. **División en Bins**: Para crear un histograma, el rango de los datos se divide en intervalos iguales, llamados "bins". En el ejemplo, se elige dividir el rango en 5 bins.

4. **Conteo de Mediciones**: Para cada bin, se cuenta cuántos datos caen dentro de ese intervalo. Esto permite observar la distribución de los datos a través de los diferentes bins.

5. **Representación del Histograma**: Finalmente, el histograma se puede representar como un gráfico de barras donde cada barra indica el número de mediciones en cada bin.

![Ejecución](.docs/text_3_1.png)

### Implementación con MPI

- **Proceso 0**: Es responsable de leer los datos iniciales y distribuirlos entre los demás procesos.
- **Distribución de Datos**: Los datos se dividen equitativamente entre los procesos disponibles. Cada proceso calcula su parte del histograma utilizando los datos que ha recibido.

- **Reducción de Resultados**: Al final, los resultados de los histogramas locales se combinan (se reducen) en el proceso 0 para formar el histograma global.

- **Impresión del Histograma**: El proceso 0 imprime el histograma final, mostrando la cantidad de datos en cada bin.

### Explicación del Código

1.  **Inclusión de Bibliotecas**:

    - Se incluyen las bibliotecas necesarias para MPI, entrada/salida estándar y manipulaciones de vectores.

2.  **Función `calculateHistogram`**:

    - Esta función toma un conjunto de datos y cuenta cuántos de esos datos caen dentro de cada bin del histograma. Utiliza el rango de los datos y el ancho de cada bin para determinar en qué bin debe contar cada valor.

    ```cpp
    void calculateHistogram(const std::vector<double>& data, int num_bins, std::vector<int>& histogram, double min_value, double bin_width) {
        for (double value : data) {
            if (value >= min_value && value < min_value + num_bins * bin_width) {
                int bin_index = static_cast<int>((value - min_value) / bin_width);
                histogram[bin_index]++;
            }
        }
    }
    ```

3.  **Función `main`**:
    - **Inicialización de MPI**: Se inicializa el entorno MPI, obteniendo el rango y el tamaño del comunicador.
    - **Definición de Parámetros**: Se establecen el número de bins, el valor mínimo y máximo del rango de datos, y se calcula el ancho de cada bin.
    - **Lectura de Datos**: El proceso 0 inicializa un vector con los datos específicos proporcionados.
    - **Comunicación de Tamaño de Datos**: El tamaño del vector de datos se transmite a todos los procesos.
    - **Distribución de Datos**: Los datos se distribuyen entre todos los procesos usando `MPI_Scatter`.
    - **Cálculo del Histograma Local**: Cada proceso calcula su propio histograma local.
    - **Reducción de Resultados**: Se utilizan `MPI_Reduce` para combinar todos los histogramas locales en un histograma global en el proceso 0.
    - **Impresión del Histograma**: El proceso 0 imprime el histograma final, mostrando cuántos datos hay en cada bin.
    - **Finalización de MPI**: Se finaliza el entorno MPI.

### Ejecución

```bash
    mpic++ -o histogram 1.cpp
    mpirun -np <numero_procesos> ./histogram
```

### Visualización

![Ejecución](.docs/Ejercicio3_1.png)

## Problema 3.2

### Contexto

El problema se basa en una simulación para estimar el valor de π utilizando el método de Monte Carlo, que aprovecha la aleatoriedad en los lanzamientos de dardos. Aquí está la descripción detallada del escenario y la lógica detrás del método:

## 1. Configuración del Tablero

- Se tiene un tablero de dardos cuadrado con un área de $$2 \text{ pies} \times 2 \text{ pies}$$ El centro del tablero (el bullseye) está en el origen del sistema de coordenadas.
- Dentro de este cuadrado, hay un círculo inscrito con un radio de 1 pie. El área del círculo es $$\pi \text{ pies}^2$$ y la del cuadrado es $$4 \text{ pies}^2$$

## 2. Proporción de Áreas

- La razón entre el área del círculo y el área del cuadrado es:
  $$
  \frac{\text{Área del círculo}}{\text{Área del cuadrado}} = \frac{\pi}{4}
  $$
- Por lo tanto, si lanzamos dardos de manera aleatoria sobre el tablero, la fracción de dardos que caerán dentro del círculo debería ser aproximadamente igual a $$ \frac{\pi}{4} $$

## 3. Simulación de Lanzamientos

- Se realiza un número total de lanzamientos de dardos.
- Para cada lanzamiento, se generan dos números aleatorios **x** y **y** que representan la posición del dardo en el cuadrado, donde ambos están distribuidos uniformemente entre -1 y 1.
- Se calcula la distancia al origen utilizando la fórmula:
  $$
  \text{distancia cuadrada} = x^2 + y^2
  $$
- Si esta distancia cuadrada es menor o igual a 1, el dardo ha caído dentro del círculo, y se incrementa un contador.

## 4. Estimación de π

- Después de realizar todos los lanzamientos, se estima el valor de π utilizando la fórmula:
  $$
  \pi \approx 4 \times \frac{\text{número en círculo}}{\text{total de lanzamientos}}
  $$
- Este cálculo se realiza en el proceso 0.

## 5. Paralelización con MPI

- Se utilizará MPI para distribuir la carga de trabajo entre varios procesos.
- El proceso 0 leerá el número total de lanzamientos y lo difundirá a los otros procesos.
- Cada proceso realizará un número local de lanzamientos y contará cuántos dardos cayeron dentro del círculo.
- Finalmente, se utilizará `MPI_Reduce` para sumar los contadores locales de dardos que cayeron en el círculo, obteniendo así un total global que se usará para estimar π en el proceso 0.
