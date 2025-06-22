#ifndef ARESTA_H
#define ARESTA_H

using namespace std;
class Aresta {
public:
    Aresta(char id_origem, char id_destino, int peso);
    ~Aresta();

    char id_origem;
    char id_destino;
    int peso;
};

#endif //ARESTA_H
