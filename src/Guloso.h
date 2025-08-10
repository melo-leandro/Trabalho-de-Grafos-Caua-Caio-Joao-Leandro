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

        static pair<vector<char>, float> algoritmo_guloso_adaptativo(Grafo &grafo);
        static pair<vector<char>, float> guloso_randomizado(Grafo &grafo, float alpha, int n_iteracoes);
        static pair<vector<char>, float> guloso_randomizado_reativo(Grafo &grafo, float alpha[], int n_iteracoes, int bloco);
        
        // Função simplificada para gerar tabela consolidada de todas as instâncias
        static void executar_experimentos_consolidado(const string& diretorio_instancias, const string& arquivo_saida);
        
        // Função para gerar tabela com diferenças percentuais em relação ao best
        static void executar_experimentos_percentuais(const string& diretorio_instancias, const string& arquivo_saida);
        
        // Experimentos focados na qualidade da melhor solução (peso total)
        static void experimentos_melhor_solucao(Grafo &grafo, const string& nome_instancia, const string& arquivo_saida);

    private:
        static pair<vector<char>, float> construir_solucao_grasp(Grafo &grafo, float alpha);
};

#endif // GULOSO_H