// ---------------------------------------------------------
// Proyecto: Esqueletización de Imágenes y Poda Voraz
// Autor: Gabriel Ortiz Sepulveda | Fecha: 24/03/2026
// Materia: Análisis y diseño de algoritmos (ADA)
// ---------------------------------------------------------

#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

// Alias para matriz dinámica
using Matrix = vector<vector<int>>;

// --- Utilidades ---

// Imprime la matriz: '#' para pixeles activos, '.' para fondo
void imprimirMatriz(const Matrix& img, string titulo) {
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
    for (int i = 5; i <= 14; ++i) {
        img[i][5] = 1; img[i][6] = 1;   // Línea izq
        img[i][13] = 1; img[i][14] = 1; // Línea der
    }
    for (int j = 7; j <= 12; ++j) {
        img[9][j] = 1; img[10][j] = 1;  // Barra central
    }
}

void crearHConRuido(Matrix& img) {
    crearHNormal(img);
    img[7][4] = 1; img[7][3] = 1; // Rama de ruido
    img[12][9] = 1;               // Punto aislado
}

void crearCuadradoRelleno(Matrix& img) {
    for (int i = 5; i <= 14; ++i) {
        for (int j = 5; j <= 14; ++j) img[i][j] = 1;
    }
}

// --- Algoritmo de Zhang-Suen ---

// Suma de los 8 vecinos (conectividad 8)
int contarVecinos(const Matrix& m, int r, int c) {
    return m[r-1][c] + m[r-1][c+1] + m[r][c+1] + m[r+1][c+1] +
           m[r+1][c] + m[r+1][c-1] + m[r][c-1] + m[r-1][c-1];
}

// Cuenta patrones 0->1 en orden horario
int contarTransiciones(const Matrix& m, int r, int c) {
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
    const int filas = img.size();
    const int cols = img[0].size();

    while (cambio) {
        bool cambioPaso1 = false, cambioPaso2 = false;
        vector<pair<int, int>> borrar;

        // Paso 1: Eliminar bordes Sur-Este y esquinas Nor-Oeste
        for (int i = 1; i < filas - 1; ++i) {
            for (int j = 1; j < cols - 1; ++j) {
                if (img[i][j] == 1) {
                    int a = contarTransiciones(img, i, j);
                    int b = contarVecinos(img, i, j);
                    if (a == 1 && (b >= 2 && b <= 6) && 
                        (img[i-1][j] * img[i][j+1] * img[i+1][j] == 0) && 
                        (img[i][j+1] * img[i+1][j] * img[i][j-1] == 0)) 
                        borrar.push_back({i, j});
                }
            }
        }
        for (auto& p : borrar) { img[p.first][p.second] = 0; cambioPaso1 = true; }
        borrar.clear();

        // Paso 2: Eliminar bordes Nor-Oeste y esquinas Sur-Este
        for (int i = 1; i < filas - 1; ++i) {
            for (int j = 1; j < cols - 1; ++j) {
                if (img[i][j] == 1) {
                    int a = contarTransiciones(img, i, j);
                    int b = contarVecinos(img, i, j);
                    if (a == 1 && (b >= 2 && b <= 6) && 
                        (img[i-1][j] * img[i][j+1] * img[i][j-1] == 0) && 
                        (img[i-1][j] * img[i+1][j] * img[i][j-1] == 0)) 
                        borrar.push_back({i, j});
                }
            }
        }
        for (auto& p : borrar) { img[p.first][p.second] = 0; cambioPaso2 = true; }
        
        cambio = cambioPaso1 || cambioPaso2;
    }
}

// --- Poda Voraz (Greedy Pruning) ---

// Elimina recursivamente pixeles con 1 o 0 vecinos (puntos finales)
void podaVoraz(Matrix& img) {
    bool cambio = true;
    while (cambio) {
        cambio = false;
        vector<pair<int, int>> borrar;
        for (int i = 1; i < (int)img.size() - 1; ++i) {
            for (int j = 1; j < (int)img[0].size() - 1; ++j) {
                if (img[i][j] == 1 && contarVecinos(img, i, j) <= 1) 
                    borrar.push_back({i, j});
            }
        }
        if (!borrar.empty()) {
            for (auto& p : borrar) img[p.first][p.second] = 0;
            cambio = true;
        }
    }
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

/* COMPLEJIDAD:
Temporal: O((Iteraciones) * Filas * Columnas)
Espacial: O(Filas * Columnas) para almacenar la imagen.
*/