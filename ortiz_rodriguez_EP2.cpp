// ---------------------------------------------------------
// Proyecto: Esqueletizacion de Imagenes y Poda Voraz
// Autor: Gabriel Ortiz Sepulveda | Fecha: 24/03/2026
// Materia: Analisis y diseno de algoritmos (ADA)
// Referencias: 
// - https://cppreference.com/w/cpp/io.html
// ---------------------------------------------------------

#include <iostream> // para imprimir y leer datos en la consola permite usar 'cout' y 'cin'.
#include <vector>   // para crear arreglos dinamicos necesario para manejar nuestra matriz 2D.


using namespace std;

// Alias para matriz dinamica
using Matrix = vector<vector<int>>;

// --- Utilidades ---

// Imprime la matriz: '#' para pixeles activos, '.' para fondo
void imprimirMatriz(const Matrix& img, string titulo) { // Imprime la matriz: '#' para pixeles activos, '.' para fondo
    cout << "\n" << titulo << ":" << endl;
    for (const auto& fila : img) {
        for (int pixel : fila) {
            cout << (pixel == 1 ? " # " : " . ");
        }
        cout << endl;
    }
}

// --- Casos de Prueba ---

void crearHNormal(Matrix& img) {
    for (int i = 5; i <= 14; ++i) { // Dibuja las dos lineas verticales de la 'H'.
        img[i][5] = 1; img[i][6] = 1;   // Linea izq
        img[i][13] = 1; img[i][14] = 1; // Cada linea tiene 2 pixeles de grosor.
    }
    for (int j = 7; j <= 12; ++j) { // Dibuja la barra horizontal de la 'H', tambien de 2 pixeles de grosor.
        img[9][j] = 1; img[10][j] = 1;  // Barra central
    }
}

void crearHConRuido(Matrix& img) {
    crearHNormal(img);            // Primero, crea la 'H' normal.
    img[7][4] = 1; img[7][3] = 1; // Anade una pequena rama de ruido de 2 pixeles en el lado izquierdo.
    img[12][9] = 1;               // Anade un pixel de ruido aislado (isla) dentro de la 'H'.
}

void crearCuadradoRelleno(Matrix& img) {
    for (int i = 5; i <= 14; ++i) {
        for (int j = 5; j <= 14; ++j) img[i][j] = 1;
    }
}

// --- Algoritmo de Zhang-Suen ---

// Suma de los 8 vecinos conectividad 8 // Funcion para contar los vecinos activos valor 1 de un pixel en r, c usando conectividad de 8.
int contarVecinos(const Matrix& m, int r, int c) {
    return m[r-1][c] + m[r-1][c+1] + m[r][c+1] + m[r+1][c+1] + // Devuelve la suma de los valores de los 8 pixeles circundantes.
           m[r+1][c] + m[r+1][c-1] + m[r][c-1] + m[r-1][c-1];
}

// Cuenta patrones 0->1 en orden horario
int contarTransiciones(const Matrix& m, int r, int c) { // Funcion para contar las transiciones de 0 a 1 en el vecindario de un pixel.
    int p[] = { m[r-1][c], m[r-1][c+1], m[r][c+1], m[r+1][c+1],
                m[r+1][c], m[r+1][c-1], m[r][c-1], m[r-1][c-1], m[r-1][c] };
    int count = 0;
    for (int i = 0; i < 8; ++i) {
        if (p[i] == 0 && p[i+1] == 1) count++;
    }
    return count;
}

void adelgazar(Matrix& img) {
    bool cambio = true;
    const int filas = img.size(); // Bandera de control del bucle. Se mantiene en 'true' mientras la imagen siga cambiando.
    const int cols = img[0].size(); // Obtiene el numero de filas de la imagen. 'const' porque no cambiara.

    while (cambio) {
        bool cambioPaso1 = false, cambioPaso2 = false; // El bucle principal se ejecuta mientras 'cambio' sea verdadero, es decir, hasta que la imagen converge y no se borran mas pixeles.
        vector<pair<int, int>> borrar; // Bandera para detectar si hubo cambios en el Paso 1.

        // Paso 1: Eliminar bordes Sur-Este y esquinas Nor-Oeste // Vector para almacenar las coordenadas fila, col de los pixeles a borrar.
        for (int i = 1; i < filas - 1; ++i) {
            for (int j = 1; j < cols - 1; ++j) {
                if (img[i][j] == 1) {
                    int a = contarTransiciones(img, i, j);
                    int b = contarVecinos(img, i, j);
                    if (a == 1 && (b >= 2 && b <= 6) && 
                        (img[i-1][j] * img[i][j+1] * img[i+1][j] == 0) && 
                        (img[i][j+1] * img[i+1][j] * img[i][j-1] == 0)) 
                        borrar.push_back({i, j}); // Si se cumplen todas, se marca el pixel para borrarlo.
                }
            }
        }
        for (auto& p : borrar) { img[p.first][p.second] = 0; cambioPaso1 = true; } // Despues de revisar toda la imagen, se procesan los pixeles marcados.
        borrar.clear(); // Se comprueba si hay pixeles para borrar.

        // Paso 2: Eliminar bordes Nor-Oeste y esquinas Sur-Este // Se limpia la lista para el siguiente paso.
        for (int i = 1; i < filas - 1; ++i) {
            for (int j = 1; j < cols - 1; ++j) {
                if (img[i][j] == 1) {
                    int a = contarTransiciones(img, i, j);
                    int b = contarVecinos(img, i, j);
                    if (a == 1 && (b >= 2 && b <= 6) && 
                        (img[i-1][j] * img[i][j+1] * img[i][j-1] == 0) && 
                        (img[i-1][j] * img[i+1][j] * img[i][j-1] == 0)) 
                        borrar.push_back({i, j}); // Se marca el pixel para borrarlo.
                }
            }
        }
        for (auto& p : borrar) { img[p.first][p.second] = 0; cambioPaso2 = true; } // Se borran los pixeles marcados en el Paso 2.
        
        cambio = cambioPaso1 || cambioPaso2; // Se actualiza la bandera principal. Si hubo cambios en el Paso 1 O en el Paso 2, el bucle debe continuar.
    }
}

// --- Poda Voraz (Greedy Pruning) ---

// Elimina recursivamente pixeles con 1 o 0 vecinos puntos finales // Funcion para realizar una poda iterativa y segura del esqueleto.
void podaVoraz(Matrix& img) {
    bool cambio = true;
    while (cambio) {
        cambio = false;
        vector<pair<int, int>> borrar;
        for (int i = 1; i < (int)img.size() - 1; ++i) {
            for (int j = 1; j < (int)img[0].size() - 1; ++j) {
                if (img[i][j] == 1 && contarVecinos(img, i, j) <= 1) 
                    borrar.push_back({i, j}); // Se anade a la lista de poda.
            }
        }
        if (!borrar.empty()) { // Si se encontro al menos un pixel para podar...
            for (auto& p : borrar) img[p.first][p.second] = 0; // Se itera sobre la lista y se elimina cada pixel (se pone a 0).
            cambio = true; // Se activa la bandera para indicar que la imagen cambio, forzando otra iteracion.
        }
    } // El bucle se ejecuta mientras se sigan podando pixeles en una pasada completa.
}

int main() {
    int size = 20;
    vector<string> nombres = {"Caso 1: H Normal", "Caso 2: H con Ruido", "Caso 3: Cuadrado Relleno"};
    
    for(int k = 0; k < 3; ++k) {
        Matrix img(size, vector<int>(size, 0));
        if(k == 0) crearHNormal(img);
        else if(k == 1) crearHConRuido(img);
        else crearCuadradoRelleno(img);

        imprimirMatriz(img, nombres[k] + " (Original)");
        
        adelgazar(img);
        imprimirMatriz(img, nombres[k] + " (Esqueleto)");
        
        podaVoraz(img);
        imprimirMatriz(img, nombres[k] + " (Final)");
        cout << "\n------------------------------------------" << endl;
    }
    return 0;
}

/*
--- Analisis de Complejidad ---
Temporal: O(K + P * N * M), donde: // Analisis de Complejidad ---
 - N: es el numero de filas de la imagen Filas.
 - M: es el numero de columnas de la imagen Columnas.
 - K: es el numero de iteraciones en 'adelgazar'. K es proporcional al grosor de la forma, ya que en cada pasada se elimina una capa de pixeles. Dentro de cada iteracion, se recorre la matriz costo N*M.
 - P: es el numero de iteraciones en 'podaVoraz'. P es proporcional a la longitud de la rama mas larga a podar. Tambien recorre la matriz en cada pasada costo N*M.
 - La complejidad total suma ambos procesos: O K*N*M + O P*N*M.

Espacial: O N * M, debido a:
 - La matriz 'img' que almacena la imagen es el componente que mas memoria consume N * M.
 - El vector 'borrar' usa espacio adicional, pero en el peor caso es tambien O N * M, por lo que no cambia la complejidad general.
*/

// Yo Gabriel Ortiz Sepulveda he realizado este examen apego al codigo udem 
// Yo Diego Rodriguez Quintana he realizado este examen apego al codigo udem 