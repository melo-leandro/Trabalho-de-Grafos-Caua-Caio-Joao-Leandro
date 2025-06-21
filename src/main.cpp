#include <iostream>
#include "Gerenciador.h"
using namespace std;

int main(int argc, char *argv[])
{
    
    Grafo* grafo = Gerenciador::carregarInformacoesEntrada(argv[1]);

    Gerenciador::comandos(grafo);
    
    delete grafo;
    return 0;
}
