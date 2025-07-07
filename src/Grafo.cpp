#include "Grafo.h"
#include <functional>
#include <algorithm>
#include <climits>
#include <map>
#include <queue>

using namespace std;

// Construtor
Grafo::Grafo(bool dir, bool pond_arestas, bool pond_vertices, int ordem) {
    set_direcionado(dir);
    set_ponderado_vertice(pond_vertices);
    set_ponderado_aresta(pond_arestas);
    set_ordem(ordem);
}

// Destrutor
Grafo::~Grafo() {
    for(No* no : this->lista_adj) {
        delete no;
    }
}


//Função auxiliar criada para encontrar nó no grafo a partir do ID
No* Grafo::encontrar_no_por_id(char id) {
    for(No* vertice: this->lista_adj){
        if(vertice->get_id() == id)
            return vertice;
    }
    return nullptr;
}

//Função auxiliar criada para encontrar o caminho mínimo e calcular a distância simultâneamente
pair<vector<char>, int> Grafo::auxiliar_dijkstra(char id_no_a, char id_no_b) {
    No* no_a = this->encontrar_no_por_id(id_no_a);
    No* no_b = this->encontrar_no_por_id(id_no_b);
    if(!no_a || !no_b) return {{}, INT_MAX};

    priority_queue<pair<int, char>, vector<pair<int, char>>, greater<>> fila;
    map<char,int> distancia = {};
    map<char, char> predecessor = {};

    for(No* no : this->get_lista_adj())
        distancia[no->get_id()] = INT_MAX;

    distancia[id_no_a] = 0;
    fila.push({0, id_no_a});
    predecessor[id_no_a] = id_no_a;

    while(!fila.empty()) {
        pair<int, char> topo = fila.top();
        int dist_atual = topo.first;
        char atual = topo.second;
        fila.pop();

        if(distancia[atual] < dist_atual) continue;

        No* no_atual = encontrar_no_por_id(atual);
        for(Aresta* aresta : no_atual->get_arestas()) {
            char vizinho = aresta->get_id_destino();
            int peso = aresta->get_peso();
            int nova_dist = dist_atual + peso;

            if(nova_dist < distancia[vizinho]) {
                distancia[vizinho] = nova_dist;
                predecessor[vizinho] = atual;
                fila.push({nova_dist, vizinho});
            }
        }
    }

    if(distancia[id_no_b] == INT_MAX) return {{}, INT_MAX};

    vector<char> caminho = {};
    for(char atual = id_no_b; atual != id_no_a; atual = predecessor[atual]) {
        if(predecessor.find(atual) == predecessor.end()) return {{}, INT_MAX};
        caminho.push_back(atual);
    }

    caminho.push_back(id_no_a);
    reverse(caminho.begin(), caminho.end());

    return {caminho, distancia[id_no_b]};
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
    return this->ordem;
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

// Fecho Transitivo Direto
vector<char> Grafo::fecho_transitivo_direto(char id_no) {
    vector<char> fecho;
    map<char, bool> visitados;
    queue<char> fila;

    No* no_inicial = encontrar_no_por_id(id_no);
    if (!no_inicial) {
        cout << "Nó não encontrado." << endl;
        return {};
    }

    fila.push(id_no);
    visitados[id_no] = true;

    while (!fila.empty()) {
        char atual = fila.front();
        fila.pop();
        
        // Adiciona ao fecho (inclui o nó inicial)
        fecho.push_back(atual);

        // Percorre arestas de SAÍDA (fecho direto)
        No* no_atual = encontrar_no_por_id(atual);
        if (!no_atual) continue;

        for (Aresta* aresta : no_atual->get_arestas()) {
            char vizinho = aresta->get_id_destino();
            if (!visitados[vizinho]) {
                visitados[vizinho] = true;
                fila.push(vizinho);
            }
        }
    }

    return fecho;
}

// Fecho Transitivo Indireto
vector<char> Grafo::fecho_transitivo_indireto(char id_no) {
    vector<char> fecho;
    queue<pair<char, int>> fila_bfs;
    map<char, bool> visitado;

    No* no_inicial = encontrar_no_por_id(id_no);
    if (!no_inicial) {
        cout << "Erro: vértice não encontrado." << endl;
        return fecho;
    }

    fila_bfs.push({id_no, 0});
    visitado[id_no] = true;

    while (!fila_bfs.empty()) {
        auto item = fila_bfs.front();
        fila_bfs.pop();

        char atual = item.first;
        int dist = item.second;

        No* no_atual = encontrar_no_por_id(atual);
        if (!no_atual) continue;

        for (Aresta* aresta : no_atual->get_arestas()) {
            char vizinho = aresta->get_id_destino();
            if (!visitado[vizinho]) {
                visitado[vizinho] = true;
                fila_bfs.push({vizinho, dist + 1});
                if(dist + 1 > 1)
                    fecho.push_back(vizinho);
            }
        }
    }

    sort(fecho.begin(), fecho.end());

    if (fecho.empty()) {
        cout << "Erro: não há fecho transitivo indireto." << endl;
    }

    return fecho;
}


// Caminho Mínimo Dijkstra
vector<char> Grafo::caminho_minimo_dijkstra(char id_no_a, char id_no_b) {
    return auxiliar_dijkstra(id_no_a, id_no_b).first;
}

// Caminho Mínimo Floyd-Warshall
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

// Árvore Geradora Mínima (Prim)
Grafo * Grafo::arvore_geradora_minima_prim(vector<char> ids_nos) {
    if(ids_nos.empty()){
        cout<<"O subconjunto de nós é vazio"<<endl;
        return nullptr;
    }

    if(!is_ponderado_aresta() || is_direcionado()){
        cout << "Não é possível encontrar a AGM de um grafo direcionado ou não ponderado nas arestas" << endl;
        return nullptr;
    }

    // Cria um novo grafo para a AGM
    Grafo* agm = new Grafo(false, this->is_ponderado_aresta(), this->is_ponderado_vertice(), 0);

    // Estruturas auxiliares
    map<char, int> chave;            // Menor peso para conectar à AGM
    map<char, char> predecessor;     // Vértice de origem na AGM
    map<char, bool> presente_agm;    // Indica se o vértice já está na AGM
    priority_queue<pair<int, char>, vector<pair<int, char>>, greater<>> heap;

    // Inicialização
    for (No* no : lista_adj) {
        char id = no->get_id();
        chave[id] = INT_MAX;
        predecessor[id] = '\0';
        presente_agm[id] = false;
    }

    // Começa pelo primeiro nó do subconjunto
    char raiz = ids_nos[0];
    chave[raiz] = 0;
    heap.push({0, raiz});

    // Algoritmo de Prim
    while (!heap.empty()) {
        char u = heap.top().second;
        heap.pop();

        if (presente_agm[u]) continue;
        presente_agm[u] = true;

        // Adiciona o nó e aresta à AGM (exceto a raiz)
        if (predecessor[u] != '\0') {
            No* no_pred = agm->encontrar_no_por_id(predecessor[u]);
            if (!no_pred) {
                no_pred = new No(predecessor[u], 0);
                agm->adicionar_no(no_pred);
            }
            no_pred->adicionar_aresta(new Aresta(u, chave[u]));
        }

        // Processa vizinhos
        No* no_u = encontrar_no_por_id(u);
        if (!no_u) continue;

        for (Aresta* aresta : no_u->get_arestas()) {
            char v = aresta->get_id_destino();
            
            // Verifica se v está no subconjunto de entrada
            if (find(ids_nos.begin(), ids_nos.end(), v) == ids_nos.end()) {
                continue;
            }

            int peso = aresta->get_peso();
            if (!presente_agm[v] && peso < chave[v]) {
                chave[v] = peso;
                predecessor[v] = u;
                heap.push({chave[v], v});
            }
        }
    }

    // Verifica se todos os nós do subconjunto foram conectados
    for (char id : ids_nos) {
        if (!presente_agm[id]) {
            cout << "Subconjunto não forma um grafo conexo." << endl;
            delete agm;
            return nullptr;
        }
    }

    return agm;
}

// Árvore Geradora Mínima (Kruskal)
Grafo * Grafo::arvore_geradora_minima_kruskal(vector<char> ids_nos) {
    if(!is_ponderado_aresta() || is_direcionado()){
        cout << "Não é possível encontrar a AGM de um grafo direcionado ou não ponderado nas arestas" << endl;
        return nullptr;
    }
    
    //Árvore Geradora Mínima
    Grafo* AGM = new Grafo(false, true, is_ponderado_vertice(), ids_nos.size());

    // Primeiro, adiciona todos os nós na AGM
    for(char no: ids_nos){
        No* node = this->encontrar_no_por_id(no);
        if(node){
            AGM->adicionar_no(new No(node->get_id(), node->get_peso()));
        } else {
            cout << "Erro: Nó " << no << " não encontrado no grafo." << endl;
            delete AGM;
            return nullptr;
        }
    }

    vector<Aresta*> lista_arestas;
    for(int i = 0; i < ids_nos.size(); i++){
        No* node = this->encontrar_no_por_id(ids_nos[i]);
        if(node){
            for(Aresta* aresta: node->get_arestas()){
                char destino = aresta->get_id_destino();
                for(int j = i + 1; j < ids_nos.size(); j++){
                    if(ids_nos[j] == destino){
                        lista_arestas.push_back(aresta);
                        break;
                    }
                }
            }
        }
    }

    sort(lista_arestas.begin(), lista_arestas.end(),
         [](Aresta* a, Aresta* b)
            {return a->get_peso() < b->get_peso();}
        );

    //Método Union Find
    map<char, char> pai;
    map<char, int> rank_no;

    for(char id: ids_nos){
        pai[id] = id;
        rank_no[id] = 0;
    }

    //Função find lambda
    function<char(char)> achar_rep = [&](char x) -> char {
        if(pai[x] != x)
            pai[x] = achar_rep(pai[x]);
        return pai[x];
    };

    //Função union lambda
    function<bool(char, char)> unir_grupos = [&](char x, char y) -> bool {
        char paiX = achar_rep(x);
        char paiY = achar_rep(y);

        if (paiX == paiY) return false;

        if (rank_no[paiX] < rank_no[paiY]) {
            pai[paiX] = paiY;
        } else if (rank_no[paiX] > rank_no[paiY]) {
            pai[paiY] = paiX;
        } else {
            pai[paiY] = paiX;
            rank_no[paiX]++;
        }
        return true;
    };

    //Método para achar o id do nó de origem de uma aresta
    function<char(Aresta*)> acha_origem = [&](Aresta* aresta) -> char {
        for(char id: ids_nos){
            No* no = this->encontrar_no_por_id(id);
            if(no != nullptr){
                vector<Aresta*> arestas_no = no->get_arestas();
                if(find(arestas_no.begin(), arestas_no.end(), aresta) != arestas_no.end()){
                    return no->get_id();
                }
            }
        }
        return '\0'; 
    };

    //Algoritmo de Kruskal
    int contador = 0;

    for(Aresta* aresta: lista_arestas){
        char no_origem = acha_origem(aresta);
        if(no_origem == '\0') {
            cout << "Erro: Aresta não encontrada no grafo." << endl;
            delete AGM;
            return nullptr;
        }

        char no_destino = aresta->get_id_destino();

        if(unir_grupos(no_origem, no_destino)){
            No* no_origem_agm = AGM->encontrar_no_por_id(no_origem);
            No* no_destino_agm = AGM->encontrar_no_por_id(no_destino);
            
            if(no_origem_agm != nullptr && no_destino_agm != nullptr){
                no_origem_agm->adicionar_aresta(new Aresta(no_destino, aresta->get_peso()));
                no_destino_agm->adicionar_aresta(new Aresta(no_origem, aresta->get_peso()));
            }
            contador++;
            
            if(contador == ids_nos.size() - 1){
                break;
            }
        }
    }

    if(contador < ids_nos.size() - 1){
        cout << "Não foi possível formar uma árvore geradora mínima - grafo desconexo" << endl;
        delete AGM;
        return nullptr;
    }
    
    return AGM;
}

// Função auxiliar para construir a árvore de caminhamento em profundidade
void Grafo::dfs_util(char atual, map<char, bool>& visitado, Grafo* arvore, map<char, No*>& mapa_novos_nos) {
    visitado[atual] = true;
    No* no_atual = this->encontrar_no_por_id(atual);
    for (Aresta* aresta : no_atual->get_arestas()) {
        char vizinho = aresta->get_id_destino();
        if (!visitado[vizinho]) {
            mapa_novos_nos[atual]->adicionar_aresta(new Aresta(vizinho, aresta->get_peso()));
            if (!this->is_direcionado()) {
                mapa_novos_nos[vizinho]->adicionar_aresta(new Aresta(atual, aresta->get_peso()));
            }
            this->dfs_util(vizinho, visitado, arvore, mapa_novos_nos);
        }
    }
}

// Árvore de Caminhamento em Profundidade
Grafo* Grafo::arvore_caminhamento_profundidade(char id_no) {
    No* no_inicial = encontrar_no_por_id(id_no);
    if (!no_inicial) {
        cout << "Vertice nao encontrado." << endl;
        return nullptr;
    }
    Grafo* arvore = new Grafo(this->is_direcionado(), this->is_ponderado_aresta(), this->is_ponderado_vertice(), this->get_ordem());
    map<char, No*> mapa_novos_nos;
    for (No* no : this->get_lista_adj()) {
        No* novo_no = new No(no->get_id(), no->get_peso());
        arvore->adicionar_no(novo_no);
        mapa_novos_nos[no->get_id()] = novo_no;
    }
    map<char, bool> visitado;
    this->dfs_util(id_no, visitado, arvore, mapa_novos_nos);
    return arvore;
}

// Função auxiliar que calcula a distância entre dois nó usando Dijkstra
int Grafo::distancia(char id_no_a, char id_no_b) {
    return auxiliar_dijkstra(id_no_a, id_no_b).second;
}

// Função auxiliar que retorna a excentricidade de um nó
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

//Função que calcula o raio do grafo
int Grafo::raio() {
    int raio = INT_MAX;

    for(No* no : this->get_lista_adj()) {
        int excentricidade_atual = excentricidade(no->get_id());
        if(excentricidade_atual == INT_MAX) {
            continue;
        }
        if(excentricidade_atual < raio || raio == INT_MAX) {
            raio = excentricidade_atual;
        }
    }
    if(raio == INT_MAX) {
        return -1;
    }
    return raio;
}

// Função que calcula o diâmetro do grafo
int Grafo::diametro() {
    int diametro = INT_MIN;

    for(No* no : this->get_lista_adj()) {
        int excentricidade_atual = excentricidade(no->get_id());
        if(excentricidade_atual == INT_MAX) {
            continue;
        }
        if(excentricidade_atual > diametro || diametro == INT_MIN) {
            diametro = excentricidade_atual;
        }
    }
    if(diametro == INT_MIN || diametro == INT_MAX) {
        return -1;
    }
    return diametro;
}

// Função que retorna o centro do grafo
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

// Função que retorna a periferia do grafo
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

