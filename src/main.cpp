#include <iostream>
#include "Gerenciador.h"
using namespace std;

void imprimir_grafo(Grafo* grafo) {
    for(No* no: grafo->get_lista_adj()) {
        cout << no->get_id() << "("<<  no->get_peso() << ") -> ";
        
        for(Aresta* aresta: no->get_arestas()){
            cout << "{"<< aresta->get_peso() << "} ";
        
            cout << aresta->get_id_destino() << "(" << grafo->encontrar_no_por_id(aresta->get_id_destino())->get_peso() << ") ";
        
        }
        
        cout << endl;
    }
}

int main(int argc, char *argv[])
{
    cout << "ENTROU MAIN" << endl;
    Grafo* grafo = Gerenciador::carregar_informacoes_entrada(argv[1]);
    cout << "GRAFO CARREGADO" << endl;
    imprimir_grafo(grafo);
    Gerenciador::comandos(grafo);
    
    delete grafo;
    return 0;
}
