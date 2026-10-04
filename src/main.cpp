//******************************************************************************
/*
  main.cpp
  Conway's Game of Life usando raylib.
  Cada generacion del tablero se guarda en un nodo de una lista
  doblemente ligada. Permite navegar manualmente entre generaciones,
  reproduccion automatica con pausa y control de velocidad, y musica
  de fondo en loop.

  Diseño visual: paleta reducida, tipografia con jerarquia clara,
  estilo sobrio tipo figura academica.
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

const int CELL_SIZE = 10;
const int ROWS = 600 / CELL_SIZE;
const int COLS = 800 / CELL_SIZE;
const int HEADER_HEIGHT = 64;     // franja superior para info
const int FOOTER_HEIGHT = 22;     // franja inferior para el pie de figura


//******************************************************************************
//    Paleta de colores — estilo sobrio, papel academico
//******************************************************************************

const Color COLOR_FONDO       = (Color){ 250, 250, 247, 255 };   // blanco calido, tipo papel
const Color COLOR_CELDA       = (Color){  32,  32,  38, 255 };   // casi negro, azulado
const Color COLOR_GRID        = (Color){ 228, 228, 222, 255 };   // lineas casi invisibles
const Color COLOR_TEXTO       = (Color){  40,  40,  45, 255 };   // texto principal
const Color COLOR_TEXTO_SUAVE = (Color){ 140, 140, 134, 255 };   // texto secundario / etiquetas
const Color COLOR_ACENTO      = (Color){ 120,  24,  34, 255 };   // vino / burdeos, unico color fuerte
const Color COLOR_LINEA       = (Color){ 210, 210, 204, 255 };   // separadores finos


//******************************************************************************
//    Estructura que guarda un estado (una generacion) del tablero
//******************************************************************************

struct EstadoTablero
{
    bool celdas[ROWS * COLS];
    int numeroEstado;
    int poblacion;
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
    const int anchoVentana = COLS * CELL_SIZE;
    const int altoVentana  = ROWS * CELL_SIZE;

    InitWindow(anchoVentana, altoVentana, "Conway's Game of Life — Lista Doblemente Ligada");
    SetTargetFPS(60);

    // ------------------------------------------------------------
    // AUDIO
    // ------------------------------------------------------------

    InitAudioDevice();

    Music musica = LoadMusicStream("assets/musica.ogg");
    musica.looping = true;
    PlayMusicStream(musica);

    bool musicaActiva = true;

    // Motor de numeros pseudoaleatorios
    std::random_device rd;
    std::mt19937 generador(rd());
    std::uniform_real_distribution<float> distribucion(0.0f, 1.0f);

    // Lista doblemente ligada con las 100 generaciones
    LinkedList<EstadoTablero> lista;

    bool estadoActual[ROWS * COLS];
    bool dummy[ROWS * COLS];

    for (int k = 0; k < ROWS * COLS; k++)
    {
        estadoActual[k] = distribucion(generador) > 0.75f;
    }

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
    // PANTALLA DE INSTRUCCIONES — estilo portada sobria
    // ------------------------------------------------------------

    while (!WindowShouldClose())
    {
        UpdateMusicStream(musica);

        if (IsKeyPressed(KEY_ENTER))
        {
            break;
        }

        BeginDrawing();

            ClearBackground(COLOR_FONDO);

            // marco delgado alrededor de toda la ventana
            DrawRectangleLines(0, 0, anchoVentana, altoVentana, COLOR_LINEA);

            // titulo centrado
            int tituloAncho = MeasureText("CONWAY'S GAME OF LIFE", 28);
            DrawText("CONWAY'S GAME OF LIFE", (anchoVentana - tituloAncho) / 2, 150, 28, COLOR_TEXTO);

            int subAncho = MeasureText("Simulacion con lista doblemente ligada  —  C++ / raylib", 14);
            DrawText("Simulacion con lista doblemente ligada  —  C++ / raylib",
                      (anchoVentana - subAncho) / 2, 188, 14, COLOR_TEXTO_SUAVE);

            // linea separadora fina
            DrawLine(anchoVentana / 2 - 80, 220, anchoVentana / 2 + 80, 220, COLOR_LINEA);

            // bloque de controles, alineado en columna
            int xControles = anchoVentana / 2 - 170;
            int yControles = 250;
            int paso = 24;

            DrawText("CONTROLES", xControles, yControles, 13, COLOR_ACENTO);
            DrawText("Flecha derecha / izquierda   navegar generacion",
                      xControles, yControles + paso * 1, 14, COLOR_TEXTO);
            DrawText("Espacio                       reproducir / pausar",
                      xControles, yControles + paso * 2, 14, COLOR_TEXTO);
            DrawText("+  /  -                        velocidad",
                      xControles, yControles + paso * 3, 14, COLOR_TEXTO);
            DrawText("M                              musica",
                      xControles, yControles + paso * 4, 14, COLOR_TEXTO);
            DrawText("Esc                            salir",
                      xControles, yControles + paso * 5, 14, COLOR_TEXTO);

            int enterAncho = MeasureText("ENTER para comenzar", 16);
            DrawText("ENTER para comenzar", (anchoVentana - enterAncho) / 2, 420, 16, COLOR_ACENTO);

        EndDrawing();
    }

    // ------------------------------------------------------------
    // VARIABLES DE LA SIMULACION
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
        UpdateMusicStream(musica);

        // --- entradas ---

        if (IsKeyPressed(KEY_RIGHT) && actual->next != nullptr)
        {
            actual = actual->next;
        }

        if (IsKeyPressed(KEY_LEFT) && actual->prev != nullptr)
        {
            actual = actual->prev;
        }

        if (IsKeyPressed(KEY_SPACE))
        {
            pausado = !pausado;
        }

        if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD))
        {
            velocidad -= 0.05f;
            if (velocidad < velocidadMin) velocidad = velocidadMin;
        }

        if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT))
        {
            velocidad += 0.05f;
            if (velocidad > velocidadMax) velocidad = velocidadMax;
        }

        if (IsKeyPressed(KEY_M))
        {
            musicaActiva = !musicaActiva;
            if (musicaActiva) ResumeMusicStream(musica);
            else PauseMusicStream(musica);
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

            ClearBackground(COLOR_FONDO);

            // ---------- tablero ----------
            for (int r = 0; r < ROWS; ++r)
            {
                for (int c = 0; c < COLS; ++c)
                {
                    int y = HEADER_HEIGHT + r * CELL_SIZE;

                    if (actual->data.celdas[r * COLS + c])
                    {
                        DrawRectangle(c * CELL_SIZE, y, CELL_SIZE, CELL_SIZE, COLOR_CELDA);
                    }

                    DrawRectangleLines(c * CELL_SIZE, y, CELL_SIZE, CELL_SIZE, COLOR_GRID);
                }
            }

            // ---------- encabezado ----------
            DrawRectangle(0, 0, anchoVentana, HEADER_HEIGHT, COLOR_FONDO);
            DrawLine(0, HEADER_HEIGHT, anchoVentana, HEADER_HEIGHT, COLOR_LINEA);

            // columna 1: generacion
            DrawText("GENERACION", 16, 10, 11, COLOR_TEXTO_SUAVE);
            DrawText(TextFormat("%03d / 100", actual->data.numeroEstado), 16, 26, 22, COLOR_TEXTO);

            DrawLine(150, 12, 150, 52, COLOR_LINEA);

            // columna 2: poblacion
            DrawText("POBLACION", 166, 10, 11, COLOR_TEXTO_SUAVE);
            DrawText(TextFormat("N = %d", actual->data.poblacion), 166, 26, 20, COLOR_ACENTO);

            DrawLine(300, 12, 300, 52, COLOR_LINEA);

            // columna 3: estado
            DrawText("ESTADO", 316, 10, 11, COLOR_TEXTO_SUAVE);
            const char* textoEstado = pausado ? "PAUSADO" : "REPRODUCIENDO";
            DrawText(textoEstado, 316, 26, 18, COLOR_TEXTO);
            DrawCircle(316 + MeasureText(textoEstado, 18) + 14, 35, 5,
                        pausado ? COLOR_TEXTO_SUAVE : COLOR_ACENTO);

            DrawLine(500, 12, 500, 52, COLOR_LINEA);

            // columna 4: velocidad
            DrawText("VELOCIDAD", 516, 10, 11, COLOR_TEXTO_SUAVE);
            if (!pausado)
            {
                DrawText(TextFormat("%.2f s/gen", velocidad), 516, 26, 16, COLOR_TEXTO);
            }
            else
            {
                DrawText("—", 516, 26, 16, COLOR_TEXTO_SUAVE);
            }

            DrawLine(630, 12, 630, 52, COLOR_LINEA);

            // columna 5: musica
            DrawText("AUDIO", 646, 10, 11, COLOR_TEXTO_SUAVE);
            DrawText(musicaActiva ? "ON" : "OFF", 646, 26, 16,
                       musicaActiva ? COLOR_TEXTO : COLOR_TEXTO_SUAVE);

            // barra de progreso delgada, al fondo del encabezado
            float progreso = (float)actual->data.numeroEstado / 100.0f;
            DrawRectangle(0, HEADER_HEIGHT - 2, anchoVentana, 2, COLOR_LINEA);
            DrawRectangle(0, HEADER_HEIGHT - 2, (int)(anchoVentana * progreso), 2, COLOR_ACENTO);

            // ---------- pie de figura ----------
            DrawRectangle(0, altoVentana - FOOTER_HEIGHT, anchoVentana, FOOTER_HEIGHT, COLOR_FONDO);
            DrawLine(0, altoVentana - FOOTER_HEIGHT, anchoVentana, altoVentana - FOOTER_HEIGHT, COLOR_LINEA);

            const char* pie = "Fig. 1 — Automata celular B3/S23 sobre malla toroidal 80 x 60";
            DrawText(pie, 12, altoVentana - FOOTER_HEIGHT + 5, 12, COLOR_TEXTO_SUAVE);

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