#ifndef ARESTA_H
#define ARESTA_H

using namespace std;
class Aresta {
public:
    Aresta(char id_origem, char id_destino, int peso, bool arco);
    ~Aresta();

    char get_id_origem();
    void set_id_origem(char id);
    
    char get_id_destino();
    void set_id_destino(char id);
    
    int get_peso();
    void set_peso(int p);

    bool is_arco();
    void set_arco(bool a);

private:
    char id_origem;
    char id_destino;
    int peso;
    bool arco;
};

#endif //ARESTA_H
