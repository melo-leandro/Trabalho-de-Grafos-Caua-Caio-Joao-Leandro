#include "Gerenciador.h"
#include "Guloso.h"

using namespace std;

int main(int argc, char *argv[]) {
    srand(time(nullptr));
    
    Grafo* grafo = Gerenciador::carregar_informacoes_entrada(argv[1]);
    Guloso::executar_experimentos(*grafo, "instancia", "relatorio.txt");

    // Gerenciador::comandos(grafo);
    
    delete grafo;
    return 0;
}
