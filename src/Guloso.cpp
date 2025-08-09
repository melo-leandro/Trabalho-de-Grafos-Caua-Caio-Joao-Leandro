#include "Guloso.h"
#include <vector>
#include <set>
#include <climits>
#include <cfloat>
#include <cstdlib>
#include <ctime>

using namespace std;

Guloso::Guloso() {}

Guloso::~Guloso() {}

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
                
                // Critério de eficiência otimizado - peso já verificado como > 0
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
        return {{}, 0.0f};  // Padronizado: erro retorna peso 0
        
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
            
            // Peso já verificado como > 0 pela condição anterior
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

pair<vector<char>, float> Guloso::guloso_randomizado(Grafo &grafo, float alpha, int n_iteracoes) {
    if (grafo.get_lista_adj().empty() || n_iteracoes <= 0 || alpha < 0.0f || alpha > 1.0f)
        return {{}, 0.0f};
        
    vector<char> melhor_solucao;
    float melhor_peso = FLT_MAX;
    
    for (int iter = 0; iter < n_iteracoes; ++iter) {
        auto [solucao, peso_total] = construir_solucao_grasp(grafo, alpha);
        
        if (!solucao.empty() && peso_total < melhor_peso) {
            melhor_peso = peso_total;
            melhor_solucao = solucao;
        }
    }
    
    return {melhor_solucao, melhor_peso};
}

pair<vector<char>, float> Guloso::guloso_randomizado_reativo(Grafo &grafo, float alpha[], int n_iteracoes, int bloco) {
    if (grafo.get_lista_adj().empty() || alpha == nullptr || n_iteracoes <= 0 || bloco <= 0 || bloco > n_iteracoes)
        return {{}, 0.0f};
    
    int num_alphas = 3;
    vector<int> contador(num_alphas, 0);
    vector<float> qualidade_total(num_alphas, 0.0);
    vector<float> probabilidades(num_alphas, 1.0f / num_alphas);
    
    vector<char> melhor_solucao;
    float melhor_peso = FLT_MAX;
    
    for (int iter = 0; iter < n_iteracoes; ++iter) {
        // Atualiza probabilidades a cada bloco (mudança significativa)
        if ((iter + 1) % bloco == 0) {
            float soma_qualidades = 0.0f;
            for (int j = 0; j < num_alphas; ++j) {
                if (contador[j] > 0) {
                    float media = qualidade_total[j] / contador[j];
                    soma_qualidades += media;
                }
            }
            
            // Recalcula probabilidades baseado na qualidade
            if (soma_qualidades > 0.0f) {
                for (int j = 0; j < num_alphas; ++j) {
                    if (contador[j] > 0) {
                        float media = qualidade_total[j] / contador[j];
                        probabilidades[j] = media / soma_qualidades;
                    } else {
                        probabilidades[j] = 0.0f;
                    }
                }
            } else {
                // Se nenhum alpha teve sucesso, reinicia equiprovável
                for (int j = 0; j < num_alphas; ++j) {
                    probabilidades[j] = 1.0f / num_alphas;
                }
            }
        }
        
        // Seleção probabilística de alpha com fallback
        float rand_val = (float)rand() / RAND_MAX;
        int alpha_idx = 0;
        float acumulado = 0.0f;
        bool alpha_selecionado = false;
        
        for (int j = 0; j < num_alphas; ++j) {
            acumulado += probabilidades[j];
            if (rand_val <= acumulado) {
                alpha_idx = j;
                alpha_selecionado = true;
                break;
            }
        }
        
        // Fallback: se algo deu errado, escolhe aleatoriamente
        if (!alpha_selecionado) {
            alpha_idx = rand() % num_alphas;
        }
        
        // Executa GRASP
        auto [solucao, peso_total] = construir_solucao_grasp(grafo, alpha[alpha_idx]);
        
        // Atualiza melhor solução e estatísticas
        if (!solucao.empty()) {
            if (peso_total < melhor_peso) {
                melhor_peso = peso_total;
                melhor_solucao = solucao;
            }
            
            contador[alpha_idx]++;
            if (peso_total > 0.0f) {
                qualidade_total[alpha_idx] += 1.0f / peso_total;
            } else {
                qualidade_total[alpha_idx] += 1000.0f;
            }
        }
    }
    
    return {melhor_solucao, melhor_peso};
}   