#ifndef GERENCIADOR_H
#define GERENCIADOR_H

#include <iostream>
#include "Grafo.h"
#include <algorithm>

using namespace std;
class Gerenciador {
public:
    static void comandos(Grafo* grafo);
    static char get_id_entrada();
    static vector<char> get_conjunto_ids(Grafo* grafo, int tam);
    static bool pergunta_imprimir_arquivo(string nome_arquivo);
    
    // Função para carregar as informações do grafo a partir de um .txt
    static Grafo* carregar_informacoes_entrada(string nome_arquivo);

    // Função para imprimir lista de adjacência em tela
    static void imprimir_grafo(Grafo* grafo);
    
    // Funções para imprimir em arquivo
    static void gerar_arquivo_saida_grafo(Grafo* grafo, string nome_arquivo, bool imprimir_estrutura_grafo);
    static void gerar_arquivo_saida_vetor(vector<char> vetor, string nome_arquivo);
};


#endif //GERENCIADOR_H
