#include "Guloso.h"
#include "Gerenciador.h"
#include <vector>
#include <unordered_set>
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

        // ETAPA 1: INICIALIZAÇÃO DE ESTRUTURAS DE CONTROLE
        const auto& nos = grafo.get_lista_adj();
        const int num_nos = nos.size();
        
        vector<char> solucao;                     // Conjunto dominante final
        unordered_set<char> nos_usados;          // Nós já incluídos na solução
        unordered_set<char> nos_dominados;       // Nós já dominados
        nos_usados.reserve(num_nos);
        nos_dominados.reserve(num_nos);
        float peso_total = 0.0;                  // Rastreia peso total da solução
        
        // Dominações pré-computadas para evitar recálculos
        vector<unordered_set<char>> cache_dominacao(num_nos);
        for (int i = 0; i < num_nos; ++i) {
            No* no = nos[i];
            cache_dominacao[i].insert(no->get_id());
            for (Aresta* aresta : no->get_arestas()) {
                cache_dominacao[i].insert(aresta->get_id_destino());
            }
        }
        
        // ETAPA 2: LOOP DE CONSTRUÇÃO DA POSSÍVEL SOLUÇÃO
        while(nos_dominados.size() < num_nos) {
            No* melhor_candidato = nullptr;
            float melhor_beneficio = 0.0;
            
            // Avalia todos os nós não utilizados usando cache
            for(int i = 0; i < num_nos; ++i) {
                No* no = nos[i];
                if(nos_usados.find(no->get_id()) != nos_usados.end())
                    continue;
                
                // Inclusão imediata de nós peso zero
                if(no->get_peso() == 0) {
                    solucao.push_back(no->get_id());
                    nos_usados.insert(no->get_id());
                    peso_total += no->get_peso();
                    
                    for(char v : cache_dominacao[i]) {
                        nos_dominados.insert(v);
                    }
                    continue;
                }
                
                // Cálculo de novos dominados
                int novos_dominados = 0;
                for(char v : cache_dominacao[i]) {
                    if(nos_dominados.find(v) == nos_dominados.end()) {
                        novos_dominados++;
                    }
                }
                if(novos_dominados == 0) continue;
                
                // Heuristicamente, o benefício é a razão entre novos dominados e peso
                float beneficio = (float)novos_dominados / no->get_peso();
                
                if(beneficio > melhor_beneficio) {
                    melhor_beneficio = beneficio;
                    melhor_candidato = no;
                }
            }
            
            if(melhor_candidato == nullptr)
                break;
            
            // Atualização da solução
            solucao.push_back(melhor_candidato->get_id());
            nos_usados.insert(melhor_candidato->get_id());
            peso_total += melhor_candidato->get_peso();
            
            // Atualização dos dominados
            for (int i = 0; i < num_nos; ++i) {
                if (nos[i] == melhor_candidato) {
                    for (char v : cache_dominacao[i]) {
                        nos_dominados.insert(v);
                    }
                    break;
                }
            }
        }

    return {solucao, peso_total};
}

// Método auxiliar para construção dos randomizados
pair<vector<char>, float> Guloso::construir_solucao_grasp(Grafo &grafo, float alpha) {
    if (grafo.get_lista_adj().empty() || alpha < 0.0f || alpha > 1.0f)
        return {{}, 0.0f};
    
    // ETAPA 1: INICIALIZAÇÃO DE ESTRUTURAS DE CONTROLE
    const auto& nos = grafo.get_lista_adj();
    const int num_nos = nos.size();
    
    unordered_set<char> nos_usados;
    unordered_set<char> nos_dominados;
    nos_usados.reserve(num_nos);
    nos_dominados.reserve(num_nos);
    
    vector<char> solucao;
    solucao.reserve(num_nos);
    
    vector<pair<No*, float>> candidatos;
    candidatos.reserve(num_nos);
    
    vector<No*> LRC;
    LRC.reserve(num_nos);
    
    float peso_total = 0.0;
    
    // Dominações pré-computadas para evitar recálculos
    vector<unordered_set<char>> cache_dominacao(num_nos);
    for (int i = 0; i < num_nos; ++i) {
        No* no = nos[i];
        cache_dominacao[i].insert(no->get_id());
        for (Aresta* aresta : no->get_arestas()) {
            cache_dominacao[i].insert(aresta->get_id_destino());
        }
    }
    
    // ETAPA 2: LOOP DE CONSTRUÇÃO DA POSSÍVEL SOLUÇÃO
    while (nos_dominados.size() < num_nos) {
        candidatos.clear();
        float max_beneficio = 0.0f;
        float min_beneficio = FLT_MAX;
        
        for (int i = 0; i < num_nos; ++i) {
            No* no = nos[i];
            
            if (nos_usados.find(no->get_id()) != nos_usados.end())
                continue;
            
            // Inclusão imediata de nós peso zero
            if (no->get_peso() == 0.0f) {
                solucao.push_back(no->get_id());
                nos_usados.insert(no->get_id());
                peso_total += no->get_peso();
                
                for (char v : cache_dominacao[i]) {
                    nos_dominados.insert(v);
                }
                continue;
            }
            
            // Cálculo de novos dominados
            int novos_dominados = 0;
            for (char v : cache_dominacao[i]) {
                if (nos_dominados.find(v) == nos_dominados.end()) {
                    novos_dominados++;
                }
            }
            if (novos_dominados == 0) continue;
            
            // Heuristicamente, o benefício é a razão entre novos dominados e peso
            float beneficio = (float)novos_dominados / no->get_peso();
            candidatos.push_back({no, beneficio});
            
            // Atualizações incrementais de min/max
            if (beneficio > max_beneficio) max_beneficio = beneficio;
            if (beneficio < min_beneficio) min_beneficio = beneficio;
        }
        
        if (candidatos.empty()) break;
        
        // Construção da LRC
        LRC.clear();
        if (max_beneficio == min_beneficio) {
            for (const auto& candidato : candidatos) {
                LRC.push_back(candidato.first);
            }
        } else {
            float limite = max_beneficio - alpha * (max_beneficio - min_beneficio);
            for (const auto& candidato : candidatos) {
                if (candidato.second >= limite) {
                    LRC.push_back(candidato.first);
                }
            }
        }
        
        if (LRC.empty()) break;
        
        No* escolhido = LRC[rand() % LRC.size()];
        
        // Atualização da solução
        solucao.push_back(escolhido->get_id());
        nos_usados.insert(escolhido->get_id());
        peso_total += escolhido->get_peso();
        
        // Atualização dos dominados
        for (int i = 0; i < num_nos; ++i) {
            if (nos[i] == escolhido) {
                for (char v : cache_dominacao[i]) {
                    nos_dominados.insert(v);
                }
                break;
            }
        }
    }
    
    return {solucao, peso_total};
}

pair<vector<char>, float> Guloso::guloso_randomizado(Grafo &grafo, float alpha, int n_iteracoes) {
    if (grafo.get_lista_adj().empty() || n_iteracoes <= 0 || alpha < 0.0f || alpha > 1.0f)
        return {{}, 0.0f};
        
    vector<char> melhor_solucao;
    float melhor_peso = FLT_MAX;
    
    //Itera diversas vezes para encontrar a melhor solução
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
    
    // ETAPA 1: INICIALIZAÇÃO DE CONSTANTES E ESTRUTURAS DE CONTROLE
    const int num_alphas = 3;
    const double prob_inicial = 1.0 / num_alphas;
    const double epsilon = 1e-9;

    vector<int> contador(num_alphas, 0);
    vector<double> qualidade_total(num_alphas, 0.0);
    vector<double> probabilidades(num_alphas, 1.0 / num_alphas);
    
    vector<char> melhor_solucao;
    float melhor_peso = FLT_MAX;
    
    // ETAPA 2: LOOP PRINCIPAL DE ITERAÇÕES
    for (int iter = 0; iter < n_iteracoes; ++iter) {
        // Atualização de probabilidades a cada bloco
        if ((iter + 1) % bloco == 0 && iter > 0) {
            vector<double> medias_qualidade(num_alphas, 0.0);
            double soma_medias = 0.0;
            int alphas_com_dados = 0;
            
            // Calcula médias de qualidade por alfa
            for (int j = 0; j < num_alphas; ++j) {
                if (contador[j] > 0) {
                    medias_qualidade[j] = qualidade_total[j] / contador[j];
                    soma_medias += medias_qualidade[j];
                    alphas_com_dados++;
                }
            }
            
            // Atualiza probabilidades apenas se há dados suficientes
            if (soma_medias > epsilon && alphas_com_dados > 0) {
                for (int j = 0; j < num_alphas; ++j) {
                    if (contador[j] > 0) {
                        probabilidades[j] = medias_qualidade[j] / soma_medias;
                    } else {
                        // Mantém probabilidade mínima para exploração
                        probabilidades[j] = 0.01;
                    }
                }
                
                // Normaliza probabilidades
                double soma_prob = 0.0;
                for (int j = 0; j < num_alphas; ++j) {
                    soma_prob += probabilidades[j];
                }
                if (soma_prob > epsilon) {
                    for (int j = 0; j < num_alphas; ++j) {
                        probabilidades[j] /= soma_prob;
                    }
                }
            } else {
                // Reset para distribuição uniforme se não há dados suficientes
                fill(probabilidades.begin(), probabilidades.end(), prob_inicial);
            }
        }
        
        // Seleção probabilística de alfa
        double rand_val = (double)rand() / RAND_MAX;
        int alpha_idx = 0;
        double acumulado = 0.0;
        
        for (int j = 0; j < num_alphas; ++j) {
            acumulado += probabilidades[j];
            if (rand_val <= acumulado) {
                alpha_idx = j;
                break;
            }
        }
        
        auto [solucao, peso_total] = construir_solucao_grasp(grafo, alpha[alpha_idx]);
        
        // Atualização dados
        if (!solucao.empty()) {
            if (peso_total < melhor_peso) {
                melhor_peso = peso_total;
                melhor_solucao = solucao;
            }
            
            contador[alpha_idx]++;
            
            // Atualização das qualidades baseada na significância do peso
            if (peso_total > epsilon) {
                qualidade_total[alpha_idx] += 1.0 / peso_total;
            } else {
                qualidade_total[alpha_idx] += 1.0 / epsilon;
            }
        }
    }
    
    return {melhor_solucao, melhor_peso};
}   

// Função para gerar tabela com melhores qualidades diferenças percentuais em relação ao best
void Guloso::executar_experimentos_melhores(const string& diretorio_instancias, const string& arquivo_saida) {
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
        int num_nos;
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
        resultado.num_nos = grafo->get_lista_adj().size();
        
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
                << setw(8) << resultado.num_nos
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
        int num_nos;
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
        resultado.num_nos = grafo->get_lista_adj().size();
        
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
                << setw(8) << resultado.num_nos
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

// Função para gerar tabela com médias e diferenças percentuais em relação ao best
void Guloso::executar_experimentos_medias(const string& diretorio_instancias, const string& arquivo_saida) {
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
    
    // Estrutura para armazenar médias de todas as instâncias
    struct MediaInstancia {
        string nome;
        int num_nos;
        float best_media;
        float media_guloso;
        float media_randomizado_02;
        float media_randomizado_03;
        float media_randomizado_04;  
        float media_reativo;
    };
    
    vector<MediaInstancia> resultados_medias;
    
    cout << "=== PROCESSANDO INSTÂNCIAS PARA ANÁLISE DE MÉDIAS ===" << endl;
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
        
        MediaInstancia resultado;
        resultado.nome = nome_instancia;
        resultado.num_nos = grafo->get_lista_adj().size();
        
        // ========== ALGORITMO GULOSO ADAPTATIVO - CALCULAR MÉDIA ==========
        float soma_guloso = 0.0f;
        for (int exec = 0; exec < NUM_EXECUCOES; ++exec) {
            auto [solucao, peso] = algoritmo_guloso_adaptativo(*grafo);
            soma_guloso += peso;
        }
        resultado.media_guloso = soma_guloso / NUM_EXECUCOES;
        
        // ========== ALGORITMO GULOSO RANDOMIZADO - CALCULAR MÉDIAS ==========
        vector<float> somas_randomizado(3, 0.0f);
        for (int alpha_idx = 0; alpha_idx < 3; ++alpha_idx) {
            for (int exec = 0; exec < NUM_EXECUCOES; ++exec) {
                auto [solucao, peso] = guloso_randomizado(*grafo, alphas_randomizado[alpha_idx], ITER_RANDOMIZADO);
                somas_randomizado[alpha_idx] += peso;
            }
        }
        resultado.media_randomizado_02 = somas_randomizado[0] / NUM_EXECUCOES;
        resultado.media_randomizado_03 = somas_randomizado[1] / NUM_EXECUCOES; 
        resultado.media_randomizado_04 = somas_randomizado[2] / NUM_EXECUCOES;
        
        // ========== ALGORITMO GULOSO RANDOMIZADO REATIVO - CALCULAR MÉDIA ==========
        float soma_reativo = 0.0f;
        for (int exec = 0; exec < NUM_EXECUCOES; ++exec) {
            auto [solucao, peso] = guloso_randomizado_reativo(*grafo, alphas_reativo, ITER_REATIVO, BLOCO_REATIVO);
            soma_reativo += peso;
        }
        resultado.media_reativo = soma_reativo / NUM_EXECUCOES;
        
        // Define best como a menor média encontrada
        resultado.best_media = min({resultado.media_guloso, resultado.media_randomizado_02, 
                                   resultado.media_randomizado_03, resultado.media_randomizado_04, 
                                   resultado.media_reativo});
        
        resultados_medias.push_back(resultado);
        
        cout << "  Médias: Guloso=" << fixed << setprecision(2) << resultado.media_guloso
             << " | Random=" << resultado.media_randomizado_02 << ", " << resultado.media_randomizado_03 
             << ", " << resultado.media_randomizado_04 << " | Reativo=" << resultado.media_reativo 
             << " | Best=" << resultado.best_media << endl;
        
        delete grafo;
    }
    
    // ========== GERAÇÃO DA TABELA COM DIFERENÇAS PERCENTUAIS DAS MÉDIAS ==========
    ofstream arquivo(arquivo_saida);
    if (!arquivo.is_open()) {
        cerr << "ERRO: Não foi possível criar o arquivo: " << arquivo_saida << endl;
        return;
    }
    
    // Cabeçalho da tabela
    arquivo << "Tabela 4: Diferenças percentuais das médias em relação ao melhor resultado médio" << endl << endl;
    arquivo << left << setw(20) << "Instancias" 
            << setw(8) << "#V"
            << setw(12) << "best_media"
            << setw(12) << "Guloso"
            << setw(36) << "Randomizado"
            << setw(12) << "Reativo" << endl;
    arquivo << left << setw(20) << ""
            << setw(8) << ""
            << setw(12) << ""
            << setw(12) << ""
            << setw(12) << "alfa=0,25"
            << setw(12) << "alfa=0,50" 
            << setw(12) << "alfa=0,75"
            << setw(12) << "{0,25;0,50;0,75}" << endl;
    arquivo << string(100, '-') << endl;
    
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
    for (const auto& resultado : resultados_medias) {
        // Converte nome da instância para o formato da tabela (remove extensão e ajusta)
        string nome_tabela = resultado.nome;
        nome_tabela = nome_tabela.substr(0, nome_tabela.find(".txt"));
        
        // Calcula percentuais das médias em relação ao best
        double perc_guloso = calcular_percentual(resultado.media_guloso, resultado.best_media);
        double perc_rand02 = calcular_percentual(resultado.media_randomizado_02, resultado.best_media);
        double perc_rand03 = calcular_percentual(resultado.media_randomizado_03, resultado.best_media);
        double perc_rand04 = calcular_percentual(resultado.media_randomizado_04, resultado.best_media);
        double perc_reativo = calcular_percentual(resultado.media_reativo, resultado.best_media);
        
        // Acumula para média geral dos percentuais
        soma_perc_guloso += perc_guloso;
        soma_perc_rand02 += perc_rand02;
        soma_perc_rand03 += perc_rand03;
        soma_perc_rand04 += perc_rand04;
        soma_perc_reativo += perc_reativo;
        count++;
        
        arquivo << left << setw(20) << nome_tabela
                << setw(8) << resultado.num_nos
                << setw(12) << fixed << setprecision(2) << resultado.best_media
                << setw(12) << fixed << setprecision(2) << perc_guloso << "%"
                << setw(12) << fixed << setprecision(2) << perc_rand02 << "%"
                << setw(12) << fixed << setprecision(2) << perc_rand03 << "%"
                << setw(12) << fixed << setprecision(2) << perc_rand04 << "%"
                << setw(12) << fixed << setprecision(2) << perc_reativo << "%" << endl;
    }
    
    // Linha das médias dos percentuais
    arquivo << string(100, '-') << endl;
    arquivo << left << setw(20) << "MÉDIA PERCENTUAL"
            << setw(8) << ""
            << setw(12) << ""
            << setw(12) << fixed << setprecision(2) << (soma_perc_guloso / count) << "%"
            << setw(12) << fixed << setprecision(2) << (soma_perc_rand02 / count) << "%"
            << setw(12) << fixed << setprecision(2) << (soma_perc_rand03 / count) << "%"
            << setw(12) << fixed << setprecision(2) << (soma_perc_rand04 / count) << "%"
            << setw(12) << fixed << setprecision(2) << (soma_perc_reativo / count) << "%" << endl;
    
    arquivo << string(100, '-') << endl;
    arquivo << endl;
    
    // Estatísticas das médias
    arquivo << "=== ANÁLISE ESTATÍSTICA DAS MÉDIAS ===" << endl;
    arquivo << "Diferença percentual média por algoritmo (em relação ao best médio):" << endl;
    arquivo << "  Guloso: " << fixed << setprecision(2) << (soma_perc_guloso / count) << "%" << endl;
    arquivo << "  Randomizado (α=0,25): " << fixed << setprecision(2) << (soma_perc_rand02 / count) << "%" << endl;
    arquivo << "  Randomizado (α=0,50): " << fixed << setprecision(2) << (soma_perc_rand03 / count) << "%" << endl;
    arquivo << "  Randomizado (α=0,75): " << fixed << setprecision(2) << (soma_perc_rand04 / count) << "%" << endl;
    arquivo << "  Reativo: " << fixed << setprecision(2) << (soma_perc_reativo / count) << "%" << endl;
    
    // Contagem de vitórias baseada nas médias
    int vitorias_guloso = 0, vitorias_rand02 = 0, vitorias_rand03 = 0, vitorias_rand04 = 0, vitorias_reativo = 0;
    
    for (const auto& resultado : resultados_medias) {
        float melhor_media = min({resultado.media_guloso, resultado.media_randomizado_02, 
                                 resultado.media_randomizado_03, resultado.media_randomizado_04, 
                                 resultado.media_reativo});
        
        if (abs(resultado.media_guloso - melhor_media) < 1e-6) vitorias_guloso++;
        if (abs(resultado.media_randomizado_02 - melhor_media) < 1e-6) vitorias_rand02++;
        if (abs(resultado.media_randomizado_03 - melhor_media) < 1e-6) vitorias_rand03++;
        if (abs(resultado.media_randomizado_04 - melhor_media) < 1e-6) vitorias_rand04++;
        if (abs(resultado.media_reativo - melhor_media) < 1e-6) vitorias_reativo++;
    }
    
    arquivo << "\nVitórias por algoritmo (baseado nas médias):" << endl;
    arquivo << "  Guloso: " << vitorias_guloso << endl;
    arquivo << "  Randomizado (α=0,25): " << vitorias_rand02 << endl;
    arquivo << "  Randomizado (α=0,50): " << vitorias_rand03 << endl;
    arquivo << "  Randomizado (α=0,75): " << vitorias_rand04 << endl;
    arquivo << "  Reativo: " << vitorias_reativo << endl;
    arquivo << "Total de instâncias analisadas: " << count << endl;
    
    // Valores médios absolutos
    arquivo << "\nValores médios absolutos por algoritmo:" << endl;
    double soma_media_guloso = 0.0, soma_media_rand02 = 0.0, soma_media_rand03 = 0.0;
    double soma_media_rand04 = 0.0, soma_media_reativo = 0.0, soma_best_media = 0.0;
    
    for (const auto& resultado : resultados_medias) {
        soma_media_guloso += resultado.media_guloso;
        soma_media_rand02 += resultado.media_randomizado_02;
        soma_media_rand03 += resultado.media_randomizado_03;
        soma_media_rand04 += resultado.media_randomizado_04;
        soma_media_reativo += resultado.media_reativo;
        soma_best_media += resultado.best_media;
    }
    
    arquivo << "  Best médio geral: " << fixed << setprecision(2) << (soma_best_media / count) << endl;
    arquivo << "  Guloso médio geral: " << fixed << setprecision(2) << (soma_media_guloso / count) << endl;
    arquivo << "  Randomizado (α=0,25) médio geral: " << fixed << setprecision(2) << (soma_media_rand02 / count) << endl;
    arquivo << "  Randomizado (α=0,50) médio geral: " << fixed << setprecision(2) << (soma_media_rand03 / count) << endl;
    arquivo << "  Randomizado (α=0,75) médio geral: " << fixed << setprecision(2) << (soma_media_rand04 / count) << endl;
    arquivo << "  Reativo médio geral: " << fixed << setprecision(2) << (soma_media_reativo / count) << endl;
    
    arquivo.close();
    
    cout << "\n=== ANÁLISE DE MÉDIAS CONCLUÍDA ===" << endl;
    cout << "Resultados salvos em: " << arquivo_saida << endl;
    cout << "Instâncias processadas: " << count << endl;
}   

