#ifndef NO_H
#define NO_H

#include <string>
#include <vector>
#include "Aresta.h"


using namespace std;
class No {
public:
    No(char id, int peso);
    ~No();

    char get_id();
    void set_id(char id);

    int get_peso();
    void set_peso(int peso);

    vector<Aresta*> get_arestas();
    void adicionar_aresta(Aresta* aresta);

private:
    char id;
    int peso;
    vector<Aresta*> arestas;
};



#endif //NO_H
