#ifndef GULOSO_H
#define GULOSO_H

#include "Grafo.h"
#include <iostream>

class Guloso {
    public:
        // Algoritmo Guloso Tradicional
        void gulosoTradicional(Grafo &grafo);
        // Algoritmo Guloso Randomizado
        void gulosoRandomizado(Grafo &grafo, float alpha, int n_iteracoes);
        // Algoritmo Guloso Randomizado Adaptativo
        void gulosoRandomizadoAdaptativo(Grafo &grafo, float alpha[], int n_iteracoes, int bloco);

    private:
};

#endif // GULOSO_H