# Conway's Game of Life

Proyecto realizado para la materia de **Estructuras de Datos**.

Implementación en C++ del **Game of Life de Conway**, utilizando una lista doblemente ligada como estructura principal y `raylib` para la parte gráfica.

## ¿Qué hace?

El programa simula generaciones del Game of Life siguiendo las reglas de Conway:

* Una célula viva con menos de 2 vecinos muere.
* Una célula viva con 2 o 3 vecinos sobrevive.
* Una célula viva con más de 3 vecinos muere.
* Una célula muerta con exactamente 3 vecinos revive.

## Estructura

```text
Game of life/
├── Makefile
├── conway.cpp
└── src/
    ├── Node.h
    ├── lista.h
    ├── lista.cpp
    └── main.cpp
```

La lista doblemente ligada se implementó desde cero para trabajar con los elementos del juego.

## Tecnologías

* C++
* raylib
* Make
* Git

## Compilar

Con `raylib` instalada:

```bash
make
```

Después ejecutar el programa generado.

## Estado

Proyecto en desarrollo. La idea es seguir agregando funcionalidades y mejorar la simulación.
