// ---------------------------------------------------------
// Título: Esqueletización de Imágenes y Poda Voraz
// Autores: Gabriel Ortiz Sepulveda
// Fecha de elaboración: 24/03/2026
// Materia: Análisis y diseño de algoritmos (ADA)
// Fuente: CPP Reference (std::vector, std::iostream)
// ---------------------------------------------------------

// --- Inclusión de Librerías ---
// iostream: Necesaria para la entrada y salida de datos, como imprimir la matriz en la consola (usando 'cout').
#include <iostream>
// vector: Proporciona el contenedor 'std::vector', que usamos para crear nuestra matriz dinámica.
#include <vector>
// iomanip: (Aunque no se usa activamente con funciones como setw), se incluye a menudo para formatear la salida.
#include <iomanip>

// --- Espacio de Nombres ---
// Usamos el espacio de nombres estándar para evitar escribir 'std::' repetidamente.
using namespace std;

// --- Definición de Tipos ---
// Se crea un alias de tipo 'Matrix' para 'vector<vector<int>>' usando la sintaxis moderna de C++11.
// Esto mejora la legibilidad, permitiéndonos escribir 'Matrix' en lugar de la declaración completa del tipo.
using Matrix = vector<vector<int>>;

// --- Funciones de Utilidad ---

// Función para imprimir una matriz en la consola.
// Recibe una referencia constante a la matriz ('const Matrix& img') para evitar hacer una copia costosa de toda la matriz.
void imprimirMatriz(const Matrix& img, string titulo) {
    cout << "\n" << titulo << ":" << endl; // Imprime el título proporcionado para la matriz.
    // Inicia un bucle 'for-each' para iterar sobre cada 'fila' en la 'img'.
    for (const auto& fila : img) {
        // Dentro de cada fila, inicia otro bucle 'for-each' para iterar sobre cada 'pixel'.
        for (int pixel : fila) {
            // Usa un operador ternario: si el valor del 'pixel' es 1, imprime " # "; de lo contrario, imprime " . ".
            cout << (pixel == 1 ? " # " : " . ");
        }
        cout << endl; // Al final de cada fila, imprime un salto de línea para pasar a la siguiente.
    }
}

// --- Funciones para Crear Casos de Prueba ---

void crearHNormal(Matrix& img) {
    for (int i = 5; i <= 14; ++i) {
        img[i][5] = 1; img[i][6] = 1;   // Dibuja las dos líneas verticales de la 'H'.
        img[i][13] = 1; img[i][14] = 1; // Cada línea tiene 2 píxeles de grosor.
    }
    for (int j = 7; j <= 12; ++j) {
        img[9][j] = 1; img[10][j] = 1; // Dibuja la barra horizontal de la 'H', también de 2 píxeles de grosor.
    }
}

void crearHConRuido(Matrix& img) {
    crearHNormal(img);            // Primero, crea la 'H' normal.
    img[7][4] = 1; img[7][3] = 1; // Añade una pequeña rama de ruido de 2 píxeles en el lado izquierdo.
    img[12][9] = 1;               // Añade un píxel de ruido aislado (isla) dentro de la 'H'.
}

void crearCuadradoRelleno(Matrix& img) {
    // Bucle anidado para rellenar un área cuadrada de la matriz.
    for (int i = 5; i <= 14; ++i) {
        for (int j = 5; j <= 14; ++j) img[i][j] = 1;
    }
}

// --- Algoritmo de Zhang-Suen ---

// Función para contar los vecinos activos (valor 1) de un píxel en (r, c) usando conectividad de 8.
int contarVecinos(const Matrix& m, int r, int c) {
    // Devuelve la suma de los valores de los 8 píxeles circundantes.
    // Dado que los píxeles son 0 (fondo) o 1 (forma), la suma es igual al número de vecinos que son parte de la forma.
    return m[r-1][c] + m[r-1][c+1] + m[r][c+1] + m[r+1][c+1] +
           m[r+1][c] + m[r+1][c-1] + m[r][c-1] + m[r-1][c-1];
}

// Función para contar las transiciones de 0 a 1 en el vecindario de un píxel.
int contarTransiciones(const Matrix& m, int r, int c) {
    // Se define un array 'p' que contiene los valores de los 8 vecinos en orden horario (P2, P3, ..., P9).
    // Se añade P2 de nuevo al final (p[8]) para cerrar el ciclo y facilitar el bucle.
    int p[] = { m[r-1][c], m[r-1][c+1], m[r][c+1], m[r+1][c+1],
                m[r+1][c], m[r+1][c-1], m[r][c-1], m[r-1][c-1], m[r-1][c] };
    int count = 0; // Inicializa el contador de transiciones.
    // Se itera sobre los 8 pares de vecinos consecutivos (p[0]-p[1], p[1]-p[2], ..., p[7]-p[8]).
    for (int i = 0; i < 8; ++i) {
        // Si un vecino es 0 (fondo) y el siguiente es 1 (forma), se ha encontrado una transición.
        if (p[i] == 0 && p[i+1] == 1) count++;
    }
    return count; // Devuelve el número total de transiciones 0->1.
}

void adelgazar(Matrix& img) {
    bool cambio = true; // Bandera de control del bucle. Se mantiene en 'true' mientras la imagen siga cambiando.
    const int filas = img.size(); // Obtiene el número de filas de la imagen. 'const' porque no cambiará.
    const int cols = img[0].size(); // Obtiene el número de columnas.

    // El bucle principal se ejecuta mientras 'cambio' sea verdadero, es decir, hasta que la imagen converge y no se borran más píxeles.
    while (cambio) {
        bool cambioPaso1 = false; // Bandera para detectar si hubo cambios en el Paso 1.
        bool cambioPaso2 = false; // Bandera para detectar si hubo cambios en el Paso 2.
        vector<pair<int, int>> pixelesParaBorrar; // Vector para almacenar las coordenadas (fila, col) de los píxeles a borrar.

        // --- PASO 1 del algoritmo de Zhang-Suen ---
        // Se recorren todos los píxeles INTERIORES de la imagen (se omiten los bordes).
        for (int i = 1; i < filas - 1; ++i) {
            for (int j = 1; j < cols - 1; ++j) {
                if (img[i][j] == 1) { // Solo se consideran los píxeles que son parte de la forma.
                    int a = contarTransiciones(img, i, j); // A(p1): Número de transiciones 0->1.
                    int b = contarVecinos(img, i, j);      // B(p1): Número de vecinos con valor 1.
                    
                    // Se aplican las 4 condiciones del Paso 1 de Zhang-Suen:
                    if (a == 1 &&                           // 1. El número de transiciones es 1.
                        (b >= 2 && b <= 6) &&                // 2. El número de vecinos está entre 2 y 6.
                        (img[i - 1][j] * img[i][j + 1] * img[i + 1][j] == 0) && // 3. P2*P4*P6 = 0 (al menos un vecino N, E, S es 0).
                        (img[i][j + 1] * img[i + 1][j] * img[i][j - 1] == 0)) { // 4. P4*P6*P8 = 0 (al menos un vecino E, S, O es 0).
                        pixelesParaBorrar.push_back({i, j}); // Si se cumplen todas, se marca el píxel para borrarlo.
                    }
                }
            }
        }
        // Después de revisar toda la imagen, se procesan los píxeles marcados.
        if (!pixelesParaBorrar.empty()) { // Se comprueba si hay píxeles para borrar.
            // Se itera sobre la lista de píxeles a borrar.
            for (const auto& p : pixelesParaBorrar) { img[p.first][p.second] = 0; }
            cambioPaso1 = true; // Se indica que hubo un cambio en este paso.
            pixelesParaBorrar.clear(); // Se limpia la lista para el siguiente paso.
        }

        // --- PASO 2 del algoritmo de Zhang-Suen ---
        // Se recorren nuevamente todos los píxeles interiores.
        for (int i = 1; i < filas - 1; ++i) {
            for (int j = 1; j < cols - 1; ++j) {
                if (img[i][j] == 1) { // Solo se consideran los píxeles de la forma.
                    int a = contarTransiciones(img, i, j); // A(p1)
                    int b = contarVecinos(img, i, j);      // B(p1)

                    // Se aplican las 4 condiciones del Paso 2, que son ligeramente diferentes a las del Paso 1.
                    if (a == 1 &&                           // 1. El número de transiciones es 1.
                        (b >= 2 && b <= 6) &&                // 2. El número de vecinos está entre 2 y 6.
                        (img[i - 1][j] * img[i][j + 1] * img[i][j - 1] == 0) && // 3. P2*P4*P8 = 0 (al menos un vecino N, E, O es 0).
                        (img[i - 1][j] * img[i + 1][j] * img[i][j - 1] == 0)) { // 4. P2*P6*P8 = 0 (al menos un vecino N, S, O es 0).
                        pixelesParaBorrar.push_back({i, j}); // Se marca el píxel para borrarlo.
                    }
                }
            }
        }
        // Se borran los píxeles marcados en el Paso 2.
        if (!pixelesParaBorrar.empty()) { // Se comprueba si hay píxeles para borrar.
            for (const auto& p : pixelesParaBorrar) { img[p.first][p.second] = 0; }
            cambioPaso2 = true; // Se indica que hubo un cambio en este paso.
        }

        // Se actualiza la bandera principal. Si hubo cambios en el Paso 1 O en el Paso 2, el bucle debe continuar.
        cambio = cambioPaso1 || cambioPaso2;
    }
}

// --- Poda Voraz (Greedy Pruning) ---

// Función para realizar una poda iterativa y segura del esqueleto.
// Elimina píxeles finales (ramas de ruido) hasta que la imagen deja de cambiar.
void podaVoraz(Matrix& img) {
    bool cambio = true; // Bandera de control del bucle.
    const int filas = img.size(); // Obtiene dimensiones.
    const int cols = img[0].size();

    // El bucle se ejecuta mientras se sigan podando píxeles en una pasada completa.
    while (cambio) {
        cambio = false; // Se resetea la bandera al inicio de cada iteración.
        vector<pair<int, int>> pixelesParaBorrar; // Buffer para almacenar los píxeles a podar.

        // Fase 1: Recolectar todos los puntos finales.
        // Se recorren los píxeles interiores.
        for (int i = 1; i < filas - 1; ++i) {
            for (int j = 1; j < cols - 1; ++j) {
                // Un píxel es un punto final si es parte de la forma (valor 1) y tiene 1 o 0 vecinos.
                if (img[i][j] == 1 && contarVecinos(img, i, j) <= 1) {
                    pixelesParaBorrar.push_back({i, j}); // Se añade a la lista de poda.
                }
            }
        }

        // Fase 2: Borrar los píxeles recolectados.
        if (!pixelesParaBorrar.empty()) { // Si se encontró al menos un píxel para podar...
            // Se itera sobre la lista y se elimina cada píxel (se pone a 0).
            for (const auto& p : pixelesParaBorrar) {
                img[p.first][p.second] = 0;
            }
            cambio = true; // Se activa la bandera para indicar que la imagen cambió, forzando otra iteración.
        }
    }
}

int main() {
    // Define el tamaño (lado) de la matriz cuadrada que se usará para las pruebas.
    int size = 20;
    
    // Crea un vector de strings para almacenar los títulos de cada caso de prueba.
    vector<string> nombres = {"Caso 1: H Normal", "Caso 2: H con Ruido", "Caso 3: Cuadrado Relleno"};
    
    // Inicia un bucle 'for' que se ejecutará 3 veces, una para cada caso de prueba.
    for(int k = 0; k < 3; ++k) {
        // En cada iteración, crea una nueva matriz 'img' del tamaño especificado, inicializada con ceros.
        Matrix img(size, vector<int>(size, 0));
        
        // Usa una estructura 'switch' para llamar a la función de creación de imagen correspondiente.
        switch(k) {
            case 0: crearHNormal(img); break;      // Para k=0, dibuja una H.
            case 1: crearHConRuido(img); break;    // Para k=1, dibuja una H con ruido.
            case 2: crearCuadradoRelleno(img); break; // Para k=2, dibuja un cuadrado.
        }

        // Llama a la función para imprimir el estado inicial de la imagen.
        imprimirMatriz(img, nombres[k] + " (Original)");
        
        // Aplica el algoritmo de adelgazamiento de Zhang-Suen a la imagen.
        adelgazar(img);
        // Imprime la imagen después del adelgazamiento (el esqueleto).
        imprimirMatriz(img, nombres[k] + " (Esqueletizada)");
        
        // Aplica el algoritmo de poda para limpiar el esqueleto de ruido y ramas cortas.
        podaVoraz(img);
        // Imprime la imagen final después de la poda.
        imprimirMatriz(img, nombres[k] + " (Final con Poda)");
        cout << "\n------------------------------------------" << endl; // Imprime un separador.
    }

    return 0; // El programa finaliza exitosamente.
}

/*
--- Análisis de Complejidad ---
Temporal: O((K + P) * N * M), donde:
 - N: es el número de filas de la imagen.
 - M: es el número de columnas de la imagen.
 - K: es el número de iteraciones del bucle 'while' en 'adelgazar'. K es proporcional al grosor máximo de la forma. En cada iteración, se recorre la matriz completa (N*M).
 - P: es el número de iteraciones en 'podaVoraz'. P es proporcional a la longitud de la rama más larga a podar. En cada iteración, también se recorre la matriz (N*M).

Espacial: O(N * M), debido a:
 - La matriz 'img' que almacena la imagen, ocupando N * M espacio.
 - El vector 'pixelesParaBorrar'. En el peor de los casos (una forma de tablero de ajedrez), podría almacenar hasta la mitad de los píxeles, por lo que su espacio también es proporcional a N * M.
*/