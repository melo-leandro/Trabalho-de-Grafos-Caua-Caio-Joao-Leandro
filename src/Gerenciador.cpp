#include "Gerenciador.h"
#include "No.h"
#include <fstream>
#include <sstream>


void Gerenciador::comandos(Grafo* grafo) {
    cout<<"Digite uma das opcoes abaixo e pressione enter:"<<endl<<endl;
    cout<<"(a) Fecho transitivo direto de um no;"<<endl;
    cout<<"(b) Fecho transitivo indireto de um no;"<<endl;
    cout<<"(c) Caminho minimo (Djikstra);"<<endl;
    cout<<"(d) Caminho minimo (Floyd);"<<endl;
    cout<<"(e) Arvore Geradora Minima (Algoritmo de Prim);"<<endl;
    cout<<"(f) Arvore Geradora Minima (Algoritmo de Kruskal);"<<endl;
    cout<<"(g) Arvore de caminhamento em profundidade;"<<endl;
    cout<<"(h) Raio, diametro, centro e periferia do grafo;"<<endl;
    cout<<"(0) Sair;"<<endl<<endl;

    char resp;
    cin >> resp;
    switch (resp) {
        case 'a': {

            char id_no = get_id_entrada();
            vector<char> fecho_transitivo_direto = grafo->fecho_transitivo_direto(id_no);
            cout<<"Metodo de impressao em tela nao implementado"<<endl<<endl;

            if(pergunta_imprimir_arquivo("fecho_trans_dir.txt")) {
                cout<<"Metodo de impressao em arquivo nao implementado"<<endl<<endl;
            }


            break;
        }

        case 'b':{

            char id_no = get_id_entrada();
            vector<char> fecho_transitivo_indireto = grafo->fecho_transitivo_indireto(id_no);
            for(char id : fecho_transitivo_indireto) {
                cout << id << (id != fecho_transitivo_indireto.back() ? ", " : "\n");
            }

            if(pergunta_imprimir_arquivo("fecho_trans_indir.txt")) {
                cout<<"Metodo de impressao em arquivo nao implementado"<<endl;
            }


            break;
        }

        case 'c': {

            char id_no_1 = get_id_entrada();
            char id_no_2 = get_id_entrada();
            vector<char> caminho_minimo_dijkstra = grafo->caminho_minimo_dijkstra(id_no_1,id_no_2);
            for(char id : caminho_minimo_dijkstra) {
                cout << id << endl;
            }

            if(pergunta_imprimir_arquivo("caminho_minimo_dijkstra.txt")) {
                cout<<"Metodo de impressao em arquivo nao implementado"<<endl;
            }


            break;
        }

        case 'd': {

            char id_no_1 = get_id_entrada();
            char id_no_2 = get_id_entrada();
            vector<char> caminho_minimo_floyd = grafo->caminho_minimo_floyd(id_no_1,id_no_2);
            // Impressão no formato solicitado: a,b,c
            if (!caminho_minimo_floyd.empty()) {
                for (size_t i = 0; i < caminho_minimo_floyd.size(); ++i) {
                    cout << caminho_minimo_floyd[i];
                    if (i != caminho_minimo_floyd.size() - 1) cout << ",";
                }
                cout << endl << endl;
            } else {
                cout << "Nao existe caminho entre os vertices." << endl << endl;
            }
            if(pergunta_imprimir_arquivo("caminho_minimo_floyd.txt")) {
                cout<<"Metodo de impressao em arquivo nao implementado"<<endl;
            }
            break;
        }

        case 'e': {

            int tam;
            cout<<"Digite o tamanho do subconjunto: ";
            cin>>tam;

            if(tam > 0 && tam <= grafo->get_ordem()) {

                vector<char> ids = get_conjunto_ids(grafo,tam);
                Grafo* arvore_geradora_minima_prim = grafo->arvore_geradora_minima_prim(ids);
                cout<<"Metodo de impressao em tela nao implementado"<<endl<<endl;

                if(pergunta_imprimir_arquivo("agm_prim.txt")) {
                    cout<<"Metodo de impressao em arquivo nao implementado"<<endl;
                }

                delete arvore_geradora_minima_prim;

            }else {
                cout<<"Valor invalido"<<endl;
            }

            break;
        }

        case 'f': {
            
            imprimir_grafo(grafo);
            int tam;
            cout<<"Digite o tamanho do subconjunto: ";
            cin>>tam;

            if(tam > 0 && tam <= grafo->get_ordem()) {

                vector<char> ids = get_conjunto_ids(grafo,tam);
                Grafo* arvore_geradora_minima_kruskal = grafo->arvore_geradora_minima_kruskal(ids);
                if(arvore_geradora_minima_kruskal) {
                    imprimir_grafo(arvore_geradora_minima_kruskal);
                }
                if(pergunta_imprimir_arquivo("agm_kruskal.txt")) {
                    cout<<"Metodo de impressao em arquivo nao implementado"<<endl;
                }

                delete arvore_geradora_minima_kruskal;

            }else {
                cout<<"Valor invalido"<<endl;
            }

            break;
        }

        case 'g': {

            char id_no = get_id_entrada();
            Grafo* arvore_caminhamento_profundidade = grafo->arvore_caminhamento_profundidade(id_no);
            if (arvore_caminhamento_profundidade) {
                for (No* no : arvore_caminhamento_profundidade->get_lista_adj()) {
                    cout << no->get_id() << ": ";
                    vector<Aresta*> arestas = no->get_arestas();
                    for (size_t i = 0; i < arestas.size(); ++i) {
                        cout << arestas[i]->get_id_destino();
                        if (i != arestas.size() - 1) cout << " -> ";
                    }
                    cout << endl;
                }
                cout << endl;
            } else {
                cout << "Nao foi possivel construir a arvore de caminhamento em profundidade." << endl;
            }
            if(pergunta_imprimir_arquivo("arvore_caminhamento_profundidade.txt")) {
                cout<<"Metodo de impressao em arquivo nao implementado"<<endl;
            }
            delete arvore_caminhamento_profundidade;
            break;
        }

        case 'h': {
            cout << "Raio: " << grafo->raio() << endl;
            cout << "Diametro: " << grafo->diametro() << endl;
            vector<char> centro = grafo->centro();
            vector<char> periferia = grafo->periferia();
            for (char id : centro) {
                cout << "Centro: " << id << endl;
            }
            for (char id : periferia) {
                cout << "Periferia: " << id << endl;
            }

            if(pergunta_imprimir_arquivo("arvore_caminhamento_profundidade.txt")) {
                cout<<"Metodo de impressao em arquivo nao implementado"<<endl;
            }

            break;
        }

        case '0': {
            exit(0);
        }
        
        default: {
            cout<<"Opção inválida"<<endl;
        }
    }

    comandos(grafo);

}

char Gerenciador::get_id_entrada() {
    cout<<"Digite o id de um no: ";
    char id;
    cin>>id;
    cout<<endl;
    return id;
}

vector<char> Gerenciador::get_conjunto_ids(Grafo *grafo, int tam) {
    vector<char> ids = {};
    while((int)ids.size() < tam) {
        char id_no =get_id_entrada();
        bool existe = false;
        for(No* no: grafo->get_lista_adj()){
            if(no->get_id() == id_no){
                existe = true;
                break;
            }
        }

        if(!existe){
            cout<<"Vertice nao existe"<<endl;
        }else{
            bool repetido = find(ids.begin(), ids.end(),id_no) != ids.end();
            if(repetido) {
                    cout<<"Valor repetido"<<endl;
            }else {
                ids.push_back(id_no);
            }
        }

    }

    return ids;
}

bool Gerenciador::pergunta_imprimir_arquivo(string nome_arquivo) {

    cout<<"Imprimir em arquivo externo? ("<<nome_arquivo<<")"<<endl;
    cout<<"(1) Sim;"<<endl;
    cout<<"(2) Nao."<<endl;
    int resp;
    cin>>resp;
    cout<<endl;

    switch (resp) {
        case 1:
            return true;
        case 2:
            return false;
        default:
            cout<<"Resposta invalida"<<endl;
            return pergunta_imprimir_arquivo(nome_arquivo);
    }
}

Grafo* Gerenciador::carregar_informacoes_entrada(string nome_arquivo) {
    ifstream arquivo(nome_arquivo);

    if(!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << nome_arquivo << endl;
        return nullptr;
    }

    bool direcionado, pond_arestas, pond_vertices;
    arquivo >> direcionado >> pond_arestas >> pond_vertices;

    int ordem;
    arquivo >> ordem;

    Grafo* grafo = new Grafo(direcionado, pond_arestas, pond_vertices, ordem);

    //Ler os vértices
    string linha;
    arquivo.ignore();
    for(int i = 0; i < ordem; i++){
        if(getline(arquivo, linha)) {
            istringstream iss(linha);
            char id;
            int peso = 0;

            iss >> id;

            if(pond_vertices)
                iss >> peso;

            No* no = new No(id, peso);
            grafo->adicionar_no(no);
        }
    }

    if (grafo->get_lista_adj().size() != ordem) {

        cerr << "Erro: número de vértices lido não corresponde à ordem especificada." << endl;
        delete grafo;
        return nullptr;
    }

    //Ler as arestas
    char no1, no2;
    int pond = 1;
    while(getline(arquivo, linha)) {
        istringstream iss(linha);
        iss >> no1 >> no2;

        if(!grafo->encontrar_no_por_id(no1) || !grafo->encontrar_no_por_id(no2))
            continue;
        
        if (pond_arestas && !(iss >> pond)) {
            cerr << "Erro ao ler o peso da aresta entre " << no1 << " e " << no2 << endl;
            delete grafo;
            return nullptr;
        }

        Aresta* aresta = new Aresta(no2, pond);
        Aresta* aresta_inversa = new Aresta(no1, pond);
        
        for (No* vertice : grafo->get_lista_adj()) {
            if (vertice->get_id() == no1) {
                vertice->adicionar_aresta(aresta);
            }
            if (!direcionado && vertice->get_id() == no2) {
                vertice->adicionar_aresta(aresta_inversa);
            }
        }
    }
    return grafo;
}

void Gerenciador::imprimir_grafo(Grafo* grafo) {
    for(No* no: grafo->get_lista_adj()) {
        cout << no->get_id() << "("<<  no->get_peso() << ") -> ";
        
        for(Aresta* aresta: no->get_arestas()){
            cout << "{"<< aresta->get_peso() << "} ";
        
            cout << aresta->get_id_destino() << "(" << grafo->encontrar_no_por_id(aresta->get_id_destino())->get_peso() << ") ";
        
        }
        
        cout << endl;
    }
}