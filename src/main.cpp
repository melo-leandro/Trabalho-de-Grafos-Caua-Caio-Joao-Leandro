#include <iostream>
#include "Gerenciador.h"
using namespace std;

int main(int argc, char *argv[])
{
    
    Grafo* grafo = Gerenciador::carregar_informacoes_entrada(argv[1]);

    //SEÇÃO DO CÓDIGO PARA TESTE -- REMOVER NA VERSÃO FINAL
    for(No* no: grafo->get_lista_adj()) {
        cout << no->get_id() << "("<<  no->get_peso() << ") -> ";
        
        for(Aresta* aresta: no->get_arestas()){
            cout << "{"<< aresta->get_peso() << "} ";
            if(no->get_id() == aresta->get_id_origem())
                cout << aresta->get_id_destino() << "(" << grafo->getNodeById(aresta->get_id_destino())->get_peso() << ") ";
            else
                cout << aresta->get_id_origem() << "(" << grafo->getNodeById(aresta->get_id_origem())->get_peso() << ") ";
        }
        
        cout << endl;
    }
    //FIM SEÇÃO DO CÓDIGO PARA TESTE -- REMOVER NA VERSÃO FINAL


    // Gerenciador::comandos(grafo);
    
    delete grafo;
    return 0;
}
