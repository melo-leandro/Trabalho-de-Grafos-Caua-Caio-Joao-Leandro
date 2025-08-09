#include "Guloso.h"
#include <vector>
#include <set>
#include <climits>
#include <cfloat>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <numeric>
#include <algorithm>

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

// Função para experimentos científicos - executa todos os algoritmos e gera relatório
void Guloso::executar_experimentos(Grafo &grafo, const string& nome_instancia, const string& arquivo_saida) {
    using namespace chrono;
    
    // Configurações dos experimentos conforme especificação
    const int NUM_EXECUCOES = 10;
    const int ITER_RANDOMIZADO = 30;
    const int ITER_REATIVO = 300;
    const int BLOCO_REATIVO = 40;
    
    // Valores de alpha escolhidos estrategicamente
    float alphas_randomizado[3] = {0.1f, 0.5f, 0.9f};  // Guloso, Balanceado, Aleatório
    float alphas_reativo[3] = {0.1f, 0.5f, 0.9f};      // Mesmos valores para comparação
    
    // Estruturas para armazenar resultados
    vector<float> tempos_adaptativo, pesos_adaptativo;
    vector<vector<char>> solucoes_adaptativo;  // Para armazenar as soluções
    vector<vector<float>> tempos_randomizado(3), pesos_randomizado(3);
    vector<vector<vector<char>>> solucoes_randomizado(3);  // Para armazenar soluções por alpha
    vector<float> tempos_reativo, pesos_reativo;
    vector<vector<char>> solucoes_reativo;  // Para armazenar as soluções
    
    cout << "=== INICIANDO EXPERIMENTOS PARA: " << nome_instancia << " ===" << endl;
    
    // ========== ALGORITMO GULOSO ADAPTATIVO (10 execuções) ==========
    cout << "\n1. Testando Algoritmo Guloso Adaptativo..." << endl;
    for (int exec = 0; exec < NUM_EXECUCOES; ++exec) {
        auto inicio = high_resolution_clock::now();
        auto [solucao, peso] = algoritmo_guloso_adaptativo(grafo);
        auto fim = high_resolution_clock::now();
        
        auto duracao = duration_cast<microseconds>(fim - inicio);
        double tempo_segundos = duracao.count() / 1000000.0;
        
        tempos_adaptativo.push_back(tempo_segundos);
        pesos_adaptativo.push_back(peso);
        solucoes_adaptativo.push_back(solucao);  // Armazena a solução
        
        cout << "   Execução " << setw(2) << (exec + 1) << ": Peso=" << setw(8) << peso 
             << ", Tempo=" << fixed << setprecision(6) << tempo_segundos << "s"
             << ", Solução={";
        for(size_t i = 0; i < solucao.size(); ++i) {
            cout << solucao[i];
            if(i < solucao.size() - 1) cout << ", ";
        }
        cout << "}" << endl;
    }
    
    // ========== ALGORITMO GULOSO RANDOMIZADO (30 iterações por execução) ==========
    cout << "\n2. Testando Algoritmo Guloso Randomizado (" << ITER_RANDOMIZADO << " iterações)..." << endl;
    for (int alpha_idx = 0; alpha_idx < 3; ++alpha_idx) {
        float alpha = alphas_randomizado[alpha_idx];
        cout << "   Alpha = " << alpha << ":" << endl;
        
        for (int exec = 0; exec < NUM_EXECUCOES; ++exec) {
            auto inicio = high_resolution_clock::now();
            auto [solucao, peso] = guloso_randomizado(grafo, alpha, ITER_RANDOMIZADO);
            auto fim = high_resolution_clock::now();
            
            auto duracao = duration_cast<microseconds>(fim - inicio);
            double tempo_segundos = duracao.count() / 1000000.0;
            
            tempos_randomizado[alpha_idx].push_back(tempo_segundos);
            pesos_randomizado[alpha_idx].push_back(peso);
            solucoes_randomizado[alpha_idx].push_back(solucao);  // Armazena a solução
            
            cout << "     Execução " << setw(2) << (exec + 1) << ": Peso=" << setw(8) << peso 
                 << ", Tempo=" << fixed << setprecision(6) << tempo_segundos << "s"
                 << ", Solução={";
            for(size_t i = 0; i < solucao.size(); ++i) {
                cout << solucao[i];
                if(i < solucao.size() - 1) cout << ", ";
            }
            cout << "}" << endl;
        }
    }
    
    // ========== ALGORITMO GULOSO RANDOMIZADO REATIVO (300 iterações) ==========
    cout << "\n3. Testando Algoritmo Guloso Randomizado Reativo (" << ITER_REATIVO << " iterações, bloco=" << BLOCO_REATIVO << ")..." << endl;
    for (int exec = 0; exec < NUM_EXECUCOES; ++exec) {
        auto inicio = high_resolution_clock::now();
        auto [solucao, peso] = guloso_randomizado_reativo(grafo, alphas_reativo, ITER_REATIVO, BLOCO_REATIVO);
        auto fim = high_resolution_clock::now();
        
        auto duracao = duration_cast<microseconds>(fim - inicio);
        double tempo_segundos = duracao.count() / 1000000.0;
        
        tempos_reativo.push_back(tempo_segundos);
        pesos_reativo.push_back(peso);
        solucoes_reativo.push_back(solucao);  // Armazena a solução
        
        cout << "   Execução " << setw(2) << (exec + 1) << ": Peso=" << setw(8) << peso 
             << ", Tempo=" << fixed << setprecision(6) << tempo_segundos << "s"
             << ", Solução={";
        for(size_t i = 0; i < solucao.size(); ++i) {
            cout << solucao[i];
            if(i < solucao.size() - 1) cout << ", ";
        }
        cout << "}" << endl;
    }
    
    // ========== GERAÇÃO DO RELATÓRIO EM ARQUIVO ==========
    ofstream arquivo(arquivo_saida, ios::app);
    if (!arquivo.is_open()) {
        cerr << "ERRO: Não foi possível abrir o arquivo: " << arquivo_saida << endl;
        return;
    }
    
    // Função auxiliar para calcular estatísticas
    auto calcular_stats = [](const vector<float>& dados) -> tuple<float, float, float> {
        if (dados.empty()) return {0.0f, 0.0f, 0.0f};
        
        float melhor = *min_element(dados.begin(), dados.end());
        float soma = accumulate(dados.begin(), dados.end(), 0.0f);
        float media = soma / dados.size();
        
        return {melhor, media, soma};
    };
    
    // Cabeçalho do relatório
    arquivo << "=======================================================" << endl;
    arquivo << "           RESULTADOS EXPERIMENTAIS" << endl;
    arquivo << "=======================================================" << endl;
    arquivo << "Instância: " << nome_instancia << endl;
    arquivo << "Data: " << __DATE__ << " às " << __TIME__ << endl;
    arquivo << "Execuções por algoritmo: " << NUM_EXECUCOES << endl;
    arquivo << "Iterações randomizado: " << ITER_RANDOMIZADO << endl;
    arquivo << "Iterações reativo: " << ITER_REATIVO << " (bloco=" << BLOCO_REATIVO << ")" << endl;
    arquivo << "-------------------------------------------------------" << endl;
    
    // Resultados do Algoritmo Guloso Adaptativo
    auto [melhor_peso_adap, media_peso_adap, _] = calcular_stats(pesos_adaptativo);
    auto [__, media_tempo_adap, ___] = calcular_stats(tempos_adaptativo);
    
    // Encontra a melhor solução do adaptativo
    auto it_adap = min_element(pesos_adaptativo.begin(), pesos_adaptativo.end());
    int idx_melhor_adap = distance(pesos_adaptativo.begin(), it_adap);
    vector<char> melhor_sol_adap = solucoes_adaptativo[idx_melhor_adap];
    
    arquivo << "\n1. ALGORITMO GULOSO ADAPTATIVO:" << endl;
    arquivo << "   Melhor peso: " << melhor_peso_adap << endl;
    arquivo << "   Melhor solução: {";
    for(size_t i = 0; i < melhor_sol_adap.size(); ++i) {
        arquivo << melhor_sol_adap[i];
        if(i < melhor_sol_adap.size() - 1) arquivo << ", ";
    }
    arquivo << "}" << endl;
    arquivo << "   Peso médio: " << fixed << setprecision(3) << media_peso_adap << endl;
    arquivo << "   Tempo médio: " << fixed << setprecision(6) << media_tempo_adap << "s" << endl;
    
    // Resultados do Algoritmo Randomizado
    arquivo << "\n2. ALGORITMO GULOSO RANDOMIZADO:" << endl;
    float melhor_global_randomizado = FLT_MAX;
    for (int alpha_idx = 0; alpha_idx < 3; ++alpha_idx) {
        float alpha = alphas_randomizado[alpha_idx];
        auto [melhor_peso_rand, media_peso_rand, ____] = calcular_stats(pesos_randomizado[alpha_idx]);
        auto [_____, media_tempo_rand, ______] = calcular_stats(tempos_randomizado[alpha_idx]);
        
        // Encontra a melhor solução para este alpha
        auto it_rand = min_element(pesos_randomizado[alpha_idx].begin(), pesos_randomizado[alpha_idx].end());
        int idx_melhor_rand = distance(pesos_randomizado[alpha_idx].begin(), it_rand);
        vector<char> melhor_sol_rand = solucoes_randomizado[alpha_idx][idx_melhor_rand];
        
        melhor_global_randomizado = min(melhor_global_randomizado, melhor_peso_rand);
        
        arquivo << "   Alpha " << alpha << ":" << endl;
        arquivo << "     Melhor peso: " << melhor_peso_rand << endl;
        arquivo << "     Melhor solução: {";
        for(size_t i = 0; i < melhor_sol_rand.size(); ++i) {
            arquivo << melhor_sol_rand[i];
            if(i < melhor_sol_rand.size() - 1) arquivo << ", ";
        }
        arquivo << "}" << endl;
        arquivo << "     Peso médio: " << fixed << setprecision(3) << media_peso_rand << endl;
        arquivo << "     Tempo médio: " << fixed << setprecision(6) << media_tempo_rand << "s" << endl;
    }
    
    // Resultados do Algoritmo Reativo
    auto [melhor_peso_reat, media_peso_reat, _______] = calcular_stats(pesos_reativo);
    auto [________, media_tempo_reat, _________] = calcular_stats(tempos_reativo);
    
    // Encontra a melhor solução do reativo
    auto it_reat = min_element(pesos_reativo.begin(), pesos_reativo.end());
    int idx_melhor_reat = distance(pesos_reativo.begin(), it_reat);
    vector<char> melhor_sol_reat = solucoes_reativo[idx_melhor_reat];
    
    arquivo << "\n3. ALGORITMO GULOSO RANDOMIZADO REATIVO:" << endl;
    arquivo << "   Alphas utilizados: {" << alphas_reativo[0] << ", " << alphas_reativo[1] << ", " << alphas_reativo[2] << "}" << endl;
    arquivo << "   Melhor peso: " << melhor_peso_reat << endl;
    arquivo << "   Melhor solução: {";
    for(size_t i = 0; i < melhor_sol_reat.size(); ++i) {
        arquivo << melhor_sol_reat[i];
        if(i < melhor_sol_reat.size() - 1) arquivo << ", ";
    }
    arquivo << "}" << endl;
    arquivo << "   Peso médio: " << fixed << setprecision(3) << media_peso_reat << endl;
    arquivo << "   Tempo médio: " << fixed << setprecision(6) << media_tempo_reat << "s" << endl;
    
    // Resumo comparativo
    arquivo << "\n======== RESUMO COMPARATIVO ========" << endl;
    arquivo << "Melhor solução por algoritmo:" << endl;
    arquivo << "  Guloso Adaptativo: " << melhor_peso_adap << endl;
    arquivo << "  Guloso Randomizado: " << melhor_global_randomizado << endl;
    arquivo << "  Guloso Reativo: " << melhor_peso_reat << endl;
    
    // Determina o vencedor
    float melhor_absoluto = min({melhor_peso_adap, melhor_global_randomizado, melhor_peso_reat});
    arquivo << "\nMELHOR RESULTADO GERAL: " << melhor_absoluto;
    if (melhor_absoluto == melhor_peso_adap) arquivo << " (Guloso Adaptativo)";
    else if (melhor_absoluto == melhor_global_randomizado) arquivo << " (Guloso Randomizado)";
    else arquivo << " (Guloso Reativo)";
    arquivo << endl;
    
    arquivo << "\n" << string(55, '=') << "\n" << endl;
    arquivo.close();
    
    cout << "\n=== EXPERIMENTOS CONCLUÍDOS ===" << endl;
    cout << "Resultados detalhados salvos em: " << arquivo_saida << endl;
    cout << "Melhor resultado encontrado: " << melhor_absoluto << endl;
}   