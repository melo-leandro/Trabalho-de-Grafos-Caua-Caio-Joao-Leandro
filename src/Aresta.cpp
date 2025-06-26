#include "Aresta.h"

Aresta::Aresta(char id_destino, int peso) {
    set_id_destino(id_destino);
    set_peso(peso);
}

Aresta::~Aresta(){}

char Aresta::get_id_destino() {
    return this->id_destino;
}

void Aresta::set_id_destino(char id) {
    this->id_destino = id;
}

int Aresta::get_peso() {
    return this->peso;
}

void Aresta::set_peso(int p) {
    this->peso = p;
}