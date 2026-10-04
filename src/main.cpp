//******************************************************************************
/*
  main.cpp
  Conway's Game of Life usando raylib.
  Cada generacion del tablero se guarda en un nodo de una lista
  doblemente ligada. Permite navegar manualmente entre generaciones,
  y tambien reproducir automaticamente con pausa y control de velocidad.
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
    int poblacion;               // cuantas celdas vivas tiene esta generacion
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
    SetTargetFPS(60);   // 60 fps para que los controles se sientan fluidos
                          // (la velocidad de la simulacion ya no depende de esto)

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

        // copia el tablero actual dentro del nodo, y de paso cuenta
        // cuantas celdas estan vivas en esta generacion
        int vivasContadas = 0;
        for (int k = 0; k < ROWS * COLS; k++)
        {
            estado.celdas[k] = estadoActual[k];
            if (estadoActual[k])
            {
                vivasContadas++;
            }
        }
        estado.poblacion = vivasContadas;

        if (lista.insertaFinal(estado))
        {
            std::cout << "Error al insertar la generacion " << (i + 1) << std::endl;
        }

        // calcula la siguiente generacion para la proxima vuelta del ciclo
        gameLifeConway(estadoActual, dummy, ROWS, COLS);
    }

    // ------------------------------------------------------------
    // PANTALLA DE INSTRUCCIONES (antes de iniciar la animacion)
    // ------------------------------------------------------------

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            break;   // sale de la pantalla de instrucciones y arranca la simulacion
        }

        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawText("CONWAY'S GAME OF LIFE", 190, 180, 30, BLACK);
            DrawText("Lista doblemente ligada - 100 generaciones precalculadas", 130, 230, 16, DARKGRAY);

            DrawText("Controles:", 190, 290, 20, BLACK);
            DrawText("Flecha derecha / izquierda  ->  avanzar / retroceder generacion", 190, 320, 16, DARKGRAY);
            DrawText("ESPACIO                      ->  play / pausa (automatico)", 190, 342, 16, DARKGRAY);
            DrawText("+ / -                        ->  mas rapido / mas lento", 190, 364, 16, DARKGRAY);
            DrawText("ESC                          ->  salir", 190, 386, 16, DARKGRAY);

            DrawText("Presiona ENTER para comenzar", 220, 440, 20, MAROON);

        EndDrawing();
    }

    // ------------------------------------------------------------
    // VARIABLES PARA LA SIMULACION PRINCIPAL
    // ------------------------------------------------------------

    // Puntero que indica en que generacion estamos parados
    Node<EstadoTablero>* actual = lista.Inicio();

    // Bandera para saber si el usuario ya uso la navegacion manual
    bool modoManual = false;

    // Bandera de reproduccion automatica (arranca en pausa)
    bool pausado = true;

    // Controla que tan rapido avanza solo (en segundos por generacion)
    // mas chico = mas rapido
    float velocidad = 0.3f;
    const float velocidadMin = 0.05f;   // limite: lo mas rapido que puede ir
    const float velocidadMax = 1.5f;    // limite: lo mas lento que puede ir

    // Acumula el tiempo transcurrido entre frames, para saber cuando
    // ya toca avanzar a la siguiente generacion automaticamente
    float tiempoAcumulado = 0.0f;

    // ------------------------------------------------------------
    // LOOP PRINCIPAL
    // ------------------------------------------------------------

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

        // Espacio: pausa o reanuda la reproduccion automatica
        if (IsKeyPressed(KEY_SPACE))
        {
            pausado = !pausado;
        }

        // Tecla + (o el signo =, que comparte tecla en muchos teclados):
        // aumenta la velocidad (reduce el tiempo de espera entre generaciones)
        if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD))
        {
            velocidad -= 0.05f;
            if (velocidad < velocidadMin)
            {
                velocidad = velocidadMin;
            }
        }

        // Tecla -: disminuye la velocidad
        if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT))
        {
            velocidad += 0.05f;
            if (velocidad > velocidadMax)
            {
                velocidad = velocidadMax;
            }
        }

        // Si no esta pausado, avanza automaticamente segun la velocidad
        if (!pausado)
        {
            tiempoAcumulado += GetFrameTime();

            if (tiempoAcumulado >= velocidad)
            {
                tiempoAcumulado = 0.0f;

                if (actual->next != nullptr)
                {
                    actual = actual->next;
                }
                else
                {
                    pausado = true;   // llego al final, se pausa solo
                }
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
            DrawRectangle(0, 0, 340, 160, Fade(BLACK, 0.80f));

            // Texto con el numero de generacion actual (de 100)
            DrawText(TextFormat("Generacion: %d / 100", actual->data.numeroEstado),
                      10, 10, 20, YELLOW);

            // Contador de poblacion (celdas vivas en esta generacion)
            DrawText(TextFormat("Poblacion: %d celdas vivas", actual->data.poblacion),
                      10, 36, 16, GREEN);

            // Estado de reproduccion (play / pausa) y velocidad actual
            if (pausado)
            {
                DrawText("PAUSADO", 10, 58, 16, (Color){255, 120, 120, 255});
            }
            else
            {
                DrawText(TextFormat("REPRODUCIENDO (%.2fs/gen)", velocidad), 10, 58, 16, LIME);
            }

            // Instrucciones de uso, siempre visibles
            DrawText("Flechas: navegar manual  |  ESPACIO: play/pausa", 10, 84, 14, RAYWHITE);
            DrawText("+ / - : velocidad  |  ESC: salir", 10, 100, 14, RAYWHITE);

            // Indicador de modo manual (aparece solo despues de usar las flechas)
            if (modoManual)
            {
                DrawText("Modo manual usado", 10, 124, 14, (Color){255, 200, 120, 255});
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