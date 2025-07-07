#include "No.h"
#include <iostream>

using namespace std;

No::No(char id, int peso) {
    set_id(id);
    set_peso(peso);
}

No::~No() {
    if(!get_arestas().empty()){
        for(int i = 0; i < get_arestas().size(); i++){
            delete get_arestas()[i];
        }
    }
}

char No::get_id() {
    return this->id;
}

void No::set_id(char id) {
    this->id = id;
}

int No::get_peso() {
    return this->peso;
}

void No::set_peso(int peso) {
    this->peso = peso;
}

vector<Aresta*> No::get_arestas() {
    return this->arestas;
}

void No::adicionar_aresta(Aresta* aresta){
    this->arestas.push_back(aresta);
}