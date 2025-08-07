#include "Guloso.h"
#include <vector>
#include <queue>
#include <set>

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
vector<char> Guloso::gulosoRandomizado(Grafo &grafo, float alpha, int n_iteracoes) {
    return {};
}

// Algoritmo Guloso Randomizado Adaptativo
vector<char> Guloso::gulosoRandomizadoReativo(Grafo &grafo, float alpha[], int n_iteracoes, int bloco) {
    return {};
}   