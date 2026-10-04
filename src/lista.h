/*
  lista.h
  Declaracion de la clase LinkedList (lista doblemente ligada)
  Usa el Node<T> definido en Node.h (con punteros next y prev)
*/

#ifndef _LISTA_HPP   // include guard: evita que este archivo se procese dos veces
#define _LISTA_HPP

#include "Node.h"     // trae la definicion de Node<T> (data, next, prev)


//******************************************************************************
//    Clase generica para manejar una lista doblemente ligada
//    T es el tipo de dato que guardara cada nodo (definido por quien la use)
//******************************************************************************

template<class T>
class LinkedList
{
    private:
        Node<T>* head;   // puntero al primer nodo de la lista
        Node<T>* tail;   // puntero al ultimo nodo de la lista

    public:
        // Constructor: al crear la lista, no hay nodos todavia
        LinkedList() { head = nullptr; tail = nullptr; }

        // Getters
        Node<T>* Inicio() const { return head; }
        Node<T>* Final() const { return tail; }

        // Inserta un nuevo nodo al final de la lista
        bool insertaFinal(T datos);

        // Elimina todos los nodos de la lista y libera su memoria
        void eliminarLista();
};

#include "lista.cpp"   // AL FINAL, despues de cerrar la clase (necesario por ser template)

#endif