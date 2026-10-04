//******************************************************************************
/*
  Node.h
  Encabezados del Node

  \author Ricardo Legarda Sáenz.
  \date
*/
//******************************************************************************


#ifndef _NODE_HPP
#define _NODE_HPP

template<class T>
struct Node
{
    T data;
    Node *next, *prev;

    Node()
    {
        next = nullptr;
        prev = nullptr;
    }

    Node(T d)
    {
        data = d;
        next = nullptr;
        prev = nullptr;
    }
};

template<class T>
Node<T>* CrearNode(void)
{
    Node<T>* ptr = new Node<T>;
    return ptr;
}

template<class T>
Node<T>* CrearNode(T datos)
{
    Node<T>* ptr = new Node<T>(datos);
    return ptr;
}

template<class T>
void EliminaNode(Node<T>*& ptr)
{
    delete ptr;
    ptr = nullptr;
}

#endif