//
// Created by Rafael on 28/05/2025.
//

#ifndef GRAFO_H
#define GRAFO_H

#include "No.h"
#include <iostream>
#include <vector>
#include <map>


using namespace std;
class Grafo {
    public:
        Grafo();
        ~Grafo();

        vector<char> fecho_transitivo_direto(char id_no); // a
        vector<char> fecho_transitivo_indireto(char id_no); // b
        vector<char> caminho_minimo_dijkstra(char id_no_a, char id_no_b); // c
        vector<char> caminho_minimo_floyd(char id_no, char id_no_b); // d
        Grafo* arvore_geradora_minima_prim(vector<char> ids_nos); // e
        Grafo* arvore_geradora_minima_kruskal(vector<char> ids_nos); // f
        Grafo* arvore_caminhamento_profundidade(char id_no); // g
        int raio(); // h 1
        int diametro(); // h 2
        vector<char> centro(); // h 3
        vector<char> periferia(); // h 4
        
        //Getters e Setters
        bool is_direcionado();
        void set_direcionado(bool dir);
        
        bool is_ponderado_vertice();
        void set_ponderado_vertice(bool p_vertice);

        bool is_ponderado_aresta();
        void set_ponderado_aresta(bool p_aresta);        

        int get_ordem();
        void set_ordem(int ordem);

        vector<No*> get_lista_adj();
        
        void adicionar_no(No* no);
        
        //Funções auxiliares
        No* encontrar_no_por_id(char id);
        pair<vector<char>, int> auxiliar_dijkstra(char id_no_a, char id_no_b);
        int distancia(char id_no_a, char id_no_b);
        int excentricidade(char id_no);       
        
        private:
        bool in_direcionado;
        bool in_ponderado_vertice;
        bool in_ponderado_aresta;
        int ordem;
        vector<No*> lista_adj;
        void dfs_util(char atual, map<char, bool>& visitado, Grafo* arvore, map<char, No*>& mapa_novos_nos);
};
#endif //GRAFO_H
