#include "Gerenciador.h"
#include "Guloso.h"

using namespace std;

void executar_experimentos(string diretorio_instancias, string diretorio_saida) {
    // Gera tabela com tempos de execução
    Guloso::executar_experimentos_tempos(diretorio_instancias, diretorio_saida+"/tabela_tempos.txt");

    // Gera tabela com diferenças percentuais
    Guloso::executar_experimentos_melhores(diretorio_instancias, diretorio_saida+"/tabela_melhores.txt");
    
    // Gera tabela com médias e diferenças percentuais
    Guloso::executar_experimentos_medias(diretorio_instancias, diretorio_saida+"/tabela_medias.txt");
}


int main(int argc, char *argv[]) {
    srand(time(nullptr));
    
    if(argc == 1) {
        executar_experimentos("src/input/", "src/output/");
        return 0;
    }

    Grafo* grafo = Gerenciador::carregar_informacoes_entrada(argv[1]);
    Gerenciador::comandos(grafo);
    
    delete grafo;
    
    return 0;
}
