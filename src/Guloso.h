#ifndef GULOSO_H
#define GULOSO_H

#include "Grafo.h"
#include <iostream>
#include <queue>

using namespace std;

class Guloso {
    public:
        Guloso();
        ~Guloso();

        //TRUE -> 'b' tem maior prioridade (vai para o topo)
        //FALSE -> 'a' tem maior prioridade (vai para o topo)
        struct NoComparator {
            bool operator()(No* a, No* b) const {
                return a->get_peso() > b->get_peso(); // min-heap por peso
            }
        };

        static vector<char> algoritmo_guloso(Grafo &grafo);
        static vector<char> algoritmo_guloso_adaptativo(Grafo &grafo);
        static vector<char> guloso_randomizado(Grafo &grafo, float alpha, int n_iteracoes);
        static vector<char> gulosoRandomizadoReativo(Grafo &grafo, float alpha[], int n_iteracoes, int bloco);

    private:
};

#endif // GULOSO_H