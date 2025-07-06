#include "Grafo.h"
#include <algorithm>
#include <climits>
#include <map>

using namespace std;

Grafo::Grafo(bool dir, bool pond_arestas, bool pond_vertices, int ordem) {
    set_direcionado(dir);
    set_ponderado_vertice(pond_vertices);
    set_ponderado_aresta(pond_arestas);
    set_ordem(ordem);
}

//Função criada para encontrar nó no grafo a partir do ID
No* Grafo::encontrar_no_por_id(char id) {
    for(No* vertice: this->lista_adj){
        if(vertice->get_id() == id)
            return vertice;
    }
    return nullptr;
}

//Função criada para encontrar o caminho mínimo e calcular a distância simultâneamente
pair<vector<char>, int> Grafo::auxiliar_dijkstra(char id_no_a, char id_no_b) {

    No* no_a = this->encontrar_no_por_id(id_no_a);
    No* no_b = this->encontrar_no_por_id(id_no_b);
    if(!no_a || !no_b) return {};

    map<char,int> distancia = {};
    map<char, char> predecessor = {};
    vector<No*> livres = lista_adj;

    for(No* no : this->get_lista_adj())
        distancia[no->get_id()] = INT_MAX;
    distancia[id_no_a] = 0;
    predecessor[id_no_a] = id_no_a;

    while(!livres.empty()) {
        No* mais_prox= nullptr;
        int menor_dist = INT_MAX;
        for(No* no : livres) {
            if(distancia[no->get_id()] < menor_dist) {
                mais_prox = no;
                menor_dist = distancia[no->get_id()];
            }
        }

        if(!mais_prox)
        break;

        livres.erase(remove(livres.begin(), livres.end(), mais_prox), livres.end());

        for(Aresta* aresta : mais_prox->get_arestas()) {
            int nova_distancia = distancia[mais_prox->get_id()] + aresta->get_peso();
            char id_sucessor = aresta->get_id_destino();
            if(nova_distancia < distancia[id_sucessor]) {
                distancia[id_sucessor] = nova_distancia;
                predecessor[id_sucessor] = mais_prox->get_id();
                livres.push_back(encontrar_no_por_id(id_sucessor));
            }
        }
    }

    char atual = id_no_b;
    vector<char> caminho = {};
    while(atual != id_no_a) {
        caminho.push_back(atual);
        if(predecessor.find(atual) == predecessor.end()) {
            return {};
        }
        atual = predecessor[atual];
    }
    caminho.push_back(id_no_a);
    reverse(caminho.begin(), caminho.end());

    return {caminho, distancia[id_no_b]};
}

Grafo::~Grafo() {
    
}


bool Grafo::is_direcionado() {
    return this->in_direcionado;
}

void Grafo::set_direcionado(bool dir) {
    this->in_direcionado = dir;
}

bool Grafo::is_ponderado_vertice() {
    return this->in_ponderado_vertice;
}

void Grafo::set_ponderado_vertice(bool p_vertice) {
    this->in_ponderado_vertice = p_vertice;
}

bool Grafo::is_ponderado_aresta() {
    return this->in_ponderado_aresta;
}

void Grafo::set_ponderado_aresta(bool p_aresta) {
    this->in_ponderado_aresta = p_aresta;
}

int Grafo::get_ordem() {
    return this->in_ponderado_aresta;
}

void Grafo::set_ordem(int ordem) {
    this->ordem = ordem;
}

vector<No*> Grafo::get_lista_adj() {
    return this->lista_adj;
}

void Grafo::adicionar_no(No* no){
    this->lista_adj.push_back(no);
}

vector<char> Grafo::fecho_transitivo_direto(char id_no) {
    cout<<"Metodo nao implementado"<<endl;
    return {};
}

vector<char> Grafo::fecho_transitivo_indireto(char id_no) {
    cout<<"Metodo nao implementado"<<endl;
    return {};
}

vector<char> Grafo::caminho_minimo_dijkstra(char id_no_a, char id_no_b) {
    return auxiliar_dijkstra(id_no_a, id_no_b).first;
}

vector<char> Grafo::caminho_minimo_floyd(char id_no_a, char id_no_b) {
    // Mapeia id dos vértices para índices
    vector<No*> nos = this->get_lista_adj();
    int n = nos.size();
    map<char, int> id2idx;
    map<int, char> idx2id;
    for (int i = 0; i < n; ++i) {
        id2idx[nos[i]->get_id()] = i;
        idx2id[i] = nos[i]->get_id();
    }
    if (id2idx.find(id_no_a) == id2idx.end() || id2idx.find(id_no_b) == id2idx.end()) {
        cout << "Erro: vértice(s) não encontrado(s)." << endl;
        return {};
    }
    // Inicializa matriz de distâncias e predecessores
    vector<vector<int>> dist(n, vector<int>(n, INT_MAX/2));
    vector<vector<int>> pred(n, vector<int>(n, -1));
    for (int i = 0; i < n; ++i) {
        dist[i][i] = 0;
        pred[i][i] = i;
    }
    for (int i = 0; i < n; ++i) {
        for (Aresta* aresta : nos[i]->get_arestas()) {
            int j = id2idx[aresta->get_id_destino()];
            dist[i][j] = aresta->get_peso();
            pred[i][j] = i;
        }
    }
    // Floyd-Warshall
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    pred[i][j] = pred[k][j];
                }
            }
        }
    }
    int u = id2idx[id_no_a];
    int v = id2idx[id_no_b];
    if (dist[u][v] >= INT_MAX/2) {
        cout << "Não existe caminho entre os vértices." << endl;
        return {};
    }
    // Reconstrói caminho
    vector<char> caminho;
    int atual = v;
    while (atual != u) {
        caminho.push_back(idx2id[atual]);
        atual = pred[u][atual];
        if (atual == -1) {
            cout << "Erro ao reconstruir caminho." << endl;
            return {};
        }
    }
    caminho.push_back(idx2id[u]);
    reverse(caminho.begin(), caminho.end());
    return caminho;
}

Grafo * Grafo::arvore_geradora_minima_prim(vector<char> ids_nos) {
    cout<<"Metodo nao implementado"<<endl;
    return nullptr;
}

Grafo * Grafo::arvore_geradora_minima_kruskal(vector<char> ids_nos) {
    cout<<"Metodo nao implementado"<<endl;
    return nullptr;
}

Grafo * Grafo::arvore_caminhamento_profundidade(char id_no) {
    cout<<"Metodo nao implementado"<<endl;
    return nullptr;
}

int Grafo::distancia(char id_no_a, char id_no_b) {
    return auxiliar_dijkstra(id_no_a, id_no_b).second;
}

map<char, int> Grafo::todas_excentricidades() {
    map<char, int> resultado;
    for (No* no : this->get_lista_adj()) {
        resultado[no->get_id()] = excentricidade(no->get_id());
    }
    return resultado;
}


int Grafo::excentricidade(char id_no) {
    No* no = this->encontrar_no_por_id(id_no);
    if(!no) return INT_MAX;

    int max_distancia = 0;
    for(No* outro_no : this->get_lista_adj()) {
        if(outro_no->get_id() != id_no) {
            int dist = distancia(no->get_id(), outro_no->get_id());
            if(dist > max_distancia) {
                max_distancia = dist;
            }
        }
    }
    return max_distancia;
}

int Grafo::raio() {
    int raio = INT_MAX;

    for(No* no : this->get_lista_adj()) {
        int excentricidade_atual = excentricidade(no->get_id());
        if(excentricidade_atual == -1) {
            continue;
        }
        if(excentricidade_atual < raio || raio == INT_MAX) {
            raio = excentricidade_atual;
        }
    }
    return raio;
}

int Grafo::diametro() {
    int diametro = INT_MIN;

    for(No* no : this->get_lista_adj()) {
        int excentricidade_atual = excentricidade(no->get_id());
        if(excentricidade_atual == -1) {
            continue;
        }
        if(excentricidade_atual > diametro || diametro == INT_MIN) {
            diametro = excentricidade_atual;
        }
    }
    return diametro;
}

vector<char> Grafo::centro() {
    int raio = this->raio();
    if(raio == INT_MAX) {
        return {};
    }

    vector<char> centro;

    for(No* no : this->get_lista_adj()) {
        int excentricidade_atual = excentricidade(no->get_id());
        if(excentricidade_atual == raio) {
            centro.push_back(no->get_id());
        }
    }
    return centro;
}

vector<char> Grafo::periferia() {
    int diametro = this->diametro();
    if(diametro == INT_MIN) {
        return {};
    }

    vector<char> periferia;

    for(No* no : this->get_lista_adj()) {
        int excentricidade_atual = excentricidade(no->get_id());
        if(excentricidade_atual == diametro) {
            periferia.push_back(no->get_id());
        }
    }
    return periferia;
}

