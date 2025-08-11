#include "Gerenciador.h"
#include "Guloso.h"

using namespace std;

int main(int argc, char *argv[]) {
    srand(time(nullptr));
    
    // Gera tabela com valores absolutos
    Guloso::executar_experimentos_qualidades("src/instancias_t2", "src/output/tabela_qualidade.txt");
    
    // Gera tabela com diferenças percentuais
    Guloso::executar_experimentos_percentuais("src/instancias_t2", "src/output/tabela_percentual.txt");
    
    // Gera tabela com tempos de execução
    Guloso::executar_experimentos_tempos("src/instancias_t2", "src/output/tabela_tempos.txt");
    
    // Gera tabela com médias e diferenças percentuais
    Guloso::executar_experimentos_medias("src/instancias_t2", "src/output/tabela_medias.txt");
    
    return 0;
}
