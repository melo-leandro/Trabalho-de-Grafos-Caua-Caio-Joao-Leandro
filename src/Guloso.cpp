#include "Guloso.h"
#include <vector>
#include <queue>
#include <set>
#include <climits>
#include <cfloat>
#include <cstdlib>
#include <ctime>

using namespace std;

Guloso::Guloso() {}

Guloso::~Guloso() {}

vector<char> Guloso::algoritmo_guloso(Grafo &grafo) {
        if(grafo.get_lista_adj().empty())
            return {};

        priority_queue<No*, vector<No*>, NoComparator> candidatos;
        vector<char> solucao;
        set<char> nos_usados;
        set<char> vertices_dominados;
        
        for(No* no : grafo.get_lista_adj())
            candidatos.push(no);
        
        while(!candidatos.empty()) {
            No* melhor = candidatos.top();
            candidatos.pop();

            if(nos_usados.find(melhor->get_id()) != nos_usados.end())
                continue;

            bool pode_adicionar = true;

        // === LÓGICA ESPECÍFICA: CONJUNTO DOMINANTE ===
            set<char> vertices_dominados_por_este;
            vertices_dominados_por_este.insert(melhor->get_id());

            for(Aresta* aresta : melhor->get_arestas())
                vertices_dominados_por_este.insert(aresta->get_id_destino());
            
            bool domina_novos = false;
            for(char v: vertices_dominados_por_este) {
                if(vertices_dominados.find(v) == vertices_dominados.end()) {
                    domina_novos = true;
                    break;
                }
            }

            pode_adicionar = domina_novos;
            // === FIM DA LÓGICA ESPECÍFICA ===
            if(!pode_adicionar)
                continue;

            solucao.push_back(melhor->get_id());
            nos_usados.insert(melhor->get_id());
            for (char v : vertices_dominados_por_este) {
                vertices_dominados.insert(v);
            }
        }

    return solucao;
}

// Algoritmo Guloso Adaptativo
vector<char> Guloso::algoritmo_guloso_adaptativo(Grafo &grafo) {
        if(grafo.get_lista_adj().empty())
            return {};

        vector<char> solucao;                    // Conjunto dominante final
        set<char> nos_usados;                    // Nós já incluídos na solução
        set<char> vertices_dominados;            // Vértices já dominados
        
        while(true) {
            No* melhor_candidato = nullptr;
            float melhor_beneficio = 0.0;
            
            // Avalia todos os nós não utilizados
            for(No* no : grafo.get_lista_adj()) {
                if(nos_usados.count(no->get_id()))  // count() é mais eficiente que find() != end()
                    continue;
                
                // Calcula domínio potencial uma única vez
                set<char> dominio_potencial;
                dominio_potencial.insert(no->get_id());
                for(Aresta* aresta : no->get_arestas())
                    dominio_potencial.insert(aresta->get_id_destino());

                // Conta novos vértices que seriam dominados
                int novos_dominados = 0;
                for(char v : dominio_potencial) {
                    if(!vertices_dominados.count(v)) {  // count() mais eficiente
                        novos_dominados++;
                    }
                }
                
                // Early exit se não domina novos vértices
                if(novos_dominados == 0 || no->get_peso() <= 0)
                    continue;
                
                // Critério de eficiência otimizado
                float beneficio = (float)novos_dominados / no->get_peso();
                
                if(beneficio > melhor_beneficio) {
                    melhor_beneficio = beneficio;
                    melhor_candidato = no;
                }
            }
            
            // Se não há mais candidatos válidos, termina
            if(melhor_candidato == nullptr)
                break;
            
            // Adiciona o melhor candidato à solução
            solucao.push_back(melhor_candidato->get_id());
            nos_usados.insert(melhor_candidato->get_id());
            
            // Atualiza vértices dominados (reutiliza cálculo)
            vertices_dominados.insert(melhor_candidato->get_id());
            for(Aresta* aresta : melhor_candidato->get_arestas())
                vertices_dominados.insert(aresta->get_id_destino());
        }

    return solucao;
}

// Algoritmo Guloso Randomizado
vector<char> Guloso::guloso_randomizado(Grafo &grafo, float alpha, int n_iteracoes) {
    if (grafo.get_lista_adj().empty())
        return {};
    
    srand(time(nullptr));
    
    vector<char> melhor_solucao;
    float melhor_peso = FLT_MAX;
    
    for (int iter = 0; iter < n_iteracoes; ++iter) {
        vector<char> solucao;
        set<char> nos_usados;
        set<char> vertices_dominados;
        float peso_total = 0.0;
        
        while (vertices_dominados.size() < grafo.get_lista_adj().size()) {
            vector<pair<No*, float>> candidatos;
            float max_beneficio = 0.0, min_beneficio = FLT_MAX;
            
            // Avalia todos os nós não utilizados
            for (No* no : grafo.get_lista_adj()) {
                if (nos_usados.count(no->get_id()))
                    continue;
                
                // Conta novos vértices dominados
                int novos = 0;
                if (!vertices_dominados.count(no->get_id())) novos++;
                
                for (Aresta* aresta : no->get_arestas()) {
                    if (!vertices_dominados.count(aresta->get_id_destino()))
                        novos++;
                }
                
                if (novos == 0) continue;
                
                float beneficio = (float)novos / no->get_peso();
                candidatos.push_back({no, beneficio});
                
                if (beneficio > max_beneficio) max_beneficio = beneficio;
                if (beneficio < min_beneficio) min_beneficio = beneficio;
            }
            
            if (candidatos.empty()) break;
            
            // Constrói Lista Restrita de Candidatos (LRC)
            vector<No*> LRC;
            float limite = min_beneficio + alpha * (max_beneficio - min_beneficio);
            
            for (auto& par : candidatos) {
                if (par.second >= limite) {
                    LRC.push_back(par.first);
                }
            }
            
            if (LRC.empty()) break;
            
            // Escolha aleatória simples
            No* escolhido = LRC[rand() % LRC.size()];
            
            // Atualiza solução
            solucao.push_back(escolhido->get_id());
            nos_usados.insert(escolhido->get_id());
            peso_total += escolhido->get_peso();
            
            vertices_dominados.insert(escolhido->get_id());
            for (Aresta* aresta : escolhido->get_arestas()) {
                vertices_dominados.insert(aresta->get_id_destino());
            }
        }
        
        // Atualiza melhor solução se válida e melhor
        if (vertices_dominados.size() == grafo.get_lista_adj().size() && peso_total < melhor_peso) {
            melhor_peso = peso_total;
            melhor_solucao = solucao;
        }
    }
    
    return melhor_solucao;
}

// Algoritmo Guloso Randomizado Adaptativo
vector<char> Guloso::guloso_randomizado_reativo(Grafo &grafo, float alpha[], int n_iteracoes, int bloco) {
    if (grafo.get_lista_adj().empty())
        return {};
    
    srand(time(nullptr));
    
    int num_alphas = 10;
    vector<int> contador(num_alphas, 0);
    vector<float> qualidade_total(num_alphas, 0.0);
    
    vector<char> melhor_solucao;
    float melhor_peso = FLT_MAX;
    
    for (int iter = 0; iter < n_iteracoes; ++iter) {
        // Escolhe alpha (inicialmente aleatório, depois baseado em qualidade)
        int alpha_idx = (iter < bloco) ? rand() % num_alphas : 0;
        
        if (iter >= bloco) {
            float melhor_media = -1.0;
            for (int i = 0; i < num_alphas; ++i) {
                if (contador[i] > 0) {
                    float media = qualidade_total[i] / contador[i];
                    if (media > melhor_media) {
                        melhor_media = media;
                        alpha_idx = i;
                    }
                }
            }
        }
        
        float alpha_atual = alpha[alpha_idx];
        
        // Executa GRASP com alpha escolhido
        vector<char> solucao;
        set<char> nos_usados;
        set<char> vertices_dominados;
        float peso_total = 0.0;
        
        while (vertices_dominados.size() < grafo.get_lista_adj().size()) {
            vector<pair<No*, float>> candidatos;
            float max_beneficio = 0.0, min_beneficio = FLT_MAX;
            
            for (No* no : grafo.get_lista_adj()) {
                if (nos_usados.count(no->get_id()) || no->get_peso() <= 0)
                    continue;
                
                int novos = 0;
                if (!vertices_dominados.count(no->get_id())) novos++;
                for (Aresta* aresta : no->get_arestas()) {
                    if (!vertices_dominados.count(aresta->get_id_destino()))
                        novos++;
                }
                
                if (novos == 0) continue;
                
                float beneficio = (float)novos / no->get_peso();
                candidatos.push_back({no, beneficio});
                
                if (beneficio > max_beneficio) max_beneficio = beneficio;
                if (beneficio < min_beneficio) min_beneficio = beneficio;
            }
            
            if (candidatos.empty()) break;
            
            // LRC simplificada
            vector<No*> LRC;
            float limite = min_beneficio + alpha_atual * (max_beneficio - min_beneficio);
            
            for (auto& par : candidatos) {
                if (par.second >= limite) {
                    LRC.push_back(par.first);
                }
            }
            
            if (LRC.empty()) break;
            
            No* escolhido = LRC[rand() % LRC.size()];
            
            solucao.push_back(escolhido->get_id());
            nos_usados.insert(escolhido->get_id());
            peso_total += escolhido->get_peso();
            
            vertices_dominados.insert(escolhido->get_id());
            for (Aresta* aresta : escolhido->get_arestas()) {
                vertices_dominados.insert(aresta->get_id_destino());
            }
        }
        
        // Atualiza melhor solução e estatísticas
        if (vertices_dominados.size() == grafo.get_lista_adj().size()) {
            if (peso_total < melhor_peso) {
                melhor_peso = peso_total;
                melhor_solucao = solucao;
            }
            
            contador[alpha_idx]++;
            qualidade_total[alpha_idx] += (peso_total > 0) ? 1.0 / peso_total : 0.0;
        }
    }
    
    return melhor_solucao;
}   