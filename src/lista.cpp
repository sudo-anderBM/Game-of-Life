#include "lista.h"

//******************************************************************************
//    Inserta un nuevo nodo al final de la lista
//******************************************************************************

template<class T>
bool LinkedList<T>::insertaFinal(T datos)
{
    // Crea el nuevo nodo con los datos recibidos
    // (CrearNode viene de Node.h, usa new internamente)
    Node<T>* nuevo = CrearNode<T>(datos);

    if (head == nullptr)
    {
        // Caso A: la lista esta vacia
        // el nuevo nodo es a la vez head y tail
        head = nuevo;
        tail = nuevo;
    }
    else
    {
        // Caso B: ya hay nodos, conectamos al final
        // 1. el nuevo nodo mira hacia atras, al tail actual
        nuevo->prev = tail;

        // 2. el tail actual mira hacia adelante, al nuevo nodo
        tail->next = nuevo;

        // 3. hasta el final, actualizamos tail para que sea el nuevo nodo
        //    (si haces esto antes del paso 1 o 2, pierdes la referencia
        //     al tail viejo y la lista se corrompe)
        tail = nuevo;
    }

    return false;   // false = no hubo error, insercion exitosa
}


//******************************************************************************
//    Elimina todos los nodos de la lista y libera su memoria
//******************************************************************************

template<class T>
void LinkedList<T>::eliminarLista()
{
    // mientras existan nodos en la lista
    while (head != nullptr)
    {
        // guarda temporalmente el nodo actual (el que vamos a borrar)
        Node<T>* dummy = head;

        // antes de borrar, avanzamos head al siguiente nodo
        head = head->next;

        // libera la memoria del nodo guardado en dummy
        // (EliminaNode viene de Node.h, hace delete y pone el puntero en nullptr)
        EliminaNode(dummy);
    }

    // la lista quedo vacia, tail tambien debe quedar en nullptr
    tail = nullptr;
}