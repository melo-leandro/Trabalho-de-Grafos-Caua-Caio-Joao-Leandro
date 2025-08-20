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

        static pair<vector<char>, float> algoritmo_guloso_adaptativo(Grafo &grafo);
        static pair<vector<char>, float> guloso_randomizado(Grafo &grafo, float alpha, int n_iteracoes);
        static pair<vector<char>, float> guloso_randomizado_reativo(Grafo &grafo, float alpha[], int n_iteracoes, int bloco);
        
        // Função para gerar tabela com diferenças percentuais em relação ao best
        static void executar_experimentos_melhores(const string& diretorio_instancias, const string& arquivo_saida);
        
        // Função para gerar tabela com tempos de execução
        static void executar_experimentos_tempos(const string& diretorio_instancias, const string& arquivo_saida);
        
        // Função para gerar tabela com médias e diferenças percentuais em relação ao best
        static void executar_experimentos_medias(const string& diretorio_instancias, const string& arquivo_saida);

    private:
        static pair<vector<char>, float> construir_solucao_grasp(Grafo &grafo, float alpha);
};

#endif // GULOSO_H