//******************************************************************************
/*
  main.cpp
  Conway's Game of Life usando raylib.
  Cada generacion del tablero se guarda en un nodo de una lista
  doblemente ligada. Permite navegar manualmente entre generaciones,
  y tambien reproducir automaticamente con pausa y control de velocidad.
  Incluye musica de fondo en loop.
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

    // ------------------------------------------------------------
    // AUDIO
    // ------------------------------------------------------------

    InitAudioDevice();   // prende el sistema de audio, una sola vez

    Music musica = LoadMusicStream("assets/musica.ogg");
    musica.looping = true;   // que se repita sola al terminar
    PlayMusicStream(musica);

    bool musicaActiva = true;   // bandera para poder silenciarla con una tecla

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

        gameLifeConway(estadoActual, dummy, ROWS, COLS);
    }

    // ------------------------------------------------------------
    // PANTALLA DE INSTRUCCIONES (antes de iniciar la animacion)
    // ------------------------------------------------------------

    while (!WindowShouldClose())
    {
        UpdateMusicStream(musica);   // hay que llamarlo cada frame, siempre

        if (IsKeyPressed(KEY_ENTER))
        {
            break;
        }

        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawText("CONWAY'S GAME OF LIFE", 190, 180, 30, BLACK);
            DrawText("Lista doblemente ligada - 100 generaciones precalculadas", 130, 230, 16, DARKGRAY);

            DrawText("Controles:", 190, 290, 20, BLACK);
            DrawText("Flecha derecha / izquierda  ->  avanzar / retroceder generacion", 190, 320, 16, DARKGRAY);
            DrawText("ESPACIO                      ->  play / pausa (automatico)", 190, 342, 16, DARKGRAY);
            DrawText("+ / -                        ->  mas rapido / mas lento", 190, 364, 16, DARKGRAY);
            DrawText("M                            ->  silenciar / activar musica", 190, 386, 16, DARKGRAY);
            DrawText("ESC                          ->  salir", 190, 408, 16, DARKGRAY);

            DrawText("Presiona ENTER para comenzar", 220, 460, 20, MAROON);

        EndDrawing();
    }

    // ------------------------------------------------------------
    // VARIABLES PARA LA SIMULACION PRINCIPAL
    // ------------------------------------------------------------

    Node<EstadoTablero>* actual = lista.Inicio();

    bool pausado = true;

    float velocidad = 0.3f;
    const float velocidadMin = 0.05f;
    const float velocidadMax = 1.5f;

    float tiempoAcumulado = 0.0f;

    // ------------------------------------------------------------
    // LOOP PRINCIPAL
    // ------------------------------------------------------------

    while (!WindowShouldClose())
    {
        // La musica necesita actualizarse cada frame para seguir sonando
        UpdateMusicStream(musica);

        // ------------------------------------------------------------
        // ACTUALIZACION
        // ------------------------------------------------------------

        if (IsKeyPressed(KEY_RIGHT))
        {
            if (actual->next != nullptr)
            {
                actual = actual->next;
            }
        }

        if (IsKeyPressed(KEY_LEFT))
        {
            if (actual->prev != nullptr)
            {
                actual = actual->prev;
            }
        }

        if (IsKeyPressed(KEY_SPACE))
        {
            pausado = !pausado;
        }

        if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD))
        {
            velocidad -= 0.05f;
            if (velocidad < velocidadMin)
            {
                velocidad = velocidadMin;
            }
        }

        if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT))
        {
            velocidad += 0.05f;
            if (velocidad > velocidadMax)
            {
                velocidad = velocidadMax;
            }
        }

        // Tecla M: silencia o reactiva la musica
        if (IsKeyPressed(KEY_M))
        {
            musicaActiva = !musicaActiva;

            if (musicaActiva)
            {
                ResumeMusicStream(musica);
            }
            else
            {
                PauseMusicStream(musica);
            }
        }

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
                    pausado = true;
                }
            }
        }

        // ------------------------------------------------------------
        // DIBUJO
        // ------------------------------------------------------------

        BeginDrawing();

            ClearBackground(RAYWHITE);

            for (int r = 0; r < ROWS; ++r)
            {
                for (int c = 0; c < COLS; ++c)
                {
                    if (actual->data.celdas[r * COLS + c])
                    {
                        DrawRectangle(c * CELL_SIZE, r * CELL_SIZE, CELL_SIZE, CELL_SIZE, BLACK);
                    }

                    DrawRectangleLines(c * CELL_SIZE, r * CELL_SIZE, CELL_SIZE, CELL_SIZE, LIGHTGRAY);
                }
            }

            DrawRectangle(0, 0, anchoVentana, 70, Fade(BLACK, 0.82f));

            DrawText(TextFormat("Gen %d/100", actual->data.numeroEstado),
                      12, 10, 24, RAYWHITE);

            DrawText(TextFormat("Poblacion: %d", actual->data.poblacion),
                      170, 16, 18, (Color){120, 220, 120, 255});

            const char* estadoTexto = pausado ? "|| PAUSADO" : "> REPRODUCIENDO";
            Color colorEstado = pausado ? (Color){255, 140, 140, 255} : (Color){140, 200, 255, 255};
            DrawText(estadoTexto, 360, 16, 18, colorEstado);

            if (!pausado)
            {
                DrawText(TextFormat("(%.2fs/gen)", velocidad), 560, 16, 16, GRAY);
            }

            const char* iconoMusica = musicaActiva ? "[M] musica ON" : "[M] musica OFF";
            DrawText(iconoMusica, 660, 16, 14, (Color){200, 200, 255, 255});

            DrawText("FLECHAS navegar   ESPACIO play/pausa   +/- velocidad   ESC salir",
                      12, 44, 14, (Color){170, 170, 170, 255});

        EndDrawing();
    }

    // ------------------------------------------------------------
    // LIMPIEZA
    // ------------------------------------------------------------

    lista.eliminarLista();

    UnloadMusicStream(musica);
    CloseAudioDevice();

    CloseWindow();

    return 0;
}


//******************************************************************************
//    Aplica las reglas de Conway's Game of Life sobre el tablero
//******************************************************************************

void gameLifeConway(bool estado[], bool dummy[], int rows, int cols)
{
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            int vecinos = 0;

            for (int y = -1; y <= 1; ++y)
            {
                for (int x = -1; x <= 1; ++x)
                {
                    if (y == 0 && x == 0)
                    {
                        continue;
                    }

                    int dx = (c + x + cols) % cols;
                    int dy = (r + y + rows) % rows;

                    if (estado[dy * cols + dx])
                    {
                        vecinos++;
                    }
                }
            }

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

    for (int k = 0; k < rows * cols; k++)
    {
        estado[k] = dummy[k];
    }
}