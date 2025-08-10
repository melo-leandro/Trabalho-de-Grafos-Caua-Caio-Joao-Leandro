#include "Guloso.h"
#include "Gerenciador.h"
#include <vector>
#include <set>
#include <climits>
#include <cfloat>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <fstream>
#include <iomanip>
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
                
                // Nós com peso == 0 são sempre ótimos - inclui imediatamente
                if(no->get_peso() == 0) {
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
            
            // Nós com peso = 0 são sempre ótimos - inclui imediatamente
            if(no->get_peso() == 0) {
                solucao.push_back(no->get_id());
                nos_usados.insert(no->get_id());
                peso_total += no->get_peso();
                vertices_dominados.insert(no->get_id());
                for (Aresta* aresta : no->get_arestas())
                    vertices_dominados.insert(aresta->get_id_destino());
                continue;
            }

            // Conta novos vértices dominados
            int novos_dominados = 0;
            if (!vertices_dominados.count(no->get_id())) novos_dominados++;
            
            for (Aresta* aresta : no->get_arestas()) {
                if (!vertices_dominados.count(aresta->get_id_destino()))
                    novos_dominados++;
            }
            
            if (novos_dominados == 0) continue;
            
            // Peso já verificado como > 0 pela condição anterior
            float beneficio = (float)novos_dominados / no->get_peso();
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

// Experimentos focados na qualidade da melhor solução (peso total)
void Guloso::experimentos_melhor_solucao(Grafo &grafo, const string& nome_instancia, const string& arquivo_saida) {
    // Configurações dos experimentos
    const int NUM_EXECUCOES = 10;
    const int ITER_RANDOMIZADO = 30;
    const int ITER_REATIVO = 300;
    const int BLOCO_REATIVO = 40;
    
    // Valores de alpha estratégicos
    float alphas_randomizado[3] = {0.25f, 0.5f, 0.75f};
    float alphas_reativo[3] = {0.25f, 0.5f, 0.75f};
    
    // Estruturas para armazenar apenas o melhor resultado de cada algoritmo
    float melhor_peso_adaptativo = FLT_MAX;
    vector<char> melhor_solucao_adaptativo;
    
    vector<float> melhor_peso_randomizado(3, FLT_MAX);
    vector<vector<char>> melhor_solucao_randomizado(3);
    
    float melhor_peso_reativo = FLT_MAX;
    vector<char> melhor_solucao_reativo;
    
    cout << "Executando experimentos de melhor solução para: " << nome_instancia << endl;
    
    // ========== ALGORITMO GULOSO ADAPTATIVO ==========
    cout << "1. Algoritmo Guloso Adaptativo..." << endl;
    for (int exec = 0; exec < NUM_EXECUCOES; ++exec) {
        auto [solucao, peso] = algoritmo_guloso_adaptativo(grafo);
        
        if (peso < melhor_peso_adaptativo) {
            melhor_peso_adaptativo = peso;
            melhor_solucao_adaptativo = solucao;
        }
    }
    
    // ========== ALGORITMO GULOSO RANDOMIZADO ==========
    cout << "2. Algoritmo Guloso Randomizado..." << endl;
    for (int alpha_idx = 0; alpha_idx < 3; ++alpha_idx) {
        float alpha = alphas_randomizado[alpha_idx];
        cout << "   Alpha = " << alpha << endl;
        
        for (int exec = 0; exec < NUM_EXECUCOES; ++exec) {
            auto [solucao, peso] = guloso_randomizado(grafo, alpha, ITER_RANDOMIZADO);
            
            if (peso < melhor_peso_randomizado[alpha_idx]) {
                melhor_peso_randomizado[alpha_idx] = peso;
                melhor_solucao_randomizado[alpha_idx] = solucao;
            }
        }
    }
    
    // ========== ALGORITMO GULOSO RANDOMIZADO REATIVO ==========
    cout << "3. Algoritmo Guloso Randomizado Reativo..." << endl;
    for (int exec = 0; exec < NUM_EXECUCOES; ++exec) {
        auto [solucao, peso] = guloso_randomizado_reativo(grafo, alphas_reativo, ITER_REATIVO, BLOCO_REATIVO);
        
        if (peso < melhor_peso_reativo) {
            melhor_peso_reativo = peso;
            melhor_solucao_reativo = solucao;
        }
    }
    
    // ========== GERAÇÃO DO RELATÓRIO FOCADO NA MELHOR SOLUÇÃO ==========
    ofstream arquivo(arquivo_saida, ios::app);
    if (!arquivo.is_open()) {
        cerr << "ERRO: Não foi possível abrir o arquivo: " << arquivo_saida << endl;
        return;
    }
    
    // Cabeçalho do relatório
    arquivo << "=======================================================" << endl;
    arquivo << "       RELATÓRIO DE MELHORES SOLUÇÕES ENCONTRADAS" << endl;
    arquivo << "=======================================================" << endl;
    arquivo << "Instância: " << nome_instancia << endl;
    arquivo << "Execuções por algoritmo: " << NUM_EXECUCOES << endl;
    arquivo << "Iterações randomizado: " << ITER_RANDOMIZADO << endl;
    arquivo << "Iterações reativo: " << ITER_REATIVO << " (bloco=" << BLOCO_REATIVO << ")" << endl;
    arquivo << "Critério: MELHOR PESO TOTAL encontrado" << endl;
    arquivo << "-------------------------------------------------------" << endl;
    
    // Resultado do Algoritmo Guloso Adaptativo
    arquivo << "\n1. ALGORITMO GULOSO ADAPTATIVO:" << endl;
    arquivo << "   Melhor peso encontrado: " << melhor_peso_adaptativo << endl;
    arquivo << "   Tamanho da solução: " << melhor_solucao_adaptativo.size() << " vértices" << endl;
    arquivo << "   Melhor solução: {";
    for(size_t i = 0; i < melhor_solucao_adaptativo.size(); ++i) {
        arquivo << melhor_solucao_adaptativo[i];
        if(i < melhor_solucao_adaptativo.size() - 1) arquivo << ", ";
    }
    arquivo << "}" << endl;
    
    // Resultados do Algoritmo Randomizado
    arquivo << "\n2. ALGORITMO GULOSO RANDOMIZADO:" << endl;
    float melhor_global_randomizado = FLT_MAX;
    int melhor_alpha_idx = 0;
    
    for (int alpha_idx = 0; alpha_idx < 3; ++alpha_idx) {
        float alpha = alphas_randomizado[alpha_idx];
        float peso = melhor_peso_randomizado[alpha_idx];
        
        if (peso < melhor_global_randomizado) {
            melhor_global_randomizado = peso;
            melhor_alpha_idx = alpha_idx;
        }
        
        arquivo << "   Alpha " << alpha << ":" << endl;
        arquivo << "     Melhor peso: " << peso << endl;
        arquivo << "     Tamanho da solução: " << melhor_solucao_randomizado[alpha_idx].size() << " vértices" << endl;
        arquivo << "     Melhor solução: {";
        for(size_t i = 0; i < melhor_solucao_randomizado[alpha_idx].size(); ++i) {
            arquivo << melhor_solucao_randomizado[alpha_idx][i];
            if(i < melhor_solucao_randomizado[alpha_idx].size() - 1) arquivo << ", ";
        }
        arquivo << "}" << endl;
    }
    
    // Resultado do Algoritmo Reativo
    arquivo << "\n3. ALGORITMO GULOSO RANDOMIZADO REATIVO:" << endl;
    arquivo << "   Alphas utilizados: {" << alphas_reativo[0] << ", " << alphas_reativo[1] << ", " << alphas_reativo[2] << "}" << endl;
    arquivo << "   Melhor peso encontrado: " << melhor_peso_reativo << endl;
    arquivo << "   Tamanho da solução: " << melhor_solucao_reativo.size() << " vértices" << endl;
    arquivo << "   Melhor solução: {";
    for(size_t i = 0; i < melhor_solucao_reativo.size(); ++i) {
        arquivo << melhor_solucao_reativo[i];
        if(i < melhor_solucao_reativo.size() - 1) arquivo << ", ";
    }
    arquivo << "}" << endl;
    
    // Comparação final das melhores soluções
    arquivo << "\n======== COMPARAÇÃO DAS MELHORES SOLUÇÕES ========" << endl;
    arquivo << "Melhor peso por algoritmo:" << endl;
    arquivo << "  Guloso Adaptativo: " << melhor_peso_adaptativo 
            << " (" << melhor_solucao_adaptativo.size() << " vértices)" << endl;
    arquivo << "  Guloso Randomizado: " << melhor_global_randomizado 
            << " (" << melhor_solucao_randomizado[melhor_alpha_idx].size() << " vértices)"
            << " [Alpha=" << alphas_randomizado[melhor_alpha_idx] << "]" << endl;
    arquivo << "  Guloso Reativo: " << melhor_peso_reativo 
            << " (" << melhor_solucao_reativo.size() << " vértices)" << endl;
    
    // Determina a melhor solução absoluta
    float melhor_absoluto = min({melhor_peso_adaptativo, melhor_global_randomizado, melhor_peso_reativo});
    arquivo << "\n*** MELHOR SOLUÇÃO ABSOLUTA ***" << endl;
    arquivo << "Peso: " << melhor_absoluto << endl;
    
    if (melhor_absoluto == melhor_peso_adaptativo) {
        arquivo << "Algoritmo: Guloso Adaptativo" << endl;
        arquivo << "Vértices: " << melhor_solucao_adaptativo.size() << endl;
        arquivo << "Solução: {";
        for(size_t i = 0; i < melhor_solucao_adaptativo.size(); ++i) {
            arquivo << melhor_solucao_adaptativo[i];
            if(i < melhor_solucao_adaptativo.size() - 1) arquivo << ", ";
        }
        arquivo << "}" << endl;
    } 
    else if (melhor_absoluto == melhor_global_randomizado) {
        arquivo << "Algoritmo: Guloso Randomizado (Alpha=" << alphas_randomizado[melhor_alpha_idx] << ")" << endl;
        arquivo << "Vértices: " << melhor_solucao_randomizado[melhor_alpha_idx].size() << endl;
        arquivo << "Solução: {";
        for(size_t i = 0; i < melhor_solucao_randomizado[melhor_alpha_idx].size(); ++i) {
            arquivo << melhor_solucao_randomizado[melhor_alpha_idx][i];
            if(i < melhor_solucao_randomizado[melhor_alpha_idx].size() - 1) arquivo << ", ";
        }
        arquivo << "}" << endl;
    } 
    else {
        arquivo << "Algoritmo: Guloso Reativo" << endl;
        arquivo << "Vértices: " << melhor_solucao_reativo.size() << endl;
        arquivo << "Solução: {";
        for(size_t i = 0; i < melhor_solucao_reativo.size(); ++i) {
            arquivo << melhor_solucao_reativo[i];
            if(i < melhor_solucao_reativo.size() - 1) arquivo << ", ";
        }
        arquivo << "}" << endl;
    }
    
    arquivo << "\n" << string(55, '=') << "\n" << endl;
    arquivo.close();
    
    cout << "Experimentos de melhor solução concluídos." << endl;
    cout << "Melhor peso absoluto encontrado: " << melhor_absoluto << endl;
    cout << "Resultados salvos em: " << arquivo_saida << endl;
}

// Função consolidada para processar todas as instâncias e gerar tabela final
void Guloso::executar_experimentos_qualidades(const string& diretorio_instancias, const string& arquivo_saida) {
    // Configurações dos experimentos
    const int NUM_EXECUCOES = 10;
    const int ITER_RANDOMIZADO = 30;
    const int ITER_REATIVO = 300;
    const int BLOCO_REATIVO = 40;
    
    // Valores de alpha conforme especificação
    float alphas_randomizado[3] = {0.25f, 0.5f, 0.75f};
    float alphas_reativo[3] = {0.25f, 0.5f, 0.75f};
    
    // Lista de instâncias a processar
    vector<string> instancias = {
        "g_25_0.16_0_1_0.txt", "g_25_0.16_0_1_1.txt",
        "g_25_0.21_0_1_0.txt", "g_25_0.21_0_1_1.txt", 
        "g_25_0.26_0_1_0.txt", "g_25_0.26_0_1_1.txt",
        "g_40_0.10_0_1_0.txt", "g_40_0.10_0_1_1.txt",
        "g_40_0.15_0_1_0.txt", "g_40_0.15_0_1_1.txt",
        "g_40_0.20_0_1_0.txt", "g_40_0.20_0_1_1.txt",
        "g_60_0.07_0_1_0.txt", "g_60_0.07_0_1_1.txt",
        "g_60_0.12_0_1_0.txt", "g_60_0.12_0_1_1.txt",
        "g_60_0.17_0_1_0.txt", "g_60_0.17_0_1_1.txt"
    };
    
    // Estrutura para armazenar resultados de todas as instâncias
    struct ResultadoInstancia {
        string nome;
        int num_vertices;
        float melhor_known; // Será preenchido manualmente ou calculado
        float guloso;
        float randomizado_02;
        float randomizado_03;
        float randomizado_04;  
        float reativo;
    };
    
    vector<ResultadoInstancia> resultados;
    
    cout << "=== PROCESSANDO TODAS AS INSTÂNCIAS ===" << endl;
    cout << "Total de instâncias: " << instancias.size() << endl;
    
    for (size_t idx = 0; idx < instancias.size(); ++idx) {
        const string& nome_instancia = instancias[idx];
        string caminho_completo = diretorio_instancias + "/" + nome_instancia;
        
        cout << "\nProcessando (" << (idx + 1) << "/" << instancias.size() << "): " << nome_instancia << endl;
        
        // Carregar grafo da instância
        Grafo* grafo = Gerenciador::carregar_informacoes_entrada(caminho_completo.c_str());
        if (!grafo) {
            cout << "ERRO: Não foi possível carregar " << nome_instancia << endl;
            continue;
        }
        
        ResultadoInstancia resultado;
        resultado.nome = nome_instancia;
        resultado.num_vertices = grafo->get_lista_adj().size();
        resultado.melhor_known = 0; // Será ajustado depois
        
        // ========== ALGORITMO GULOSO ADAPTATIVO ==========
        float melhor_guloso = FLT_MAX;
        for (int exec = 0; exec < NUM_EXECUCOES; ++exec) {
            auto [solucao, peso] = algoritmo_guloso_adaptativo(*grafo);
            melhor_guloso = min(melhor_guloso, peso);
        }
        resultado.guloso = melhor_guloso;
        
        // ========== ALGORITMO GULOSO RANDOMIZADO ==========
        vector<float> melhores_randomizado(3, FLT_MAX);
        for (int alpha_idx = 0; alpha_idx < 3; ++alpha_idx) {
            for (int exec = 0; exec < NUM_EXECUCOES; ++exec) {
                auto [solucao, peso] = guloso_randomizado(*grafo, alphas_randomizado[alpha_idx], ITER_RANDOMIZADO);
                melhores_randomizado[alpha_idx] = min(melhores_randomizado[alpha_idx], peso);
            }
        }
        resultado.randomizado_02 = melhores_randomizado[0];
        resultado.randomizado_03 = melhores_randomizado[1]; 
        resultado.randomizado_04 = melhores_randomizado[2];
        
        // ========== ALGORITMO GULOSO RANDOMIZADO REATIVO ==========
        float melhor_reativo = FLT_MAX;
        for (int exec = 0; exec < NUM_EXECUCOES; ++exec) {
            auto [solucao, peso] = guloso_randomizado_reativo(*grafo, alphas_reativo, ITER_REATIVO, BLOCO_REATIVO);
            melhor_reativo = min(melhor_reativo, peso);
        }
        resultado.reativo = melhor_reativo;
        
        // Define melhor_known como o menor valor encontrado
        resultado.melhor_known = min({resultado.guloso, resultado.randomizado_02, 
                                     resultado.randomizado_03, resultado.randomizado_04, resultado.reativo});
        
        resultados.push_back(resultado);
        
        cout << "  Guloso: " << resultado.guloso << " | Random: " << resultado.randomizado_02 
             << ", " << resultado.randomizado_03 << ", " << resultado.randomizado_04 
             << " | Reativo: " << resultado.reativo << endl;
        
        delete grafo;
    }
    
    // ========== GERAÇÃO DA TABELA CONSOLIDADA ==========
    ofstream arquivo(arquivo_saida);
    if (!arquivo.is_open()) {
        cerr << "ERRO: Não foi possível criar o arquivo: " << arquivo_saida << endl;
        return;
    }
    
    // Cabeçalho da tabela
    arquivo << "Tabela 1: Resultados obtidos pelos algoritmos" << endl << endl;
    arquivo << left << setw(20) << "Instancias" 
            << setw(8) << "#V"
            << setw(8) << "best"
            << setw(8) << "Guloso"
            << setw(30) << "Randomizado"
            << setw(12) << "Reativo" << endl;
    arquivo << left << setw(20) << ""
            << setw(8) << ""
            << setw(8) << ""
            << setw(8) << ""
            << setw(10) << "alfa=0,25"
            << setw(10) << "alfa=0,50" 
            << setw(10) << "alfa=0,75"
            << setw(12) << "{0,25;0,50;0,75}" << endl;
    arquivo << string(88, '-') << endl;
    
    // Dados da tabela
    for (const auto& resultado : resultados) {
        // Converte nome da instância para o formato da tabela (remove extensão e ajusta)
        string nome_tabela = resultado.nome;
        nome_tabela = nome_tabela.substr(0, nome_tabela.find(".txt"));
        
        arquivo << left << setw(20) << nome_tabela
                << setw(8) << resultado.num_vertices
                << setw(8) << static_cast<int>(resultado.melhor_known)
                << setw(8) << static_cast<int>(resultado.guloso)
                << setw(10) << static_cast<int>(resultado.randomizado_02)
                << setw(10) << static_cast<int>(resultado.randomizado_03)
                << setw(10) << static_cast<int>(resultado.randomizado_04)
                << setw(12) << static_cast<int>(resultado.reativo) << endl;
    }
    
    arquivo << string(88, '-') << endl;
    arquivo << endl;
    
    // Estatísticas resumidas
    arquivo << "=== RESUMO ESTATÍSTICO ===" << endl;
    
    int vitorias_guloso = 0, vitorias_rand02 = 0, vitorias_rand03 = 0, vitorias_rand04 = 0, vitorias_reativo = 0;
    
    for (const auto& resultado : resultados) {
        float melhor = min({resultado.guloso, resultado.randomizado_02, 
                           resultado.randomizado_03, resultado.randomizado_04, resultado.reativo});
        
        if (resultado.guloso == melhor) vitorias_guloso++;
        if (resultado.randomizado_02 == melhor) vitorias_rand02++;
        if (resultado.randomizado_03 == melhor) vitorias_rand03++;
        if (resultado.randomizado_04 == melhor) vitorias_rand04++;
        if (resultado.reativo == melhor) vitorias_reativo++;
    }
    
    arquivo << "Vitórias por algoritmo:" << endl;
    arquivo << "  Guloso: " << vitorias_guloso << endl;
    arquivo << "  Randomizado (α=0,25): " << vitorias_rand02 << endl;
    arquivo << "  Randomizado (α=0,50): " << vitorias_rand03 << endl;
    arquivo << "  Randomizado (α=0,75): " << vitorias_rand04 << endl;
    arquivo << "  Reativo: " << vitorias_reativo << endl;
    arquivo << "Total de instâncias: " << resultados.size() << endl;
    
    arquivo.close();
    
    cout << "\n=== EXPERIMENTOS CONSOLIDADOS CONCLUÍDOS ===" << endl;
    cout << "Resultados salvos em: " << arquivo_saida << endl;
    cout << "Instâncias processadas: " << resultados.size() << endl;
}

// Função para gerar tabela com diferenças percentuais em relação ao best
void Guloso::executar_experimentos_percentuais(const string& diretorio_instancias, const string& arquivo_saida) {
    // Configurações dos experimentos
    const int NUM_EXECUCOES = 10;
    const int ITER_RANDOMIZADO = 30;
    const int ITER_REATIVO = 300;
    const int BLOCO_REATIVO = 40;
    
    // Valores de alpha conforme especificação
    float alphas_randomizado[3] = {0.25f, 0.5f, 0.75f};
    float alphas_reativo[3] = {0.25f, 0.5f, 0.75f};
    
    // Lista de instâncias a processar
    vector<string> instancias = {
        "g_25_0.16_0_1_0.txt", "g_25_0.16_0_1_1.txt",
        "g_25_0.21_0_1_0.txt", "g_25_0.21_0_1_1.txt", 
        "g_25_0.26_0_1_0.txt", "g_25_0.26_0_1_1.txt",
        "g_40_0.10_0_1_0.txt", "g_40_0.10_0_1_1.txt",
        "g_40_0.15_0_1_0.txt", "g_40_0.15_0_1_1.txt",
        "g_40_0.20_0_1_0.txt", "g_40_0.20_0_1_1.txt",
        "g_60_0.07_0_1_0.txt", "g_60_0.07_0_1_1.txt",
        "g_60_0.12_0_1_0.txt", "g_60_0.12_0_1_1.txt",
        "g_60_0.17_0_1_0.txt", "g_60_0.17_0_1_1.txt"
    };
    
    // Estrutura para armazenar resultados de todas as instâncias
    struct ResultadoInstancia {
        string nome;
        int num_vertices;
        float best_value;
        float guloso;
        float randomizado_02;
        float randomizado_03;
        float randomizado_04;  
        float reativo;
    };
    
    vector<ResultadoInstancia> resultados;
    
    cout << "=== PROCESSANDO INSTÂNCIAS PARA TABELA PERCENTUAL ===" << endl;
    cout << "Total de instâncias: " << instancias.size() << endl;
    
    for (size_t idx = 0; idx < instancias.size(); ++idx) {
        const string& nome_instancia = instancias[idx];
        string caminho_completo = diretorio_instancias + "/" + nome_instancia;
        
        cout << "\nProcessando (" << (idx + 1) << "/" << instancias.size() << "): " << nome_instancia << endl;
        
        // Carregar grafo da instância
        Grafo* grafo = Gerenciador::carregar_informacoes_entrada(caminho_completo.c_str());
        if (!grafo) {
            cout << "ERRO: Não foi possível carregar " << nome_instancia << endl;
            continue;
        }
        
        ResultadoInstancia resultado;
        resultado.nome = nome_instancia;
        resultado.num_vertices = grafo->get_lista_adj().size();
        
        // ========== ALGORITMO GULOSO ADAPTATIVO ==========
        float melhor_guloso = FLT_MAX;
        for (int exec = 0; exec < NUM_EXECUCOES; ++exec) {
            auto [solucao, peso] = algoritmo_guloso_adaptativo(*grafo);
            melhor_guloso = min(melhor_guloso, peso);
        }
        resultado.guloso = melhor_guloso;
        
        // ========== ALGORITMO GULOSO RANDOMIZADO ==========
        vector<float> melhores_randomizado(3, FLT_MAX);
        for (int alpha_idx = 0; alpha_idx < 3; ++alpha_idx) {
            for (int exec = 0; exec < NUM_EXECUCOES; ++exec) {
                auto [solucao, peso] = guloso_randomizado(*grafo, alphas_randomizado[alpha_idx], ITER_RANDOMIZADO);
                melhores_randomizado[alpha_idx] = min(melhores_randomizado[alpha_idx], peso);
            }
        }
        resultado.randomizado_02 = melhores_randomizado[0];
        resultado.randomizado_03 = melhores_randomizado[1]; 
        resultado.randomizado_04 = melhores_randomizado[2];
        
        // ========== ALGORITMO GULOSO RANDOMIZADO REATIVO ==========
        float melhor_reativo = FLT_MAX;
        for (int exec = 0; exec < NUM_EXECUCOES; ++exec) {
            auto [solucao, peso] = guloso_randomizado_reativo(*grafo, alphas_reativo, ITER_REATIVO, BLOCO_REATIVO);
            melhor_reativo = min(melhor_reativo, peso);
        }
        resultado.reativo = melhor_reativo;
        
        // Define best como o menor valor encontrado
        resultado.best_value = min({resultado.guloso, resultado.randomizado_02, 
                                   resultado.randomizado_03, resultado.randomizado_04, resultado.reativo});
        
        resultados.push_back(resultado);
        delete grafo;
    }
    
    // ========== GERAÇÃO DA TABELA COM PERCENTUAIS ==========
    ofstream arquivo(arquivo_saida);
    if (!arquivo.is_open()) {
        cerr << "ERRO: Não foi possível criar o arquivo: " << arquivo_saida << endl;
        return;
    }
    
    // Cabeçalho da tabela
    arquivo << "Tabela 2: Diferenças percentuais em relação ao melhor resultado" << endl << endl;
    arquivo << left << setw(20) << "Instancias" 
            << setw(8) << "#V"
            << setw(8) << "best"
            << setw(8) << "Guloso"
            << setw(30) << "Randomizado"
            << setw(12) << "Reativo" << endl;
    arquivo << left << setw(20) << ""
            << setw(8) << ""
            << setw(8) << ""
            << setw(8) << ""
            << setw(10) << "alfa=0,25"
            << setw(10) << "alfa=0,50" 
            << setw(10) << "alfa=0,75"
            << setw(12) << "{0,25;0,50;0,75}" << endl;
    arquivo << string(88, '-') << endl;
    
    // Variáveis para calcular médias dos percentuais
    double soma_perc_guloso = 0.0, soma_perc_rand02 = 0.0, soma_perc_rand03 = 0.0;
    double soma_perc_rand04 = 0.0, soma_perc_reativo = 0.0;
    int count = 0;
    
    // Função auxiliar para calcular percentual
    auto calcular_percentual = [](float valor, float best) -> double {
        if (best == 0.0f) return 0.0;
        return ((valor - best) / best) * 100.0;
    };
    
    // Dados da tabela
    for (const auto& resultado : resultados) {
        // Converte nome da instância para o formato da tabela (remove extensão e ajusta)
        string nome_tabela = resultado.nome;
        nome_tabela = nome_tabela.substr(0, nome_tabela.find(".txt"));
        
        // Calcula percentuais
        double perc_guloso = calcular_percentual(resultado.guloso, resultado.best_value);
        double perc_rand02 = calcular_percentual(resultado.randomizado_02, resultado.best_value);
        double perc_rand03 = calcular_percentual(resultado.randomizado_03, resultado.best_value);
        double perc_rand04 = calcular_percentual(resultado.randomizado_04, resultado.best_value);
        double perc_reativo = calcular_percentual(resultado.reativo, resultado.best_value);
        
        // Acumula para média
        soma_perc_guloso += perc_guloso;
        soma_perc_rand02 += perc_rand02;
        soma_perc_rand03 += perc_rand03;
        soma_perc_rand04 += perc_rand04;
        soma_perc_reativo += perc_reativo;
        count++;
        
        arquivo << left << setw(20) << nome_tabela
                << setw(8) << resultado.num_vertices
                << setw(8) << static_cast<int>(resultado.best_value)
                << setw(8) << fixed << setprecision(2) << perc_guloso << "%"
                << setw(10) << fixed << setprecision(2) << perc_rand02 << "%"
                << setw(10) << fixed << setprecision(2) << perc_rand03 << "%"
                << setw(10) << fixed << setprecision(2) << perc_rand04 << "%"
                << setw(12) << fixed << setprecision(2) << perc_reativo << "%" << endl;
    }
    
    // Linha das médias
    arquivo << string(88, '-') << endl;
    arquivo << left << setw(20) << "MÉDIA PERCENTUAL"
            << setw(8) << ""
            << setw(8) << ""
            << setw(8) << fixed << setprecision(2) << (soma_perc_guloso / count) << "%"
            << setw(10) << fixed << setprecision(2) << (soma_perc_rand02 / count) << "%"
            << setw(10) << fixed << setprecision(2) << (soma_perc_rand03 / count) << "%"
            << setw(10) << fixed << setprecision(2) << (soma_perc_rand04 / count) << "%"
            << setw(12) << fixed << setprecision(2) << (soma_perc_reativo / count) << "%" << endl;
    
    arquivo << string(88, '-') << endl;
    arquivo << endl;
    
    // Estatísticas adicionais
    arquivo << "=== ANÁLISE ESTATÍSTICA ===" << endl;
    arquivo << "Diferença percentual média por algoritmo:" << endl;
    arquivo << "  Guloso: " << fixed << setprecision(2) << (soma_perc_guloso / count) << "%" << endl;
    arquivo << "  Randomizado (α=0,25): " << fixed << setprecision(2) << (soma_perc_rand02 / count) << "%" << endl;
    arquivo << "  Randomizado (α=0,50): " << fixed << setprecision(2) << (soma_perc_rand03 / count) << "%" << endl;
    arquivo << "  Randomizado (α=0,75): " << fixed << setprecision(2) << (soma_perc_rand04 / count) << "%" << endl;
    arquivo << "  Reativo: " << fixed << setprecision(2) << (soma_perc_reativo / count) << "%" << endl;
    arquivo << "Total de instâncias analisadas: " << count << endl;
    
    arquivo.close();
    
    cout << "\n=== TABELA PERCENTUAL CONCLUÍDA ===" << endl;
    cout << "Resultados salvos em: " << arquivo_saida << endl;
    cout << "Instâncias processadas: " << count << endl;
}

// Função para gerar tabela com tempos de execução
void Guloso::executar_experimentos_tempos(const string& diretorio_instancias, const string& arquivo_saida) {
    using namespace chrono;
    
    // Configurações dos experimentos
    const int NUM_EXECUCOES = 10;
    const int ITER_RANDOMIZADO = 30;
    const int ITER_REATIVO = 300;
    const int BLOCO_REATIVO = 40;
    
    // Valores de alpha conforme especificação
    float alphas_randomizado[3] = {0.25f, 0.5f, 0.75f};
    float alphas_reativo[3] = {0.25f, 0.5f, 0.75f};
    
    // Lista de instâncias a processar
    vector<string> instancias = {
        "g_25_0.16_0_1_0.txt", "g_25_0.16_0_1_1.txt",
        "g_25_0.21_0_1_0.txt", "g_25_0.21_0_1_1.txt", 
        "g_25_0.26_0_1_0.txt", "g_25_0.26_0_1_1.txt",
        "g_40_0.10_0_1_0.txt", "g_40_0.10_0_1_1.txt",
        "g_40_0.15_0_1_0.txt", "g_40_0.15_0_1_1.txt",
        "g_40_0.20_0_1_0.txt", "g_40_0.20_0_1_1.txt",
        "g_60_0.07_0_1_0.txt", "g_60_0.07_0_1_1.txt",
        "g_60_0.12_0_1_0.txt", "g_60_0.12_0_1_1.txt",
        "g_60_0.17_0_1_0.txt", "g_60_0.17_0_1_1.txt"
    };
    
    // Estrutura para armazenar tempos de todas as instâncias
    struct TempoInstancia {
        string nome;
        int num_vertices;
        double melhor_tempo;  // menor tempo encontrado
        double tempo_guloso;
        double tempo_randomizado_02;
        double tempo_randomizado_03;
        double tempo_randomizado_04;
        double tempo_reativo;
    };
    
    vector<TempoInstancia> resultados_tempos;
    
    cout << "=== PROCESSANDO INSTÂNCIAS PARA ANÁLISE DE TEMPOS ===" << endl;
    cout << "Total de instâncias: " << instancias.size() << endl;
    
    for (size_t idx = 0; idx < instancias.size(); ++idx) {
        const string& nome_instancia = instancias[idx];
        string caminho_completo = diretorio_instancias + "/" + nome_instancia;
        
        cout << "\nProcessando (" << (idx + 1) << "/" << instancias.size() << "): " << nome_instancia << endl;
        
        // Carregar grafo da instância
        Grafo* grafo = Gerenciador::carregar_informacoes_entrada(caminho_completo.c_str());
        if (!grafo) {
            cout << "ERRO: Não foi possível carregar " << nome_instancia << endl;
            continue;
        }
        
        TempoInstancia resultado;
        resultado.nome = nome_instancia;
        resultado.num_vertices = grafo->get_lista_adj().size();
        
        // ========== ALGORITMO GULOSO ADAPTATIVO ==========
        double tempo_total_guloso = 0.0;
        for (int exec = 0; exec < NUM_EXECUCOES; ++exec) {
            auto inicio = high_resolution_clock::now();
            auto [solucao, peso] = algoritmo_guloso_adaptativo(*grafo);
            auto fim = high_resolution_clock::now();
            
            auto duracao = duration_cast<microseconds>(fim - inicio);
            tempo_total_guloso += duracao.count() / 1000000.0;  // converter para segundos
        }
        resultado.tempo_guloso = tempo_total_guloso / NUM_EXECUCOES;  // tempo médio
        
        // ========== ALGORITMO GULOSO RANDOMIZADO ==========
        vector<double> tempos_randomizado(3, 0.0);
        for (int alpha_idx = 0; alpha_idx < 3; ++alpha_idx) {
            for (int exec = 0; exec < NUM_EXECUCOES; ++exec) {
                auto inicio = high_resolution_clock::now();
                auto [solucao, peso] = guloso_randomizado(*grafo, alphas_randomizado[alpha_idx], ITER_RANDOMIZADO);
                auto fim = high_resolution_clock::now();
                
                auto duracao = duration_cast<microseconds>(fim - inicio);
                tempos_randomizado[alpha_idx] += duracao.count() / 1000000.0;
            }
            tempos_randomizado[alpha_idx] /= NUM_EXECUCOES;  // tempo médio
        }
        resultado.tempo_randomizado_02 = tempos_randomizado[0];
        resultado.tempo_randomizado_03 = tempos_randomizado[1];
        resultado.tempo_randomizado_04 = tempos_randomizado[2];
        
        // ========== ALGORITMO GULOSO RANDOMIZADO REATIVO ==========
        double tempo_total_reativo = 0.0;
        for (int exec = 0; exec < NUM_EXECUCOES; ++exec) {
            auto inicio = high_resolution_clock::now();
            auto [solucao, peso] = guloso_randomizado_reativo(*grafo, alphas_reativo, ITER_REATIVO, BLOCO_REATIVO);
            auto fim = high_resolution_clock::now();
            
            auto duracao = duration_cast<microseconds>(fim - inicio);
            tempo_total_reativo += duracao.count() / 1000000.0;
        }
        resultado.tempo_reativo = tempo_total_reativo / NUM_EXECUCOES;  // tempo médio
        
        // Define melhor tempo como o menor encontrado
        resultado.melhor_tempo = min({resultado.tempo_guloso, resultado.tempo_randomizado_02,
                                     resultado.tempo_randomizado_03, resultado.tempo_randomizado_04,
                                     resultado.tempo_reativo});
        
        resultados_tempos.push_back(resultado);
        
        cout << "  Tempos médios (s): Guloso=" << fixed << setprecision(4) << resultado.tempo_guloso
             << " | Random=" << resultado.tempo_randomizado_02 << ", " << resultado.tempo_randomizado_03 
             << ", " << resultado.tempo_randomizado_04 << " | Reativo=" << resultado.tempo_reativo << endl;
        
        delete grafo;
    }
    
    // ========== GERAÇÃO DA TABELA DE TEMPOS ==========
    ofstream arquivo(arquivo_saida);
    if (!arquivo.is_open()) {
        cerr << "ERRO: Não foi possível criar o arquivo: " << arquivo_saida << endl;
        return;
    }
    
    // Cabeçalho da tabela
    arquivo << "Tabela 3: Tempos de execução dos algoritmos (em segundos)" << endl << endl;
    arquivo << left << setw(20) << "Instancias" 
            << setw(8) << "#V"
            << setw(12) << "best(s)"
            << setw(12) << "Guloso(s)"
            << setw(36) << "Randomizado"
            << setw(12) << "Reativo(s)" << endl;
    arquivo << left << setw(20) << ""
            << setw(8) << ""
            << setw(12) << ""
            << setw(12) << ""
            << setw(12) << "alfa=0,25"
            << setw(12) << "alfa=0,50" 
            << setw(12) << "alfa=0,75"
            << setw(12) << "{0,25;0,50;0,75}" << endl;
    arquivo << string(100, '-') << endl;
    
    // Variáveis para calcular médias dos tempos
    double soma_tempo_guloso = 0.0, soma_tempo_rand02 = 0.0, soma_tempo_rand03 = 0.0;
    double soma_tempo_rand04 = 0.0, soma_tempo_reativo = 0.0, soma_melhor_tempo = 0.0;
    int count = 0;
    
    // Dados da tabela
    for (const auto& resultado : resultados_tempos) {
        // Converte nome da instância para o formato da tabela (remove extensão e ajusta)
        string nome_tabela = resultado.nome;
        nome_tabela = nome_tabela.substr(0, nome_tabela.find(".txt"));
        
        // Acumula para média
        soma_melhor_tempo += resultado.melhor_tempo;
        soma_tempo_guloso += resultado.tempo_guloso;
        soma_tempo_rand02 += resultado.tempo_randomizado_02;
        soma_tempo_rand03 += resultado.tempo_randomizado_03;
        soma_tempo_rand04 += resultado.tempo_randomizado_04;
        soma_tempo_reativo += resultado.tempo_reativo;
        count++;
        
        arquivo << left << setw(20) << nome_tabela
                << setw(8) << resultado.num_vertices
                << setw(12) << fixed << setprecision(4) << resultado.melhor_tempo
                << setw(12) << fixed << setprecision(4) << resultado.tempo_guloso
                << setw(12) << fixed << setprecision(4) << resultado.tempo_randomizado_02
                << setw(12) << fixed << setprecision(4) << resultado.tempo_randomizado_03
                << setw(12) << fixed << setprecision(4) << resultado.tempo_randomizado_04
                << setw(12) << fixed << setprecision(4) << resultado.tempo_reativo << endl;
    }
    
    // Linha das médias
    arquivo << string(100, '-') << endl;
    arquivo << left << setw(20) << "TEMPO MÉDIO (s)"
            << setw(8) << ""
            << setw(12) << fixed << setprecision(4) << (soma_melhor_tempo / count)
            << setw(12) << fixed << setprecision(4) << (soma_tempo_guloso / count)
            << setw(12) << fixed << setprecision(4) << (soma_tempo_rand02 / count)
            << setw(12) << fixed << setprecision(4) << (soma_tempo_rand03 / count)
            << setw(12) << fixed << setprecision(4) << (soma_tempo_rand04 / count)
            << setw(12) << fixed << setprecision(4) << (soma_tempo_reativo / count) << endl;
    
    arquivo << string(100, '-') << endl;
    arquivo << endl;
    
    // Estatísticas de desempenho
    arquivo << "=== ANÁLISE DE DESEMPENHO TEMPORAL ===" << endl;
    arquivo << "Tempo médio de execução por algoritmo:" << endl;
    arquivo << "  Guloso: " << fixed << setprecision(4) << (soma_tempo_guloso / count) << " segundos" << endl;
    arquivo << "  Randomizado (α=0,25): " << fixed << setprecision(4) << (soma_tempo_rand02 / count) << " segundos" << endl;
    arquivo << "  Randomizado (α=0,50): " << fixed << setprecision(4) << (soma_tempo_rand03 / count) << " segundos" << endl;
    arquivo << "  Randomizado (α=0,75): " << fixed << setprecision(4) << (soma_tempo_rand04 / count) << " segundos" << endl;
    arquivo << "  Reativo: " << fixed << setprecision(4) << (soma_tempo_reativo / count) << " segundos" << endl;
    arquivo << "Melhor tempo médio: " << fixed << setprecision(4) << (soma_melhor_tempo / count) << " segundos" << endl;
    
    // Análise de velocidade relativa
    double tempo_base = soma_tempo_guloso / count;
    arquivo << "\nVelocidade relativa ao Guloso:" << endl;
    arquivo << "  Randomizado (α=0,25): " << fixed << setprecision(2) << ((soma_tempo_rand02 / count) / tempo_base) << "x" << endl;
    arquivo << "  Randomizado (α=0,50): " << fixed << setprecision(2) << ((soma_tempo_rand03 / count) / tempo_base) << "x" << endl;
    arquivo << "  Randomizado (α=0,75): " << fixed << setprecision(2) << ((soma_tempo_rand04 / count) / tempo_base) << "x" << endl;
    arquivo << "  Reativo: " << fixed << setprecision(2) << ((soma_tempo_reativo / count) / tempo_base) << "x" << endl;
    
    arquivo << "Total de instâncias analisadas: " << count << endl;
    
    arquivo.close();
    
    cout << "\n=== ANÁLISE DE TEMPOS CONCLUÍDA ===" << endl;
    cout << "Resultados salvos em: " << arquivo_saida << endl;
    cout << "Instâncias processadas: " << count << endl;
}   