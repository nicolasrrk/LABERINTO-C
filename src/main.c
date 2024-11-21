#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define ANCHO_PANTALLA 1280
#define ALTO_PANTALLA 960

#define MAX_NIVELES 5
#define MAX_ENEMIGOS 5

int filas = 29;
int columnas = 39;
int nivelActual = 1;

int tamañoCelda;
int desplazamientoLaberintoX;
int desplazamientoLaberintoY;

Vector2 posJugador;
Vector2 posMeta;
Vector2 enemigos[MAX_ENEMIGOS];

int laberinto[29][39];

Color verdeOscuroGameboy = (Color){15, 56, 15, 255};
Color verdeGameboy = (Color){48, 98, 48, 255};
Color verdeClaroGameboy = (Color){139, 172, 15, 255};
Color verdeBrillanteGameboy = (Color){155, 188, 15, 255};
Color blancoGameboy = (Color){255, 255, 255, 255};
Color colorNegro = (Color){0, 0, 0, 255};

int direccionesX[4] = {1, -1, 0, 0};
int direccionesY[4] = {0, 0, 1, -1};

float temporizadorMovimientoEnemigo = 0.0f;
float intervaloMovimientoEnemigo = 0.3f;

Texture2D texturaEnemigo;
Texture2D texturaJugador;
int tamañoEnemigo;

void CargarRecursos()
{
    texturaEnemigo = LoadTexture("C:/proyect/raylib-VSCode-Template-main/src/enemigosypj/enemy2.png");
    texturaJugador = LoadTexture("C:/proyect/raylib-VSCode-Template-main/src/enemigosypj/pj.png");
}

void DescargarRecursos()
{
    UnloadTexture(texturaEnemigo);
    UnloadTexture(texturaJugador);
}

void MezclarDirecciones(int *dx, int *dy)
{
    for (int i = 0; i < 4; i++)
    {
        int j = rand() % 4;
        int tempX = dx[i];
        int tempY = dy[i];
        dx[i] = dx[j];
        dy[i] = dy[j];
        dx[j] = tempX;
        dy[j] = tempY;
    }
}

void GenerarLaberintoDFS(int x, int y)
{
    laberinto[y][x] = 0;
    MezclarDirecciones(direccionesX, direccionesY);

    for (int i = 0; i < 4; i++)
    {
        int nuevoX = x + direccionesX[i] * 2;
        int nuevoY = y + direccionesY[i] * 2;

        if (nuevoX > 0 && nuevoX < columnas - 1 && nuevoY > 0 && nuevoY < filas - 1 && laberinto[nuevoY][nuevoX] == 1)
        {
            laberinto[y + direccionesY[i]][x + direccionesX[i]] = 0;
            GenerarLaberintoDFS(nuevoX, nuevoY);
        }
    }
}

void GenerarLaberintoPerfecto()
{
    for (int y = 0; y < filas; y++)
    {
        for (int x = 0; x < columnas; x++)
        {
            laberinto[y][x] = 1;
        }
    }
    GenerarLaberintoDFS(1, 1);

    laberinto[1][1] = 0;
    laberinto[filas - 2][columnas - 2] = 0;

    posJugador = (Vector2){1, 1};
    posMeta = (Vector2){columnas - 2, filas - 2};

    for (int i = 0; i < MAX_ENEMIGOS; i++)
    {
        int enemigoX, enemigoY;
        do
        {
            enemigoX = rand() % columnas;
            enemigoY = rand() % filas;
        } while (laberinto[enemigoY][enemigoX] != 0 || (enemigoX == 1 && enemigoY == 1) || (enemigoX == columnas - 2 && enemigoY == filas - 2));
        enemigos[i] = (Vector2){enemigoX, enemigoY};
    }
}

void ReiniciarJuego()
{
    nivelActual = 1;
    GenerarLaberintoPerfecto();
}

void DibujarLaberinto()
{
    for (int y = 0; y < filas; y++)
    {
        for (int x = 0; x < columnas; x++)
        {
            if (laberinto[y][x] == 1)
            {
                DrawRectangle(desplazamientoLaberintoX + x * tamañoCelda, desplazamientoLaberintoY + y * tamañoCelda, tamañoCelda, tamañoCelda, verdeGameboy);
            }
            else
            {
                DrawRectangle(desplazamientoLaberintoX + x * tamañoCelda, desplazamientoLaberintoY + y * tamañoCelda, tamañoCelda, tamañoCelda, colorNegro);
            }
        }
    }
}

void MoverEnemigos()
{
    temporizadorMovimientoEnemigo += GetFrameTime();
    if (temporizadorMovimientoEnemigo >= intervaloMovimientoEnemigo)
    {
        for (int i = 0; i < MAX_ENEMIGOS; i++)
        {
            int nuevaDireccion = rand() % 4;
            int nuevoX = enemigos[i].x + direccionesX[nuevaDireccion];
            int nuevoY = enemigos[i].y + direccionesY[nuevaDireccion];

            if (nuevoX > 0 && nuevoX < columnas && nuevoY > 0 && nuevoY < filas && laberinto[nuevoY][nuevoX] == 0)
            {
                enemigos[i] = (Vector2){nuevoX, nuevoY};
            }

            if (enemigos[i].x == posJugador.x && enemigos[i].y == posJugador.y)
            {
                ReiniciarJuego();
                return;
            }
        }
        temporizadorMovimientoEnemigo = 0.0f;
    }
}

int main(void)
{
    tamañoCelda = (ANCHO_PANTALLA / columnas < ALTO_PANTALLA / filas) ? ANCHO_PANTALLA / columnas : ALTO_PANTALLA / filas;
    desplazamientoLaberintoX = (ANCHO_PANTALLA - (tamañoCelda * columnas)) / 2;
    desplazamientoLaberintoY = (ALTO_PANTALLA - (tamañoCelda * filas)) / 2;

    tamañoEnemigo = tamañoCelda * 0.8;

    InitWindow(ANCHO_PANTALLA, ALTO_PANTALLA, "LABERINTO DEL DESAFÍO");
    SetTargetFPS(60);

    CargarRecursos();
    srand(time(NULL));
    GenerarLaberintoPerfecto();

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_RIGHT) && laberinto[(int)posJugador.y][(int)posJugador.x + 1] == 0)
            posJugador.x++;
        if (IsKeyPressed(KEY_LEFT) && laberinto[(int)posJugador.y][(int)posJugador.x - 1] == 0)
            posJugador.x--;
        if (IsKeyPressed(KEY_UP) && laberinto[(int)posJugador.y - 1][(int)posJugador.x] == 0)
            posJugador.y--;
        if (IsKeyPressed(KEY_DOWN) && laberinto[(int)posJugador.y + 1][(int)posJugador.x] == 0)
            posJugador.y++;

        BeginDrawing();
        ClearBackground(colorNegro);

        DibujarLaberinto();

        DrawTexturePro(texturaJugador,
                       (Rectangle){0, 0, (float)texturaJugador.width, (float)texturaJugador.height},
                       (Rectangle){desplazamientoLaberintoX + posJugador.x * tamañoCelda, desplazamientoLaberintoY + posJugador.y * tamañoCelda, (float)tamañoCelda, (float)tamañoCelda},
                       (Vector2){0.0f, 0.0f}, 0.0f, blancoGameboy);

        DrawRectangle(desplazamientoLaberintoX + posMeta.x * tamañoCelda, desplazamientoLaberintoY + posMeta.y * tamañoCelda, tamañoCelda, tamañoCelda, blancoGameboy);

        for (int i = 0; i < MAX_ENEMIGOS; i++)
        {
            DrawTexturePro(texturaEnemigo,
                           (Rectangle){0, 0, (float)texturaEnemigo.width, (float)texturaEnemigo.height},
                           (Rectangle){desplazamientoLaberintoX + enemigos[i].x * tamañoCelda + (tamañoCelda - tamañoEnemigo) / 2,
                                       desplazamientoLaberintoY + enemigos[i].y * tamañoCelda + (tamañoCelda - tamañoEnemigo) / 2,
                                       (float)tamañoEnemigo, (float)tamañoEnemigo},
                           (Vector2){0.0f, 0.0f}, 0.0f, blancoGameboy);
        }

        MoverEnemigos();

        if (posJugador.x == posMeta.x && posJugador.y == posMeta.y)
        {
            if (nivelActual < MAX_NIVELES)
            {
                DrawText("¡NIVEL COMPLETADO!!!", ANCHO_PANTALLA / 2 - 100, ALTO_PANTALLA / 2, 30, verdeBrillanteGameboy);
                DrawText("Presiona [Espacio] para continuar!!!", ANCHO_PANTALLA / 2 - 150, ALTO_PANTALLA / 2 + 50, 20, blancoGameboy);

                if (IsKeyPressed(KEY_SPACE))
                {
                    nivelActual++;
                    GenerarLaberintoPerfecto();
                }
            }
            else
            {
                DrawText("¡FELICIDADES, GANASTE!", ANCHO_PANTALLA / 4, ALTO_PANTALLA / 2, 40, verdeBrillanteGameboy);
            }
        }

        EndDrawing();
    }

    DescargarRecursos();
    CloseWindow();

    return 0;
}
