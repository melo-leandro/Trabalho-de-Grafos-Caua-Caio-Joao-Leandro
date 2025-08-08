#include "Gerenciador.h"

using namespace std;

int main(int argc, char *argv[]) {
    srand(time(nullptr));
    
    Grafo* grafo = Gerenciador::carregar_informacoes_entrada(argv[1]);

    Gerenciador::comandos(grafo);
    
    delete grafo;
    return 0;
}
