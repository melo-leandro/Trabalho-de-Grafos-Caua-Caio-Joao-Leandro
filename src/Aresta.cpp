#include "Aresta.h"

Aresta::Aresta(char id_origem, char id_destino, int peso, bool arco) {
    set_id_origem(id_origem);
    set_id_destino(id_destino);
    set_peso(peso);
    set_arco(arco);
}

Aresta::~Aresta(){}

char Aresta::get_id_origem() {
    return this->id_origem;
}

void Aresta::set_id_origem(char id) {
    this->id_origem = id;
}

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

bool Aresta::is_arco() {
    return this->arco;
}

void Aresta::set_arco(bool a) {
    this->arco = a;
}