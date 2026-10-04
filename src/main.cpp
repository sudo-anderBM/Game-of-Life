//******************************************************************************
/*
  main.cpp
  Conway's Game of Life usando raylib.
  Cada generacion del tablero se guarda en un nodo de una lista
  doblemente ligada. Permite navegar manualmente entre generaciones.
*/
//******************************************************************************
#include "raylib.h"
#include "lista.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <random>


//******************************************************************************
//    Constantes del tablero (grid) del Game of Life
//******************************************************************************

const int CELL_SIZE = 10;                    // tamaño en pixeles de cada celda
const int ROWS = 600 / CELL_SIZE;             // filas del tablero (60)
const int COLS = 800 / CELL_SIZE;             // columnas del tablero (80)


//******************************************************************************
//    Estructura que guarda un estado (una generacion) del tablero
//******************************************************************************

struct EstadoTablero
{
    bool celdas[ROWS * COLS];   // snapshot completo de esta generacion (4800 celdas)
    int numeroEstado;           // numero de generacion (1, 2, 3, ...)
};


//******************************************************************************
//    Declaracion de la funcion que aplica las reglas de Conway
//******************************************************************************

void gameLifeConway(bool estado[], bool dummy[], int rows, int cols);


//******************************************************************************
//    Funcion principal
//******************************************************************************

int main(void)
{
    // Configuracion de la ventana
    const int anchoVentana = COLS * CELL_SIZE;
    const int altoVentana  = ROWS * CELL_SIZE;

    InitWindow(anchoVentana, altoVentana, "Conway's Game of Life - Lista Doblemente Ligada");
    SetTargetFPS(15);

    // Configura el motor de numeros pseudoaleatorios
    std::random_device rd;
    std::mt19937 generador(rd());
    std::uniform_real_distribution<float> distribucion(0.0f, 1.0f);

    // Crea la lista doblemente ligada que guardara las 100 generaciones
    LinkedList<EstadoTablero> lista;

    // Tableros de trabajo: uno guarda el estado actual, el otro es auxiliar
    // para calcular la siguiente generacion sin pisar datos a medio calcular
    bool estadoActual[ROWS * COLS];
    bool dummy[ROWS * COLS];

    // Genera el tablero inicial de forma aleatoria (generacion 1)
    for (int k = 0; k < ROWS * COLS; k++)
    {
        estadoActual[k] = distribucion(generador) > 0.75f;
    }

    // Genera 100 generaciones y las va insertando en la lista
    for (int i = 0; i < 100; i++)
    {
        EstadoTablero estado;
        estado.numeroEstado = i + 1;

        // copia el tablero actual dentro del nodo que vamos a guardar
        for (int k = 0; k < ROWS * COLS; k++)
        {
            estado.celdas[k] = estadoActual[k];
        }

        if (lista.insertaFinal(estado))
        {
            std::cout << "Error al insertar la generacion " << (i + 1) << std::endl;
        }

        // calcula la siguiente generacion para la proxima vuelta del ciclo
        gameLifeConway(estadoActual, dummy, ROWS, COLS);
    }

    // Puntero que indica en que generacion estamos parados
    Node<EstadoTablero>* actual = lista.Inicio();

    // Bandera para saber si el usuario ya uso la navegacion manual
    bool modoManual = false;

    // Loop principal: se repite mientras la ventana no se cierre
    while (!WindowShouldClose())
    {
        // ------------------------------------------------------------
        // ACTUALIZACION (logica del programa, antes de dibujar)
        // ------------------------------------------------------------

        // Flecha derecha: avanza a la siguiente generacion (si existe)
        if (IsKeyPressed(KEY_RIGHT))
        {
            modoManual = true;

            if (actual->next != nullptr)
            {
                actual = actual->next;
            }
        }

        // Flecha izquierda: retrocede a la generacion anterior (si existe)
        if (IsKeyPressed(KEY_LEFT))
        {
            modoManual = true;

            if (actual->prev != nullptr)
            {
                actual = actual->prev;
            }
        }

        // ------------------------------------------------------------
        // DIBUJO
        // ------------------------------------------------------------

        BeginDrawing();

            // Fondo blanco
            ClearBackground(RAYWHITE);

            // Dibuja cada celda del tablero guardado en el nodo actual
            for (int r = 0; r < ROWS; ++r)
            {
                for (int c = 0; c < COLS; ++c)
                {
                    if (actual->data.celdas[r * COLS + c])
                    {
                        // celda viva: negro solido (contrasta contra el fondo blanco)
                        DrawRectangle(c * CELL_SIZE, r * CELL_SIZE, CELL_SIZE, CELL_SIZE, BLACK);
                    }

                    // lineas de la cuadricula, discretas sobre fondo blanco
                    DrawRectangleLines(c * CELL_SIZE, r * CELL_SIZE, CELL_SIZE, CELL_SIZE, LIGHTGRAY);
                }
            }

            // Panel oscuro semitransparente detras del texto, para que
            // siempre se lea bien sin importar que celdas haya debajo
            DrawRectangle(0, 0, 340, 120, Fade(BLACK, 0.80f));

            // Texto con el numero de generacion actual (de 100)
            DrawText(TextFormat("Generacion: %d / 100", actual->data.numeroEstado),
                      10, 10, 20, YELLOW);

            // Instrucciones de uso, siempre visibles
            DrawText("Flecha derecha: siguiente generacion", 10, 40, 16, RAYWHITE);
            DrawText("Flecha izquierda: generacion anterior", 10, 58, 16, RAYWHITE);
            DrawText("ESC: salir", 10, 76, 16, RAYWHITE);

            // Indicador de modo (aparece solo despues de usar las flechas)
            if (modoManual)
            {
                DrawText("Modo manual activo", 10, 96, 16, (Color){255, 120, 120, 255});
            }

        EndDrawing();
    }

    // Libera la memoria de la lista antes de cerrar
    lista.eliminarLista();

    // Cierra la ventana y libera recursos de raylib
    CloseWindow();

    return 0;
}


//******************************************************************************
//    Aplica las reglas de Conway's Game of Life sobre el tablero
//    estado: tablero actual (se actualiza al final de la funcion)
//    dummy:  tablero temporal donde se calcula la siguiente generacion
//******************************************************************************

void gameLifeConway(bool estado[], bool dummy[], int rows, int cols)
{
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            // cuenta vecinos vivos de la celda (r,c)
            int vecinos = 0;

            for (int y = -1; y <= 1; ++y)
            {
                for (int x = -1; x <= 1; ++x)
                {
                    // la celda central no cuenta como su propio vecino
                    if (y == 0 && x == 0)
                    {
                        continue;
                    }

                    // bordes periodicos (el tablero se "envuelve" como un toroide)
                    int dx = (c + x + cols) % cols;
                    int dy = (r + y + rows) % rows;

                    if (estado[dy * cols + dx])
                    {
                        vecinos++;
                    }
                }
            }

            // Reglas de Conway:
            // viva con 2 o 3 vecinos -> sigue viva
            // muerta con exactamente 3 vecinos -> nace
            if (estado[r * cols + c])
            {
                dummy[r * cols + c] = (vecinos == 2) || (vecinos == 3);
            }
            else
            {
                dummy[r * cols + c] = (vecinos == 3);
            }
        }
    }

    // copia el resultado calculado de vuelta al tablero real
    for (int k = 0; k < rows * cols; k++)
    {
        estado[k] = dummy[k];
    }
}