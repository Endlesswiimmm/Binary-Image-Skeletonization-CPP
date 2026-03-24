// ---------------------------------------------------------
// Título: Esqueletización de Imágenes y Poda Voraz
// Autores: Gabriel Ortiz Sepulveda
// Fecha de elaboración: 24/03/2026
// Materia: Análisis y diseño de algoritmos (ADA)
// Fuente: CPP Reference (std::vector, std::iostream)
// ---------------------------------------------------------

// --- Inclusión de Librerías ---
#include <iostream>
#include <vector>
#include <iomanip>

// --- Espacio de Nombres ---
// Usamos el espacio de nombres estándar para evitar escribir 'std::' repetidamente.
using namespace std;

// --- Definición de Tipos ---
// Se crea un alias llamado 'Matrix' para un vector de vectores de enteros.
// Esto hace que el código sea más legible al representar una matriz o imagen 2D.
typedef vector<vector<int>> Matrix;

// --- Funciones de Utilidad ---

// Función para imprimir una matriz en la consola.
// Recibe una referencia constante a la matriz para evitar copias innecesarias.
void imprimirMatriz(const Matrix& img, string titulo) {
    cout << "\n" << titulo << ":" << endl;
    // Itera sobre cada fila de la matriz.
    for (const auto& fila : img) {
        // Itera sobre cada píxel (entero) en la fila actual.
        for (int pixel : fila) {
            // Imprime un carácter '#' si el píxel es 1 (parte de la forma)
            // o '.' si es 0 (fondo).
            cout << (pixel == 1 ? " # " : " . ");
        }
        cout << endl;
    }
}

// --- Funciones para Crear Casos de Prueba ---

void crearHNormal(Matrix& img) {
    for (int i = 5; i <= 14; ++i) {
        img[i][5] = 1; img[i][6] = 1;
        img[i][13] = 1; img[i][14] = 1;
    }
    for (int j = 7; j <= 12; ++j) {
        img[9][j] = 1; img[10][j] = 1;
    }
}

void crearHConRuido(Matrix& img) {
    crearHNormal(img);
    img[7][4] = 1; img[7][3] = 1; // Ruido a la izquierda
    img[12][9] = 1;               // Ruido tipo isla
}

void crearCuadradoRelleno(Matrix& img) {
    for (int i = 5; i <= 14; ++i) {
        for (int j = 5; j <= 14; ++j) img[i][j] = 1;
    }
}

// --- Algoritmo de Zhang-Suen ---

// Función para contar los vecinos de un píxel (conectividad de 8).
// Un vecino es cualquier píxel adyacente (horizontal, vertical o diagonal) que tiene valor 1.
int contarVecinos(const Matrix& m, int r, int c) {
    // Suma los valores de los 8 píxeles que rodean al píxel en la posición (r, c).
    // Como los píxeles son 0 o 1, la suma da directamente el número de vecinos con valor 1.
    return m[r-1][c] + m[r-1][c+1] + m[r][c+1] + m[r+1][c+1] +
           m[r+1][c] + m[r+1][c-1] + m[r][c-1] + m[r-1][c-1];
}

// Función para contar las transiciones de 0 a 1 en el vecindario de un píxel.
int contarTransiciones(const Matrix& m, int r, int c) {
    // Se crea un array 'p' con los 8 vecinos en orden de las manecillas del reloj, comenzando desde el de arriba (P2). Se repite el primer vecino al final para cerrar el ciclo.
    int p[] = { m[r-1][c], m[r-1][c+1], m[r][c+1], m[r+1][c+1],
                m[r+1][c], m[r+1][c-1], m[r][c-1], m[r-1][c-1], m[r-1][c] };
    int count = 0;
    // Se recorren los vecinos y se cuenta cuántas veces un píxel de fondo (0) es seguido por un píxel de la forma (1).
    for (int i = 0; i < 8; ++i) {
        if (p[i] == 0 && p[i+1] == 1) count++;
    }
    return count;
}

void adelgazar(Matrix& img) {
    bool cambio = true; // Bandera para controlar si hubo cambios en la iteración. El bucle se detiene cuando no hay más cambios.
    int filas = img.size();
    int cols = img[0].size();

    // El algoritmo itera hasta que la imagen ya no cambia, es decir, hasta que se ha adelgazado por completo.
    while (cambio) {
        cambio = false;
        // Vector para almacenar las coordenadas de los píxeles que se deben borrar en cada paso.
        // No se borran directamente para no afectar el cálculo de los vecinos de otros píxeles en la misma iteración.
        vector<pair<int, int>> borrar;

        // --- PASO 1 del algoritmo de Zhang-Suen ---
        // Itera sobre los píxeles interiores de la imagen.
        for (int i = 1; i < filas - 1; ++i) {
            for (int j = 1; j < cols - 1; ++j) {
                if (img[i][j] == 0) continue; // Si el píxel es fondo, se ignora.
                
                int a = contarTransiciones(img, i, j); // A(p1): Número de transiciones 0->1.
                int b = contarVecinos(img, i, j);      // B(p1): Número de vecinos con valor 1.

                // Condiciones para marcar un píxel para su eliminación en el paso 1:
                // 1. A(p1) == 1: El píxel es un punto final o de borde, no un punto interior que conecte múltiples segmentos.
                // 2. 2 <= B(p1) <= 6: No es un punto aislado (B(p1)>=2) y no es parte de un bloque 2x2 que podría causar una desconexión (B(p1)<=6).
                // 3. P2 * P4 * P6 == 0: Al menos uno de los vecinos Norte, Este o Sur es 0.
                // 4. P4 * P6 * P8 == 0: Al menos uno de los vecinos Este, Sur o Oeste es 0.
                if (a == 1 && (b >= 2 && b <= 6) &&
                    (img[i-1][j] * img[i][j+1] * img[i+1][j] == 0) &&
                    (img[i][j+1] * img[i+1][j] * img[i][j-1] == 0)) {
                    borrar.push_back({i, j}); // Se añade el píxel a la lista de borrado.
                }
            }
        }
        // Borra todos los píxeles marcados en este paso.
        for (auto p : borrar) { img[p.first][p.second] = 0; cambio = true; } // Si se borró al menos uno, 'cambio' es verdadero.
        borrar.clear(); // Limpia la lista para el siguiente paso.

        // --- PASO 2 del algoritmo de Zhang-Suen ---
        // Similar al paso 1, pero con condiciones ligeramente diferentes para borrar píxeles de otros bordes.
        for (int i = 1; i < filas - 1; ++i) {
            for (int j = 1; j < cols - 1; ++j) {
                if (img[i][j] == 0) continue;
                int a = contarTransiciones(img, i, j);
                int b = contarVecinos(img, i, j);

                // Condiciones para marcar un píxel para su eliminación en el paso 2:
                // 1 y 2 son iguales al paso 1.
                // 3. P2 * P4 * P8 == 0: Al menos uno de los vecinos Norte, Este u Oeste es 0.
                // 4. P2 * P6 * P8 == 0: Al menos uno de los vecinos Norte, Sur u Oeste es 0.
                // Estas condiciones son complementarias a las del paso 1 para asegurar que se adelgace de manera simétrica.
                if (a == 1 && (b >= 2 && b <= 6) &&
                    (img[i-1][j] * img[i][j+1] * img[i][j-1] == 0) &&
                    (img[i-1][j] * img[i+1][j] * img[i][j-1] == 0)) {
                    borrar.push_back({i, j});
                }
            }
        }
        // Borra todos los píxeles marcados en este segundo paso.
        for (auto p : borrar) { img[p.first][p.second] = 0; cambio = true; }
    }
}

// --- Poda Voraz (Greedy Pruning) ---

// Función para realizar una poda simple del esqueleto.
void podaVoraz(Matrix& img) {
    int filas = img.size();
    int cols = img[0].size();
    // Criterio voraz: Eliminar píxeles que son puntos finales.
    // Un punto final en un esqueleto es un píxel que tiene 1 o 0 vecinos.
    for (int i = 1; i < filas - 1; ++i) {
        for (int j = 1; j < cols - 1; ++j) {
            if (img[i][j] == 1) {
                // Si un píxel de la forma tiene 1 o 0 vecinos, se considera un punto final (posiblemente ruido o una rama corta).
                if (contarVecinos(img, i, j) <= 1) { 
                    img[i][j] = 0; // Se elimina el píxel.
                }
            }
        }
    }
}

int main() {
    // Define el tamaño de la matriz/imagen de prueba.
    int size = 20;
    
    // Vector de strings para los títulos de los casos de prueba.
    vector<string> nombres = {"Caso 1: H Normal", "Caso 2: H con Ruido", "Caso 3: Cuadrado Relleno"};
    
    // Bucle para ejecutar los 3 casos de prueba.
    for(int k = 0; k < 3; ++k) {
        // Crea una matriz vacía (llena de ceros) para cada caso.
        Matrix img(size, vector<int>(size, 0));
        
        // Selecciona la función para dibujar la forma inicial según el caso.
        if(k == 0) crearHNormal(img);
        else if(k == 1) crearHConRuido(img);
        else crearCuadradoRelleno(img);

        // Imprime la imagen original.
        imprimirMatriz(img, nombres[k] + " (Original)");
        
        // Aplica el algoritmo de adelgazamiento y muestra el resultado.
        adelgazar(img);
        imprimirMatriz(img, nombres[k] + " (Esqueletizada)");
        
        // Aplica la poda voraz para limpiar el esqueleto y muestra el resultado final.
        podaVoraz(img);
        imprimirMatriz(img, nombres[k] + " (Final con Poda)");
        cout << "\n------------------------------------------" << endl;
    }

    return 0;
}

/*
--- Análisis de Complejidad ---
Temporal: O(K * N * M), donde:
 - N es el número de filas de la imagen.
 - M es el número de columnas de la imagen.
 - K es el número de iteraciones del bucle 'while' en 'adelgazar'. K depende del grosor máximo de la forma en la imagen. En cada iteración, se recorre toda la matriz.

Espacial: O(N * M), debido a:
 - La matriz 'img' que almacena la imagen.
 - El vector 'borrar' que, en el peor de los casos, podría almacenar una fracción significativa de los píxeles de la imagen antes de ser borrados.
*/