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

// Algoritmo Guloso Adaptativo
pair<vector<char>, float> Guloso::algoritmo_guloso_adaptativo(Grafo &grafo) {
        if(grafo.get_lista_adj().empty())
            return {{}, 0.0f};

        vector<char> solucao;                    // Conjunto dominante final
        set<char> nos_usados;                    // Nós já incluídos na solução
        set<char> vertices_dominados;            // Vértices já dominados
        float peso_total = 0.0;                  // Rastreia peso total da solução
        
        while(vertices_dominados.size() < grafo.get_lista_adj().size()) {  // Unificação: mesmo critério do GRASP
            No* melhor_candidato = nullptr;
            float melhor_beneficio = 0.0;
            
            // Avalia todos os nós não utilizados
            for(No* no : grafo.get_lista_adj()) {
                if(nos_usados.count(no->get_id()))
                    continue;
                
                // Nós com peso <= 0 são sempre ótimos - inclui imediatamente
                if(no->get_peso() <= 0) {
                    solucao.push_back(no->get_id());
                    nos_usados.insert(no->get_id());
                    peso_total += no->get_peso();  // Soma peso (pode ser negativo)
                    vertices_dominados.insert(no->get_id());
                    for(Aresta* aresta : no->get_arestas())
                        vertices_dominados.insert(aresta->get_id_destino());
                    continue;
                }
                
                // Calcula domínio potencial uma única vez
                set<char> dominio_potencial;
                dominio_potencial.insert(no->get_id());
                for(Aresta* aresta : no->get_arestas())
                    dominio_potencial.insert(aresta->get_id_destino());

                // Conta novos vértices que seriam dominados
                int novos_dominados = 0;
                for(char v : dominio_potencial) {
                    if(!vertices_dominados.count(v)) {
                        novos_dominados++;
                    }
                }
                
                // Early exit se não domina novos vértices
                if(novos_dominados == 0)
                    continue;
                
                // Critério de eficiência otimizado
                float beneficio = (float)novos_dominados / no->get_peso();
                
                if(beneficio > melhor_beneficio) {
                    melhor_beneficio = beneficio;
                    melhor_candidato = no;
                }
            }
            
            // Se não há mais candidatos válidos, termina (critério unificado)
            if(melhor_candidato == nullptr)
                break;
            
            // Adiciona o melhor candidato à solução
            solucao.push_back(melhor_candidato->get_id());
            nos_usados.insert(melhor_candidato->get_id());
            peso_total += melhor_candidato->get_peso();  // Soma peso do candidato escolhido
            
            // Atualiza vértices dominados (reutiliza cálculo)
            vertices_dominados.insert(melhor_candidato->get_id());
            for(Aresta* aresta : melhor_candidato->get_arestas())
                vertices_dominados.insert(aresta->get_id_destino());
        }

    return {solucao, peso_total};
}

// Método auxiliar para construção dos randomizados
pair<vector<char>, float> Guloso::construir_solucao_grasp(Grafo &grafo, float alpha) {
    if (grafo.get_lista_adj().empty() || alpha < 0.0f || alpha > 1.0f)
        return {{}, FLT_MAX};  // Retorna solução inválida
        
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
            
            // Nós com peso <= 0 são sempre ótimos - inclui imediatamente
            if(no->get_peso() <= 0) {
                solucao.push_back(no->get_id());
                nos_usados.insert(no->get_id());
                peso_total += no->get_peso();
                vertices_dominados.insert(no->get_id());
                for (Aresta* aresta : no->get_arestas())
                    vertices_dominados.insert(aresta->get_id_destino());
                continue;
            }

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
        
        // Escolha aleatória
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
    
    return {solucao, peso_total};
}

// Algoritmo Guloso Randomizado
vector<char> Guloso::guloso_randomizado(Grafo &grafo, float alpha, int n_iteracoes) {
    if (grafo.get_lista_adj().empty() || n_iteracoes <= 0 || alpha < 0.0f || alpha > 1.0f)
        return {};
        
    vector<char> melhor_solucao;
    float melhor_peso = FLT_MAX;
    
    for (int iter = 0; iter < n_iteracoes; ++iter) {
        auto [solucao, peso_total] = construir_solucao_grasp(grafo, alpha);
        
        // Atualiza melhor solução se válida e melhor
        if (!solucao.empty() && peso_total < melhor_peso) {
            melhor_peso = peso_total;
            melhor_solucao = solucao;
        }
    }
    
    return melhor_solucao;
}

// Algoritmo Guloso Randomizado Adaptativo
vector<char> Guloso::guloso_randomizado_reativo(Grafo &grafo, float alpha[], int n_iteracoes, int bloco) {
    if (grafo.get_lista_adj().empty() || alpha == nullptr || n_iteracoes <= 0 || bloco <= 0 || bloco >= n_iteracoes)
        return {};
        
    int num_alphas = 10;
    vector<int> contador(num_alphas, 0);
    vector<float> qualidade_total(num_alphas, 0.0);
    
    vector<char> melhor_solucao;
    float melhor_peso = FLT_MAX;
    
    for (int iter = 0; iter < n_iteracoes; ++iter) {
        // Escolhe alpha (inicialmente aleatório, depois baseado em qualidade)
        int alpha_idx = rand() % num_alphas;
        
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
        auto [solucao, peso_total] = construir_solucao_grasp(grafo, alpha_atual);
        
        // Atualiza melhor solução e estatísticas
        if (!solucao.empty()) {
            if (peso_total < melhor_peso) {
                melhor_peso = peso_total;
                melhor_solucao = solucao;
            }
            
            contador[alpha_idx]++;
            // Qualidade: peso_total <= 0 indica solução ótima (qualidade muito alta)
            if (peso_total > 0.0f) {
                qualidade_total[alpha_idx] += 1.0f / peso_total;
            } else {
                qualidade_total[alpha_idx] += 1000.0f;
            }
        }
    }
    
    return melhor_solucao;
}   