//******************************************************************************
/*
  main.cpp
  Conway's Game of Life usando raylib.
  Cada generacion del tablero se guarda en un nodo de una lista
  doblemente ligada. Permite navegar manualmente entre generaciones,
  reproduccion automatica con pausa y control de velocidad, musica
  de fondo en loop, y animaciones de apertura y cierre.

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
#include <cmath>
#include <algorithm>


//******************************************************************************
//    Constantes del tablero (grid) del Game of Life
//******************************************************************************

const int CELL_SIZE = 10;
const int ROWS = 600 / CELL_SIZE;
const int COLS = 800 / CELL_SIZE;
const int HEADER_HEIGHT = 64;
const int FOOTER_HEIGHT = 22;


//******************************************************************************
//    Paleta de colores — estilo sobrio, papel academico
//******************************************************************************

const Color COLOR_FONDO       = (Color){ 250, 250, 247, 255 };
const Color COLOR_CELDA       = (Color){  32,  32,  38, 255 };
const Color COLOR_GRID        = (Color){ 228, 228, 222, 255 };
const Color COLOR_TEXTO       = (Color){  40,  40,  45, 255 };
const Color COLOR_TEXTO_SUAVE = (Color){ 140, 140, 134, 255 };
const Color COLOR_ACENTO      = (Color){ 120,  24,  34, 255 };
const Color COLOR_LINEA       = (Color){ 210, 210, 204, 255 };


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

    InitWindow(anchoVentana, altoVentana, "Conway's Game of Life - Lista Doblemente Ligada");
    SetTargetFPS(60);

    // Desactiva el cierre automatico al presionar ESC: lo controlamos
    // nosotros para poder reproducir la animacion de cierre antes de salir
    SetExitKey(KEY_NULL);

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

    // Puntero de navegacion — declarado antes de la portada para que
    // tenga datos validos incluso si el usuario sale desde ahi
    Node<EstadoTablero>* actual = lista.Inicio();

    // Bandera compartida: se vuelve true cuando el usuario pide salir (ESC)
    bool salir = false;

    // ------------------------------------------------------------
    // PORTADA — con animacion de entrada en cascada
    // ------------------------------------------------------------

    const int PUNTO_SEP = 28;
    float inicioPortada = (float)GetTime();

    while (!salir && !WindowShouldClose())
    {
        UpdateMusicStream(musica);

        if (IsKeyPressed(KEY_ENTER))
        {
            break;
        }

        if (IsKeyPressed(KEY_ESCAPE))
        {
            salir = true;
        }

        float t = (float)GetTime();
        float tPortada = t - inicioPortada;   // segundos desde que arranco la portada

        BeginDrawing();

            ClearBackground(COLOR_FONDO);

            // ---------- fondo animado: campo de puntos tipo onda ----------
            for (int px = 0; px < anchoVentana; px += PUNTO_SEP)
            {
                for (int py = 0; py < altoVentana; py += PUNTO_SEP)
                {
                    float fase = sinf(px * 0.04f + py * 0.04f + t * 1.2f);
                    float intensidad = (fase + 1.0f) / 2.0f;

                    float radio = 1.0f + intensidad * 1.6f;
                    unsigned char alpha = (unsigned char)(18 + intensidad * 28);

                    DrawCircle(px, py, radio, Fade(COLOR_ACENTO, alpha / 255.0f));
                }
            }

            // ---------- titulo: fade + deslizamiento hacia arriba ----------
            float alphaTitulo = std::clamp(tPortada / 0.7f, 0.0f, 1.0f);
            float offsetTitulo = (1.0f - alphaTitulo) * 18.0f;   // desliza 18px mientras aparece

            int tituloAncho = MeasureText("CONWAY'S GAME OF LIFE", 28);
            DrawText("CONWAY'S GAME OF LIFE",
                      (anchoVentana - tituloAncho) / 2, (int)(150 + offsetTitulo), 28,
                      Fade(COLOR_TEXTO, alphaTitulo));

            // ---------- subtitulo: aparece un poco despues ----------
            float alphaSub = std::clamp((tPortada - 0.2f) / 0.7f, 0.0f, 1.0f);
            float offsetSub = (1.0f - alphaSub) * 14.0f;

            int subAncho = MeasureText("Simulacion con lista doblemente ligada  -  C++ / raylib", 14);
            DrawText("Simulacion con lista doblemente ligada  -  C++ / raylib",
                      (anchoVentana - subAncho) / 2, (int)(188 + offsetSub), 14,
                      Fade(COLOR_TEXTO_SUAVE, alphaSub));

            // linea separadora, aparece con el subtitulo
            DrawLine(anchoVentana / 2 - 80, 220, anchoVentana / 2 + 80, 220,
                      Fade(COLOR_LINEA, alphaSub));

            // ---------- controles: aparecen en cascada, linea por linea ----------
            int xControles = anchoVentana / 2 - 170;
            int yControles = 250;
            int paso = 24;

            const char* lineasControl[6] = {
                "CONTROLES",
                "Flecha derecha / izquierda   navegar generacion",
                "Espacio                       reproducir / pausar",
                "+  /  -                        velocidad",
                "M                              musica",
                "Esc                            salir"
            };

            for (int i = 0; i < 6; i++)
            {
                float retraso = 0.5f + i * 0.08f;
                float alphaLinea = std::clamp((tPortada - retraso) / 0.3f, 0.0f, 1.0f);
                float offsetLinea = (1.0f - alphaLinea) * 10.0f;

                Color colorLinea = (i == 0) ? COLOR_ACENTO : COLOR_TEXTO;
                int tam = (i == 0) ? 13 : 14;

                DrawText(lineasControl[i], xControles, (int)(yControles + paso * i + offsetLinea), tam,
                          Fade(colorLinea, alphaLinea));
            }

            // ---------- "ENTER para comenzar": aparece al final, con parpadeo ----------
            if (tPortada > 1.1f)
            {
                float alphaEnter = std::clamp((tPortada - 1.1f) / 0.4f, 0.0f, 1.0f);
                float parpadeo = (sinf(t * 3.0f) + 1.0f) / 2.0f;
                float alphaFinal = alphaEnter * (0.55f + parpadeo * 0.45f);

                int enterAncho = MeasureText("ENTER para comenzar", 16);
                DrawText("ENTER para comenzar", (anchoVentana - enterAncho) / 2, 420, 16,
                           Fade(COLOR_ACENTO, alphaFinal));
            }

        EndDrawing();
    }

    // ------------------------------------------------------------
    // SIMULACION PRINCIPAL
    // ------------------------------------------------------------

    bool pausado = true;

    float velocidad = 0.3f;
    const float velocidadMin = 0.05f;
    const float velocidadMax = 1.5f;

    float tiempoAcumulado = 0.0f;

    while (!salir && !WindowShouldClose())
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

        if (IsKeyPressed(KEY_ESCAPE))
        {
            salir = true;
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

            DrawRectangle(0, 0, anchoVentana, HEADER_HEIGHT, COLOR_FONDO);
            DrawLine(0, HEADER_HEIGHT, anchoVentana, HEADER_HEIGHT, COLOR_LINEA);

            DrawText("GENERACION", 16, 10, 11, COLOR_TEXTO_SUAVE);
            DrawText(TextFormat("%03d / 100", actual->data.numeroEstado), 16, 26, 22, COLOR_TEXTO);

            DrawLine(150, 12, 150, 52, COLOR_LINEA);

            DrawText("POBLACION", 166, 10, 11, COLOR_TEXTO_SUAVE);
            DrawText(TextFormat("N = %d", actual->data.poblacion), 166, 26, 20, COLOR_ACENTO);

            DrawLine(300, 12, 300, 52, COLOR_LINEA);

            DrawText("ESTADO", 316, 10, 11, COLOR_TEXTO_SUAVE);
            const char* textoEstado = pausado ? "PAUSADO" : "REPRODUCIENDO";
            DrawText(textoEstado, 316, 26, 18, COLOR_TEXTO);
            DrawCircle(316 + MeasureText(textoEstado, 18) + 14, 35, 5,
                        pausado ? COLOR_TEXTO_SUAVE : COLOR_ACENTO);

            DrawLine(500, 12, 500, 52, COLOR_LINEA);

            DrawText("VELOCIDAD", 516, 10, 11, COLOR_TEXTO_SUAVE);
            if (!pausado)
            {
                DrawText(TextFormat("%.2f s/gen", velocidad), 516, 26, 16, COLOR_TEXTO);
            }
            else
            {
                DrawText("-", 516, 26, 16, COLOR_TEXTO_SUAVE);
            }

            DrawLine(630, 12, 630, 52, COLOR_LINEA);

            DrawText("AUDIO", 646, 10, 11, COLOR_TEXTO_SUAVE);
            DrawText(musicaActiva ? "ON" : "OFF", 646, 26, 16,
                       musicaActiva ? COLOR_TEXTO : COLOR_TEXTO_SUAVE);

            float progreso = (float)actual->data.numeroEstado / 100.0f;
            DrawRectangle(0, HEADER_HEIGHT - 2, anchoVentana, 2, COLOR_LINEA);
            DrawRectangle(0, HEADER_HEIGHT - 2, (int)(anchoVentana * progreso), 2, COLOR_ACENTO);

            DrawRectangle(0, altoVentana - FOOTER_HEIGHT, anchoVentana, FOOTER_HEIGHT, COLOR_FONDO);
            DrawLine(0, altoVentana - FOOTER_HEIGHT, anchoVentana, altoVentana - FOOTER_HEIGHT, COLOR_LINEA);

            const char* pie = "Fig. 1 - Automata celular B3/S23 sobre malla toroidal 80 x 60";
            DrawText(pie, 12, altoVentana - FOOTER_HEIGHT + 5, 12, COLOR_TEXTO_SUAVE);

        EndDrawing();
    }

    // ------------------------------------------------------------
    // CIERRE — fade a oscuro, musica baja de volumen, resumen final
    // Solo se reproduce si el usuario salio con ESC (salir == true).
    // Si cerro con la X de la ventana, WindowShouldClose() ya es true
    // y este bloque se salta solo (cierre inmediato, comportamiento normal).
    // ------------------------------------------------------------

    float tiempoCierre = 0.0f;
    const float duracionCierre = 1.4f;

    while (salir && tiempoCierre < duracionCierre && !WindowShouldClose())
    {
        float dt = GetFrameTime();
        tiempoCierre += dt;

        UpdateMusicStream(musica);
        SetMusicVolume(musica, 1.0f - (tiempoCierre / duracionCierre));

        float t = std::clamp(tiempoCierre / duracionCierre, 0.0f, 1.0f);

        BeginDrawing();

            ClearBackground(COLOR_FONDO);

            // overlay oscuro que va cubriendo la pantalla
            DrawRectangle(0, 0, anchoVentana, altoVentana, Fade(COLOR_TEXTO, t * 0.94f));

            // el texto final aparece despues de que ya oscurecio bastante
            if (t > 0.35f)
            {
                float tTexto = std::clamp((t - 0.35f) / 0.65f, 0.0f, 1.0f);

                const char* msg = "FIN DE LA SIMULACION";
                int anchoMsg = MeasureText(msg, 26);
                DrawText(msg, (anchoVentana - anchoMsg) / 2, altoVentana / 2 - 30, 26,
                          Fade(COLOR_FONDO, tTexto));

                const char* stats = TextFormat("Generacion alcanzada: %03d / 100   -   Poblacion final: N = %d",
                                                  actual->data.numeroEstado, actual->data.poblacion);
                int anchoStats = MeasureText(stats, 14);
                DrawText(stats, (anchoVentana - anchoStats) / 2, altoVentana / 2 + 14, 14,
                          Fade(COLOR_LINEA, tTexto));
            }

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