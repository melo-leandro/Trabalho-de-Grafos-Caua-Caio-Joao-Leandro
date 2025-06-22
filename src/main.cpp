#include <iostream>
#include "Gerenciador.h"
using namespace std;

int main(int argc, char *argv[])
{
    
    Grafo* grafo = Gerenciador::carregarInformacoesEntrada(argv[1]);

    //SEÇÃO DO CÓDIGO PARA TESTE -- REMOVER NA VERSÃO FINAL
    for(No* no: grafo->lista_adj) {
        cout << no->id << "("<<  no->peso << ") -> ";
        
        for(Aresta* aresta: no->arestas){
            cout << "{"<< aresta->peso << "} ";
            if(no->id == aresta->id_origem)
                cout << aresta->id_destino << "(" << grafo->getNodeById(aresta->id_destino)->peso << ") ";
            else
                cout << aresta->id_origem << "(" << grafo->getNodeById(aresta->id_origem)->peso << ") ";
        }
        
        cout << endl;
    }
    //FIM SEÇÃO DO CÓDIGO PARA TESTE -- REMOVER NA VERSÃO FINAL


    Gerenciador::comandos(grafo);
    
    delete grafo;
    return 0;
}
