#ifndef ARESTA_H
#define ARESTA_H

using namespace std;
class Aresta {
public:
    Aresta(char id_destino, int peso);
    ~Aresta();
    
    char get_id_destino();
    void set_id_destino(char id);
    
    int get_peso();
    void set_peso(int p);

private:
    char id_destino;
    int peso;
};

#endif //ARESTA_H
